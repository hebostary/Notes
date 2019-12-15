#!/usr/bin/perl -w
$email = "Hi, my email is hebostary@126.com.";
$email =~ /^[a-zA-Z0-9_\-\.]+@[a-zA-Z0-9_\-]+\.[a-zA-Z]+$/;
print "get email is: $&\n";

