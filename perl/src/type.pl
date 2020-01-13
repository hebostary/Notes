#!/usr/bin/perl 
@varArray = (1, 2, 3);
$varArray[50] = 51;
$size = @varArray;
$maxIndex = $#varArray;
print "$size $maxIndex\n";
