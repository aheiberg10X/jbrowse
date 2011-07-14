#!/usr/bin/perl

use strict;
use warnings;

use CGI qw(:standard);

use FindBin qw($Bin);
use lib ("$Bin/../lib", "/usr/local/lib/perl5", "/usr/local/lib/perl5/site_perl");

use Getopt::Long;
use JsonGenerator;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;

use IO::Handle;

use Cwd;
use GlobalConfig;

open(OUTPUT,">","uploads/derp.txt");


my $cgi = CGI::new();
print $cgi->header;

my $bamFile = "/home/andrew/school/dnavis/jbrowse/uploads/evidence.or.missFR.5.bam"; 
print OUTPUT "opening upload file\n";
my $bam = Bio::DB::Bam->open($bamFile);
my $hdr = $bam->header();
print OUTPUT "after header\n";
my $index = Bio::DB::Bam->index($bamFile, 1);
print OUTPUT "can't make the fucking index?\n";


print "<html><body><textarea>";
print "['hello']";
print "</textarea></body></html>";

close(OUTPUT);
