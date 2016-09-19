--TEST--
str_intexplode() function
--FILE--
<?php
var_dump(str_intexplode('1'))
var_dump(str_intexplode('99'))
var_dump(str_intexplode('1,2'));
var_dump(str_intexplode('1,2,3'));
var_dump(str_intexplode('a'));
var_dump(str_intexplode('a,b,c'));
var_dump(str_intexplode('a,b,c', true));
--EXPECT--
array(1) { [0]=> int(1) }
array(1) { [0]=> int(99) }
array(2) { [0]=> int(1) [1]=> int(2) }
array(2) { [0]=> int(1) [1]=> int(2) [2]=> int(3) }
array(1) { [0]=> int(0) }
array(3) { [0]=> int(0) [1]=> int(0) [2]=> int(0) }
array(1) { [0]=> int(0) }
