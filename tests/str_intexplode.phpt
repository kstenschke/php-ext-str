--TEST--
str_intexplode() function
--FILE--
<?php
var_dump(implode(',', str_intexplode('1')));
var_dump(implode(',', str_intexplode('1,2,3')));
var_dump(implode(',', str_intexplode('a')));
var_dump(implode(',', str_intexplode('a1')));
var_dump(implode(',', str_intexplode('1cow')));
var_dump(implode(',', str_intexplode('a1cow')));
var_dump(implode(',', str_intexplode('0cows1')));
var_dump(implode(',', str_intexplode('1,2,3,1')));
--EXPECT--
string(1) "1"
string(5) "1,2,3"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(7) "1,2,3,1"
