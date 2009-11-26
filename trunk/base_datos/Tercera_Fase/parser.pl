#!/usr/bin/perl
use CGI qw/:standard/;

open (FILE, "<planilla-familias.txt") || die "Can't open '$txt': $!\n";#overwrite the old file in the new format
@emails = <FILE>;
close(FILE);

foreach (@emails) {
	chomp;
	$tabla = "persona"
	$campos ="(NroPlanilla, CI, Nombre, APellido)";
	$line=$_; 
	($p,$ci,$ape,$nom,@r) = split (/\,/, $line);
	print "INSERT INTO $tabla $campos VALUES ($p,
	$ci, \"$nom\", \"$ape\");\n";
}
