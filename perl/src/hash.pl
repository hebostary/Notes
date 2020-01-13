#!/usr/bin/perl
%data = ('google'=>'google.com', 'baidu'=>'baidu.com');
@keys = keys %data;
$size = @keys;
print "1.size: $size\n";
$data{'alibaba'} = 'alibaba.com';
@keys = keys %data;
$size = @keys;
print "2.size: $size\n";
delete $data{'baidu'};
@keys = keys %data;
$size = @keys;
print "3.size: $size\n";
