#!/usr/bin/perl -w
print "==========CASE1==========\n";
$str = "Hi, hunk is a good boy, hunk say it.";
if($str =~ /hunk/){
    print "First, match hunk success.\n";
    print "First, pre string: ",$`,"\n";
    print "First, match string: ",$&,"\n";
    print "First, post string: ",$',"\n";
} else {
    print "First, match hunk failed.\n";
}

if($str =~ /HUNK/){
    print "Second, match HUNK success.\n";
} else {
    print "Second, match HUNK failed.\n";
}

if($str =~ /HUNK/i){
    print "Third, match HUNK success.\n";
} else {
    print "Third, match HUNK failed.\n";
}
