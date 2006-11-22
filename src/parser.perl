#!/usr/bin/perl

#freq table parser

$bit0=1;
$bit5=1;
$bit6=0;
$bit7=0;
while(<>)
{
	$sdram=0.0;
	$pci=0.0;
	$agp=0.0;
	($bits, $tmp1, $fsb, $tmp2, $pci) = split(" ",$_);

	$bits =~ /(\d)(\d)(\d)(\d)/;
	$bit4=$1;
	$bit3=$2;
	$bit2=$3;
	$bit1=$4;

	$byte=$bit0+($bit1<<1)+($bit2<<2)+($bit3<<3)+($bit4<<4)+($bit5<<5)+($bit6<<6)+($bit7<<7);
	printf("{%3.0f, 0x%.2X, %3.2f},\n", $fsb, $byte, $pci);
}

