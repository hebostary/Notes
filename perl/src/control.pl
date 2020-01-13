#!/usr/bin/perl
$bVar = "true";
unless("$bVar" != "true") {
    print "bVar equal true\n";
} else {
    print "bVar not equal true\n";
}

$result = (10 > 20)?"true":"false";
print "$result\n";
