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

######################################################
#################### CGI STUFF #######################

my $cgi = CGI::new();
my $bam_filename = $cgi->param('bam_filename');                       #input type=file
my $bam_linking = defined $cgi->param('display_linking');             
my $bam_histogram_filename = $cgi->param("bam_histogram_filename");   #input type=file
#my $refseqName = $cgi->param('refseqName');  #sequence name the reads in $bam_filename pertain to

### DEBUGGING OUTPUT ###
open( my $OUTPUT, '>', $upload_dir . "/" . "upload_output.txt" ) or die $!;
open ERROR,  '>', $upload_dir . "/" . "upload_error.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;
### DEBUGGING OUTPUT ###

if( $DEBUG ) {
    print $OUTPUT "we getting the auto-histogram file?: $bam_histogram_filename\n";
    print $OUTPUT $upload_dir ."/". $bam_filename;
}

print $OUTPUT "printing headres\n";
print $cgi->header;
print "<html><body><textarea>\n";  #place a json response inside here"

#########################################################################
#######################  UPLOADING  #####################################

#write the data to $upload_dir, TODO: is it necessary to write to a file, can't we just save in a variable a la $pregen_histograms below?
#print $OUTPUT "writing upload file\n";
my $bamFile =  "$upload_dir/$bam_filename";
open (OUTFILE, ">", $bamFile) or die "Couldn't open $bam_filename for writing: $!";
while(<$bam_filename>){
  print OUTFILE $_;
}
close OUTFILE;
print $OUTPUT "done writing upload file\n";
print "{'status':'OK'}";
print "</textarea></body></html>";
