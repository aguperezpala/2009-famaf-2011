#!/usr/bin/perl
use CGI qw/:standard/;


@files= "<planilla-familias.txt\n
		 <planilla-jefe_familia.txt\n"
for each (@files){
	open (FILE, "<planilla-familias.txt")
	
	shift @file_lines;
}
open (FILE, "<planilla-familias.txt") || die "Can't open '$txt': $!\n";#overwrite the old file in the new format
@emails = <FILE>;
close(FILE);

foreach (@emails) {
	chomp;
	$campos ="(NroPlanilla, CI, Nombre, APellido)";
	$line=$_; 
	($p,$ci,$ape,$nom,@r) = split (/\,/, $line);
	print "INSERT INTO persona $campos VALUES ($p,
	$ci, \"$nom\", \"$ape\");\n";
}
