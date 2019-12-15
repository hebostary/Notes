#!/usr/bin/perl -w
$scalar    = 10;
@array     = (1, 2, 3);
%hash      = ('name'=>'hunk', 'age'=>25);
sub handler1{
    print "handler1 be called.\n";
}

sub handler2{
    print "handler2 be called.\n";
}

#get the reference of object
$scalarRef = \$scalar;
$arrayRef  = \@array;
$hashRef   = \%hash;
$subRef1   = \&handler1;
@subRef2   = (\&handler1, \&handler2);

#get the type and object of reference
print "=====CASE1=====\n";
print "scalar type: ", ref($scalarRef), " value:", $$scalarRef, "\n";
print "array type: ", ref($arrayRef), " value:", @$arrayRef, "\n";
print "array type: ", ref($hashRef), " value:", %$hashRef, "\n";
%hashObj1 = %$hashRef;
print "value of key name: ", $hashObj1{"name"}, "\n";
$hashObj1{"name"} = "jack";
print "value of key name: ", $hash{"name"}, "\n";
$$hashRef{"name"} = "jack";
print "value of key name: ", $hash{"name"}, "\n";

print "=====CASE2=====\n";
print "handler type: ", ref($subRef1), " value: ", &$subRef1, "\n";
print "---------------\n";
foreach $handler (@subRef2){
    &$handler();
}

