<html><!-- sample image search engine, part of the libpuzzle package -->
<head>
</head>
<body>
<h1>Similar images finder using <a href="http://libpuzzle.pureftpd.org">libpuzzle</a></h1>
<?php

error_reporting(E_ALL);

require_once 'config.inc.php';
require_once 'similar.inc.php';

function display_form() {
    echo '<form action="' . htmlspecialchars($_SERVER['REQUEST_URI']) . '" ' .
      'method="POST">' . "\n";
    echo 'Enter an image URL (http only):' . "\n";
    echo '<input type="text" size="100" value="" autocomplete="off" name="url" />' . "\n";
    echo '<input type="submit" />';
    echo '</form>' . "\n";
}

function display_error($err) {
    echo '<div id="err"><strong>' . htmlspecialchars($err) . '</strong></div>' . "\n";
}

function display_loading() {
    echo '<div id="loading">Loading...</div>' . "\n";
    @ob_flush(); flush();
}

function display_loaded() {
    echo '<div id="loaded">Loaded.</div>' . "\n";
    @ob_flush(); flush();
}

function display_signature_ok() {
    echo '<div id="sig-ok">Signature computed.</div>' . "\n";
    @ob_flush(); flush();
}

function remove_tmpfile($file) {
    @unlink($file);
}

function get_client_info() {
    return @$_SERVER['REMOTE_ADDR'] . '/' . time();
}

function display_similar_pictures($urls) {
    echo '<div id="images">' . "\n";
    foreach ($urls as $url) {
	echo '<a href="' . htmlentities($url) . '" ' .
	  'onclick="window.open(this.href); return false;">';
	echo '  <img src="' . htmlentities($url) . '" alt="" />';
	echo '</a>' . "\n";
							  
    }
    echo '</div>' . "\n";
}

function record_url($url, &$md5, &$cvec) {
    if (function_exists('sys_get_temp_dir')) {
        $tmpdir = sys_get_temp_dir();
    } else {
        $tmpdir = '/tmp';
    }
    $dfn = tempnam($tmpdir, 'similar-' . md5(uniqid(mt_rand(), TRUE)));
    register_shutdown_function('remove_tmpfile', $dfn);
    if (($dfp = fopen($dfn, 'w')) == FALSE) {
        display_form();
        display_error('Unable to create the temporary file');
        return FALSE;    
    }
    if (($fp = fopen($url, 'r')) == FALSE) {
        display_form();
        display_error('Unable to open: [' . $url . ']');
        return FALSE;
    }
    $f = fread($fp, 4096);
    $written = strlen($f);
    if (empty($f)) {
        display_form();
        display_error('Unable to load: [' . $url . ']');
        return FALSE;    
    }
    fwrite($dfp, $f);
    $infos = @getimagesize($dfn);
    if (empty($infos) ||
        ($infos[2] !== IMAGETYPE_GIF && $infos[2] !== IMAGETYPE_JPEG &&
         $infos[2] !== IMAGETYPE_PNG) ||
        $infos[0] < 50 || $infos[1] < 50) {
        fclose($dfp);
        display_form();
        display_error('Unsupported image format');
        return FALSE;    
    }
    fseek($dfp, strlen($f));
    while (!feof($fp)) {
        $max = MAX_IMAGE_SIZE - $written;
        if ($max > 65536) {
            $max = 65536;
        }
        $t = fread($fp, $max);
        fwrite($dfp, $t);
        $written += strlen($t);
        if ($written > MAX_IMAGE_SIZE) {
            fclose($dfp);
            display_form();
            display_error('File too large');
            return FALSE;
        }
    }
    unset($t);
    fclose($dfp);
    display_loaded();
    $md5 = @md5_file($dfn);
    if (empty($md5)) {
        display_form();
        display_error('Unable to get the MD5 of the file');
        return FALSE;        
    }
    $cvec = puzzle_fill_cvec_from_file($dfn);
    if (empty($cvec)) {
        display_form();
        display_error('Unable to compute image signature');
        return FALSE;    
    }
    display_signature_ok();
    save_signature($url, get_client_info(), $md5, $cvec);
    
    return TRUE;
}

$url = trim(@$_POST['url']);
if (empty($url)) {
    display_form();
    exit(0);
}
if (strlen($url) > MAX_URL_SIZE ||
    preg_match('£^http://([a-z0-9-]+[.])+[a-z]{2,}/.£i', $url) <= 0) {
    display_form();
    display_error('Invalid URL, must be http://...');
    exit(1);
}
display_loading();
$md5 = FALSE;
$cvec = FALSE;
if (record_url($url, $md5, $cvec) !== TRUE) {
    exit(1);
}
$urls = find_similar_pictures($md5, $cvec);
unset($cvec);
display_form();
display_similar_pictures($urls);

?>
</body>
</html>
