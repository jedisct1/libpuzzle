--TEST--
Check the puzzle_set(3) interface
--SKIPIF--
<?php if (!extension_loaded("libpuzzle")) print "skip"; ?>
--FILE--
<?php

$cvec1 = puzzle_fill_cvec_from_file(dirname(__FILE__) . '/pics/pic-a-0.jpg');
$cvec2 = puzzle_fill_cvec_from_file(dirname(__FILE__) . '/pics/pic-a-1.jpg');
puzzle_set_max_width(1500);
puzzle_set_max_height(1500);
puzzle_set_lambdas(11);
puzzle_set_noise_cutoff(1.0);
puzzle_set_p_ratio(2.0);
puzzle_set_contrast_barrier_for_cropping(0.1);
puzzle_set_max_cropping_ratio(0.1);
puzzle_set_autocrop(FALSE);

$d = puzzle_vector_normalized_distance($cvec1, $cvec2);
exit((int) ($d < PUZZLE_CVEC_SIMILARITY_LOWER_THRESHOLD));

?>
--EXPECT--
1
