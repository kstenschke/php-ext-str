--TEST--
str_startsnumerical() function
--FILE--
<?php
var_dump(str_startsnumerical("hello"));
var_dump(str_startsnumerical("hello", false));
var_dump(str_startsnumerical("hello", true));
var_dump(str_startsnumerical("0hello"));
var_dump(str_startsnumerical("1hello"));
var_dump(str_startsnumerical("-1hello"));
var_dump(str_startsnumerical("-1hello", true));
var_dump(str_startsnumerical("-hello", true));
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
