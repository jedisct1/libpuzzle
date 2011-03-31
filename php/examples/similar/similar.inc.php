<?php

function split_into_words($sig) {
    $words = array();
    $u = 0;    
    do {
	$words[$u] = substr($sig, $u, MAX_WORD_LENGTH);
    } while (++$u < MAX_WORDS);

    return $words;
}

function save_signature($url, $client_info, $md5, $cvec) {
    $compressed_cvec = puzzle_compress_cvec($cvec);
    $words = split_into_words($cvec);
    $dbh = new PDO(DB_DSN);
    $dbh->beginTransaction();
    try {
	$st = $dbh->prepare
	  ('DELETE FROM sentpictures WHERE url = :url');
	$st->execute(array(':url' => $url));
	$st = $dbh->prepare
	  ('SELECT id FROM pictures WHERE digest = :digest');
	$st->execute(array(':digest' => $md5));
	$picture_id = $st->fetchColumn();
	$st->closeCursor();
	$duplicate = TRUE;
	if ($picture_id === FALSE) {
	    $duplicate = FALSE;
	    $st = $dbh->prepare
	      ('INSERT INTO pictures (digest) VALUES (:digest)');
	    $st->execute(array(':digest' => $md5));
	    $picture_id = $dbh->lastInsertId('id');
	}	
	$st = $dbh->prepare
	  ('INSERT INTO sentpictures (url, sender, picture_id) ' .
	   'VALUES (:url, :sender, :picture_id)');
	$st->execute(array(':url' => $url, ':sender' => $client_info,
			   ':picture_id' => $picture_id));
	if ($duplicate === TRUE) {
	    $dbh->commit();
	    return TRUE;
	}
	$st = $dbh->prepare
	  ('INSERT INTO signatures (compressed_signature, picture_id) ' .
	   'VALUES(:compressed_signature, :picture_id)');
	$st->execute(array(':compressed_signature' => $compressed_cvec,
			   ':picture_id' => $picture_id));
	$signature_id = $dbh->lastInsertId('id');
	$st = $dbh->prepare
	  ('INSERT INTO words (pos_and_word, signature_id) ' .
	   'VALUES (:pos_and_word, :signature_id)');
	foreach ($words as $u => $word) {
	    $st->execute(array('pos_and_word'
			       => chr($u) . puzzle_compress_cvec($word),
			       'signature_id' => $signature_id));
	}
	$dbh->commit();
    } catch (Exception $e) {
	var_dump($e);
	$dbh->rollback();
    }    
    return TRUE;
}

function find_similar_pictures($md5, $cvec,
			       $threshold = PUZZLE_CVEC_SIMILARITY_THRESHOLD) {
    $compressed_cvec = puzzle_compress_cvec($cvec);
    $words = split_into_words($cvec);    
    $dbh = new PDO(DB_DSN);
    $dbh->beginTransaction();
    $sql = 'SELECT DISTINCT(signature_id) AS signature_id FROM words ' .
      'WHERE pos_and_word IN (';
    $coma = FALSE;
    foreach ($words as $u => $word) {
	if ($coma === TRUE) {
	    $sql .= ',';
	}
	$sql .= $dbh->quote(chr($u) . puzzle_compress_cvec($word));
	$coma = TRUE;
    }
    $sql .= ')';
    $res_words = $dbh->query($sql);
    $scores = array();    
    $st = $dbh->prepare('SELECT compressed_signature, picture_id ' .
			'FROM signatures WHERE id = :id');
    while (($signature_id = $res_words->fetchColumn()) !== FALSE) {
	$st->execute(array(':id' => $signature_id));
	$row = $st->fetch();
	$found_compressed_signature = $row['compressed_signature'];
	$picture_id = $row['picture_id'];
	$found_cvec = puzzle_uncompress_cvec($found_compressed_signature);
	$distance = puzzle_vector_normalized_distance($cvec, $found_cvec);
	if ($distance < $threshold && $distance > 0.0) {
	    $scores[$picture_id] = $distance;
	}
    }
    $sql = 'SELECT url FROM sentpictures WHERE picture_id IN (';
    $coma = FALSE;
    foreach ($scores as $picture_id => $score) {
	if ($coma === TRUE) {
	    $sql .= ',';
	}
	$sql .= $dbh->quote($picture_id);
	$coma = TRUE;
    }
    $sql .= ')';
    $urls = array();    
    if (!empty($scores)) {
	$res_urls = $dbh->query($sql);
	while (($url = $res_urls->fetchColumn()) !== FALSE) {
	    array_push($urls, $url);
	}
    }
    $dbh->commit();    
        
    return $urls;
}

?>
