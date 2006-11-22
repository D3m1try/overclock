#!/usr/bin/perl

#freq table parser

$bit0=0;
$bit1=1;
$bit3=1;
while(<>)
{
	$sdram=0.0;
	$pci=0.0;
	$agp=0.0;
	($bit2, $bit7, $bit6, $bit5, $bit4, $fsb, $sdram, $pci, $agp) = split(" ",$_);


	#$bits =~ /(\d)(\d)(\d)(\d)/;
	#$bit2=$1;
	#$bit7=$2;
	#$bit6=$3;
	#$bit5=$4;
	#$bit4=$5;

	$byte=$bit0+($bit1<<1)+($bit2<<2)+($bit3<<3)+($bit4<<4)+($bit5<<5)+($bit6<<6)+($bit7<<7);
	printf("\t{%3.0f, 0x%.2X, %3.2f, %3.2f, %3.2f},\n", $fsb, $byte, $pci, $sdram, $agp);
}

