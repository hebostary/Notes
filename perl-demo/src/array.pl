#!/usr/bin/perl -w
@sites = ('google', 'tengxun', 'taobao');
push(@sites, "baidu");
print "1.@sites\n";
$num = unshift(@sites, "sina");
print "2.@sites & num = $num\n";
$eleLast = pop(@sites);
print "3.@sites & eleLast = $eleLast\n";
$eleFirst = shift(@sites);
print "4.@sites & eleFirst = $eleFirst\n";
splice(@sites, 0, 3, 'alibaba', "veritas");
print "5.@sites\n";

$string = "baidu,veritas,alibaba";
@strArray = split(',', $string);
print "6.$strArray[0] $strArray[1]\n";
$site = "www-baidu-com";
@siteSub = split('-', $site);
print "7.$siteSub[0] $siteSub[1]\n";
print "8.@strArray\n";
@strArray = sort(@strArray);
print "9.@strArray\n";
