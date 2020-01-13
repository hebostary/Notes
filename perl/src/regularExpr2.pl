#!/usr/bin/perl -w
print "==========CASE1==========\n";
$str = "Hi, hunk is a good boy, hunk say it.";
$str =~ s/hunk/jack/;
print "string after replace: $str.\n";

print "==========CASE2==========\n";
$str = "Hi, hunk is a good boy, hunk say it.";
$str =~ s/hunk/jack/g;
print "string after replace: $str.\n";
