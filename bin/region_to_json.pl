#!/usr/bin/perl

use strict;
use warnings;

#use CGI::Carp qw(fatalsToBrowser);
use CGI;

use FindBin qw($Bin);
use lib "$Bin/../lib";

use Getopt::Long;
use JsonGenerator;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;

use IO::Handle;

use GlobalConfig;

my $using_CGI = 1;
my $region_filename;
if ($using_CGI) {

    my $cgi = CGI::new();
    $region_filename = $cgi->param('region_filename');
    if( not defined $region_filename ){
	$region_filename = "regions.illumina.c1.400.1M.5.txt";
    }
    print $cgi->header;
    print "<html><body><textarea>\n";

    #for debugging when doing CGI 
    open OUTPUT, '>', "$upload_dir/region_output.txt" or die $!;
    open ERROR,  '>', "$upload_dir/region_error.txt"  or die $!;

#    STDOUT->fdopen( \*OUTPUT, 'w' ) or die $!;
    STDERR->fdopen( \*ERROR,  'w' ) or die $!;

    open (OUTFILE, ">", "$upload_dir/$region_filename") or die "Couldn't open $region_filename for writing: $!";
    while(<$region_filename>){
        print OUTFILE $_;
    }
    close OUTFILE;
}
else{
    $region_filename = "/home/andrew/school/dnavis/jbrowse/data/svn/regions.illumina.c1.400.1M.5.txt";
}

my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $bamFile, $trackLabel, $key, $nclChunk, $compress);

my $defaultClass = "transcript";
$cssClass = $defaultClass;

my $reversed = reverse $region_filename;
my ($extension, $folders) = split(/\./, $reversed, 2);
$extension = reverse $extension;
$folders = reverse $folders;
print OUTPUT "extension: $extension\n";
print OUTPUT "string folders: $folders\n";
my @folders = split(/\//,$folders);
print OUTPUT "folders array: @folders\n";
$key = $folders[-1];
print OUTPUT "key: $key\n";
$trackLabel = $key;

if (!defined($nclChunk)) {
  # default chunk size is 50KiB

    $nclChunk = 50000;
    # $nclChunk is the uncompressed size, so we can make it bigger if
    # we're compressing
    $nclChunk *= 4 if $compress;
}

my $trackRel = "tracks";
my $trackDir = "$data_dir/$trackRel";
mkdir($data_dir) unless (-d $data_dir);
mkdir($trackDir) unless (-d $trackDir);
my @refSeqs = @{JsonGenerator::readJSON("$data_dir/refSeqs.js", [], 1)};


my @bamHeaders = ("start","end","strand");
my $startIndex = 0;
my $endIndex = 1;
my %style = ("class" => $cssClass,
             "key" => $key);

$style{clientConfig} = JSON::from_json($clientConfig) if (defined($clientConfig));



#if ($cssClass eq $defaultClass) {
#    $style{clientConfig}->{featureCss} = "background-color: #668; height: 8px;"
#    unless defined($style{clientConfig}->{featureCss});
#    $style{clientConfig}->{histCss} = "background-color: #88F"
#    unless defined($style{clientConfig}->{histCss});
#    $style{clientConfig}->{histScale} = 2
#    unless defined($style{clientConfig}->{histScale});
#}

#need to define a refSeq and trackLabel for this region

my ($jsonGen, $sorter);

open( region_FH, '<', "$upload_dir/$region_filename" ) or die $!;
#header variables
my $seqName;

#body variables
my (@splt, $start, $end);
my $line_num = 1;

while (<region_FH>) {
    print OUTPUT "$_\n";
    #process headers
    if ($line_num == 1){
	#rtrim
        $_ =~ s/\s+$//;
	print OUTPUT "trimmed |$_|\n";
	
        my @splt = split(/\s+/,$_);
	print OUTPUT "splt: ", @splt;
        $seqName = $splt[1];
        
        print OUTPUT "seqName : $seqName\n";
        foreach my $seqInfo (@refSeqs) {
            print OUTPUT "||" . $seqInfo->{name} . "||";
            if (lc $seqInfo->{name} eq lc $seqName){
                my $track_path = "$trackDir/".
                                 (lc $seqName).
                                   "/".$trackLabel;
                print OUTPUT $track_path;
                $jsonGen = JsonGenerator->new(
                           $track_path,
                           $nclChunk,
                           $compress,
                           $trackLabel,
                           $seqName,
                           $seqInfo->{start},
                           $seqInfo->{end}, 
                           \%style,
                           \@bamHeaders);

                $sorter = NCLSorter->new(
                            sub { $jsonGen->addFeature($_[0]) },
                            $startIndex, $endIndex
                          );
            }
        }
    }
    #process body
    else {
        @splt = split(/\s+/,$_);
        $start = int($splt[1]);
        $end = int($splt[2]);
        $sorter->addSorted( [$start,$end,1] );
    }
    
    $line_num++;
}

print OUTPUT "before flush\n";

$sorter->flush();
$jsonGen->generateTrack();

print OUTPUT "after gentrack\n";

my $ext = ($compress ? "jsonz" : "json");
my $new_entry_json = {
        'label' => $trackLabel,
        'key' => $key,
        'url' => "$trackRel/{refseq}/" . $trackLabel . "/trackData.$ext",
        'type' => "FeatureTrack",
    };

JsonGenerator::writeTrackEntry(
    "$data_dir/trackInfo.js",
    $new_entry_json
);

print 'trackInfo = [';
print JSON::to_json($new_entry_json, {pretty => 1});
print ']';
print "\n</textarea></body></html>";


if ($using_CGI) {
    close OUTPUT;
    close ERROR;
}

close region_FH;

