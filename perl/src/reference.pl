#!/usr/bin/perl -w
$var = 100;
$varRef = \$var;
$varRefRef = \$varRef;
print "value: ", $$varRef, "\n";
print "value: ", $varRef, "\n";
print "value: ", $$varRefRef, "\n";
