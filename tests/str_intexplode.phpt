--TEST--
str_intexplode() function
--FILE--
<?php
var_dump(count(str_intexplode('1')));
var_dump(count(str_intexplode('99')));
var_dump(implode(',', str_intexplode('1,2')));
--EXPECT--
int(1)
int(1)
string(3) "1,2"
