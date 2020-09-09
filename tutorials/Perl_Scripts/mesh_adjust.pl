##########################################################################################
## 	Script : OGRE: Mesh File adjustement.
##  Auteur : David
##  Date   : 13/10/2004
##########################################################################################
##  The ANALYSIS script should be run once par object.
##  The ANALYSIS script analyse the mesh and skeleton files and generate a template adjustment file.
##  The template ADJUSTMENT file should be then modified manualy, with the parameters you want.
##
##  Then run this ADJUST script, every time you have a new version of your mesh object,
##  and the parameters you set in the ADJUSTMENT file will be applied automatiquely.
##
##  This ADJUST script will update your .MESH.XML file.
##  Afterwards, you have to run the ogre XMLconverter to convert it in a .MESH file.
##########################################################################################
#! /usr/local/bin/perl

my $mesh = @ARGV[0];
my $line;
my $readline;
my $newline;
my %assignment;
my %bone;
my $currentmat;

	if (scalar(@ARGV)==0)
	{
		print ("usage: mesh_adjust <objectname> \n");
		print ("   ex: perl mesh_adjust.pl door (extensions are added). ");
		exit (1);
	}

	print "Updating: $mesh \n";

	## READ THE CONFIGURATION FILE
	if(open(ADJFILE,"< $mesh.adjust.xml"))
	{
		# read the configuration (adjustment) file
		print ("File $mesh.adjust.xml opened. \n");
		print ("New assignments are: \n");
		while ($line = <ADJFILE>)
        {
			if ($line =~ /<bone\s/)
			{
		        # Get the name and ID of every bone
		        # Ex:  <bone id="0" name="zzwxy">
				my ($start, $id, $middle, $name, $end) = split ("\"", $line);
				$bone{$name}=$id;
			}
			if ($line =~ /<materialboneassignment/)
			{
		        # Get the new assigment between material and bones
		        # Ex: <materialboneassignment material= "Door/contour"          bonename= "zzwxy" >
				my ($start, $material, $middle, $bonename, $end) = split ("\"", $line);
				$assignment{$material}=$bone{$bonename};
				my $space = " " x (20-length($material));
				print (" $material $space => (id: ".$assignment{$material}.") $bonename \n");
			}
		}
		close (ADJFILE);
	}

	## PREPARE THE OUTPUT FILES

	# Find an available name for the backup file
	my $indice=0;
	my $backupfile = "$mesh.mesh.xml.bck_$indice";
    while (-e $backupfile)
	{
		$indice++;
		$backupfile = "$mesh.mesh.xml.bck_$indice";
 	}

	# Create a temporary new XML file
	my $newfile  = "$mesh.new.mesh.xml";
	my $meshfile = "$mesh.mesh.xml";


	## APPLY THE ADJUSTMENT ON THE XML FILE
	if(!open(MESHFILE,"< $meshfile"))
	{
		print ("$meshfile not found ! \n");
 	}
	{
		# Create a backup file
		open  (SVGFILE,"> $backupfile");
		# Apply the new assigments
		open  (NEWFILE,"> $newfile");
		print "File $meshfile opened. \n";
		while ($readline = <MESHFILE>)
        {
			if ($readline =~ /<submesh\s/)
			{
				# Detect the materials
				# Ex: <submesh material = "Door/frame" usesharedvertices="false" use32bitindexes="false">
				($head, $currentmat, $end) = split ("\"", $readline);
				print (" Updating $currentmat \n");
   			}
			if ($readline =~ /<vertexboneassignment\s/)
			{
				# Apply the assigned bone to the current material
				# And force the weight to 1
				# Ex: <vertexboneassignment vertexindex="25" boneindex="1" weight="0.986446" />
				$newline = $readline;
				$newline =~ s/boneindex\s*=\s*\"\d+\"/boneindex=\"$assignment{$currentmat}\"/;
 				$newline =~ s/weight\s*=\s*\".*\"/weight=\"1\"/;
			}
			else
			{
				$newline = $readline;
   			}
       		print (SVGFILE $readline);
        	print (NEWFILE $newline);
		}
		close (MESHFILE);
		close (NEWFILE);
		close (SVGFILE);
		print "Backup file created: $backupfile \n";
 	}

	# Replace the XML file with the new one
	rename ($newfile,$meshfile);
	print "Mesh file adjusted: $meshfile \n";
	exit(0);



