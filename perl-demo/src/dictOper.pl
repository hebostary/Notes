#!/usr/bin/perl -w
print "==========CASE1=========\n";
$dir = "/home/*";
my @files = glob($dir);
foreach (@files){
    print $_."\n";
}

print "==========CASE2=========\n";
opendir(DIR, '.') or die "open current dir failed, $!";
while($file = readdir(DIR)){
    print "$file\n";
}
closedir DIR;

print "==========CASE3=========\n";
$dir = "/home/test";
rmdir($dir);
mkdir($dir) or die "create dir failed,$!";
print "create dir success.\n";
rmdir($dir) or die "delete dir failed, $!";
print "delete dir success.\n";
$dir = "/home";
chdir($dir) or die "change dir failed, $!";
print "change dir success.\n";
$currPath = readpipe("pwd");
print "current work path: $currPath.\n";
