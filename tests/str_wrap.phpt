--TEST--
str_wrap() function
--FILE--
<?php
var_dump(str_wrap("", "", ""));
var_dump(str_wrap("a", "", ""));
var_dump(str_wrap("", "", "c"));
var_dump(str_wrap("", "b", ""));
var_dump(str_wrap("a", "b", ""));
var_dump(str_wrap("", "b", "c"));
var_dump(str_wrap("a", "b", "c"));
--EXPECT--
string(0) ""
string(1) "a"
string(1) "c"
string(1) "b"
string(2) "ab"
string(2) "bc"
string(3) "abc"
