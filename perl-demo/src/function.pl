#!/usr/bin/perl -w

#CASE1:compute the average oh the all params
print "\n=====CASE1=====\n";
sub getAverage{
    my $paramNum = scalar(@_);
    print "the number of params: $paramNum\n";
    $paramSum = 0;

    foreach $param (@_){
        $paramSum += $param;
    }

    $average = $paramSum / $paramNum;
    return $average;
}

@array = (10, 20 ,30, 40);
$ave = getAverage(@array);
print "average value of array(@array): $ave\n";

#CASE2:delivery list params
print "\n=====CASE2=====\n";
sub printParams{
    my @list = @_;
    print "print all params: @list\n";
}

$aInt = 10;
@bArray = (20,30,40);
printParams($aInt, @bArray);

#CASE3: delivery hash param
print "\n=====CASE3=====\n";
sub printHash{
    my (%hash) = @_;
    foreach my $key (keys %hash){
        my $value = $hash{$key};
        print "$key : $value\n";
    }
}

%hash = ('name' => 'hunk', 'age' => 25);
printHash(%hash);

#CASE4: test local and global var
print "\n=====CASE4=====\n";
$helloString = "hello wolrld";
sub printHello{
    my $helloString = "hello hunk";
    print "local: $helloString\n";
    printHelloSub();
}

sub printHelloSub{
    print "sub: $helloString\n";
}
print "global: $helloString\n";
printHello();
print "global: $helloString\n";


#CASE5: state
print "\n=====CASE5=====\n";
use feature 'state';

sub printCount{
    $count = 0;
    print "count: $count\n";
    $count++;
}

for(1..3){
    printCount();
}
