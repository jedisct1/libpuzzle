--TEST--
Check for libpuzzle presence
--SKIPIF--
<?php if (!extension_loaded("libpuzzle")) print "skip"; ?>
--FILE--
<?php
echo "libpuzzle extension is available";
?>
--EXPECT--
libpuzzle extension is available
