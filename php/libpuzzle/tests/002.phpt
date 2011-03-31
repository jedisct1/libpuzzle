--TEST--
Check for distance between similar images
--SKIPIF--
<?php if (!extension_loaded("libpuzzle")) print "skip"; ?>
--FILE--
<?php

$cvec1 = puzzle_fill_cvec_from_file(dirname(__FILE__) . '/pics/pic-a-0.jpg');
$cvec2 = puzzle_fill_cvec_from_file(dirname(__FILE__) . '/pics/pic-a-1.jpg');
$d = puzzle_vector_normalized_distance($cvec1, $cvec2);
exit((int) ($d < PUZZLE_CVEC_SIMILARITY_LOWER_THRESHOLD));

?>
--EXPECT--
1
