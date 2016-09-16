--TEST--
str_random() function
--FILE--
<?php
var_dump(str_random(1));
var_dump(str_random(2));
var_dump(str_random(3));
--EXPECT--
string(1) "a"
string(2) "aa"
string(3) "aaa"
