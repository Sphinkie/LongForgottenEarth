##########################################################################################
## 	Script : OGRE: Mesh File analysis.
##  Auteur : David
##  Date   : 13/10/2004
##########################################################################################
##  This ANALYSIS script should be run once par object.
##  This ANALYSIS script analyse the mesh and skeleton files and generate a template adjustment file.
##  The template ADJUSTMENT file should be then edited manualy, with the parameters you want.
##
##  Then run the ADJUST script, every time you have a new version of you mesh object,
##  and the parameters you set in the ADJUSTMENT file will be applied automatiquely.
##########################################################################################
#! /usr/local/bin/perl

my $mesh = @ARGV[0];
my $line;

	if (scalar(@ARGV)==0)
	{
		print ("usage: mesh_analyse <objectname> \n");
		print ("   ex: perl mesh_analyse.pl door (extensions are added). ");
		exit (1);
	}

	print "Analyzing $mesh. \n";
	open  (OUTFILE,"> $mesh.adjust.xml");

	print (OUTFILE "<adjustment> \n");
	if(open(FICHIER,"< $mesh.skeleton.xml"))
	{
		print "File $mesh.skeleton.xml opened. \n";
		print (OUTFILE "   <bonelist> \n");
		while ($line = <FICHIER>)
        {
        	# Build the bone list
			# Ex: <bone id="0" name="zzwxy">
			if ($line =~ /<bone\s/)
			{
				chop ($line);
				# Find the default bone (ID 0)
				print (OUTFILE "  $line \n");
				my ($head, $id, $middle, $name, $end) = split ("\"", $line);
				if ($id==0) {$firstbone=$name;}
				print " New bone found: $name \n";
			}
		}
		print (OUTFILE "   </bonelist> \n");
		close (FICHIER);
	}

	if(open(FICHIER,"< $mesh.mesh.xml"))
	{
		print "File $mesh.mesh.xml opened. \n";
		print (OUTFILE "   <materiallist> \n");
		print (OUTFILE "   <!-- WRITE HERE THE BONENAME YOU WANT TO BE ASSIGN TO EACH MATERIAL --> \n");
		while ($line = <FICHIER>)
        {
			if ($line =~ /<submesh\s/)
			{
				# look for material = (\s=space)
				# and keep only what is between " "
				$line =~ s/material\s*=\s*\"(.+?)\"/$1/;
				my $newmat = $1;
				my $space = " " x (20-length($newmat));
				print " New material found: $newmat \n";
				print (OUTFILE "       <materialboneassignment material= \"".$newmat."\" $space bonename= \"$firstbone\" > \n");
			}
		}
		print (OUTFILE "   </materiallist> \n");
		close (FICHIER);
 	}

	print (OUTFILE "</adjustment> \n");
	close (OUTFILE);
	print "File $mesh.adjust.xml created.\n";
	exit(0);



