#!/usr/bin/perl -w
@result = readpipe("date") ;

print $?, " ", $$, " ", $^O, " ", $^T, " ",$^X, " ", @ARGV;
