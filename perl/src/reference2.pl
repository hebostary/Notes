#!/usr/bin/perl -w
$arrayRef1 = [11, 22, 33];
$arrayRef2 = [
	[1, 2, 3],
	[4, 5, 6],
];
$hashRef = {'name'=>'hunk', 'age'=>25};
$subRef  = sub {print "sub be called.\n"};

print $$arrayRef1[2], "\n";
print @$arrayRef2, "\n";
print @$arrayRef2[1], "\n";
print $$arrayRef2[1][1], "\n";
print $$hashRef{'name'}, "\n";
&$subRef;
