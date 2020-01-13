#!/usr/bin/perl

#print "hello hunk"
=pod
Just
Print hello
=cut
print "hello hunk!\n";
print 'hello hunk!\n';
print "\n";

#heredoc
$a = 100;
$str = <<"EOF";
hello world
print a : a = $a
EOF
print "print str = $str\n";
