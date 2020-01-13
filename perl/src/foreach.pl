#!/usr/bin/perl -w
@array = ('google', 'baidu', 'alibaba');
foreach(@array){
    print "$_\t";
}
print "\n";

foreach $e (@array){
    print "$e\t";
}
print "\n";

foreach(a..d, A..D){
    print "$_\t";
}
print "\n";
