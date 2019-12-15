#!/usr/bin/perl -w
print "==========CASE1==========\n";
$str = "Hi, hunk is a good boy, hunk say it.";
$str =~ tr/a-z/A-Z/;
print "string after convert: $str.\n";
$str =~ tr/A-Z/A-Z/s;
print "string after convert: $str.\n";
$str =~ tr/ //d;
print "string after convert: $str.\n";
