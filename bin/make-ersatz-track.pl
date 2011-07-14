#!/usr/bin/perl

use strict;
use warnings;

use FindBin qw($Bin);
use lib ("$Bin/../lib", "/usr/local/lib/perl5", "/usr/local/lib/perl5/site_perl");

use CGI qw(:standard);
use Getopt::Long;
use JsonGenerator qw/ readJSON /;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;
use IO::Handle;

use Cwd;
use GlobalConfig;



my $trackLabel = "ersatz1";
my $outdir = "/home/andrew/school/dnavis/jbrowse/data/tracks/chr1/$trackLabel";
my $nclChunk = 50000;
my $compress = 0;
my $seqInfo = {"name" , "chr1", 
               "start" , 0,
               "end" , 1000000};
my $cssClass = "transcript";
my $subfeatureClasses = {"forward","forward-strand",
                         "reverse","reverse-strand",
                         "hanging","feature3",
                         "SNP","SNP"};
my $key = $trackLabel;
my %style = ("class" => $cssClass,
             "subfeature_classes" => $subfeatureClasses,
             "key" => $key);
my @bamHeaders = ("start", "end", "strand","subfeatures");
my @subfeatureHeaders = ("start","end","strand","type");

my $startIndex = 0;
my $endIndex = 1;

my $pregen_histograms;
my $featureCount = 3;

my $jsonGen = JsonGenerator->new($outdir,
                                 $nclChunk,
                                 $compress, 
                                 $trackLabel,
                                 $seqInfo->{name},
                                 $seqInfo->{start},
                                 $seqInfo->{end},
                                 \%style, 
                                 \@bamHeaders, 
                                 \@subfeatureHeaders,
                                 $featureCount,
                                 $pregen_histograms);

my $sorter = NCLSorter->new(sub { $jsonGen->addFeature($_[0]) },
                            $startIndex, $endIndex);


#reading in an ersatz $paired_info for sandboxing with lazy loading of vertical data
my $paired_info = readJSON( "../data/ersatz_intervals_1.js", "not exist", 0, 0 );
print $paired_info;

my @sorted = sort {$paired_info->{$a}->[0] <=> $paired_info->{$b}->[0]} keys %$paired_info;
foreach my $key (@sorted){
    print "key: $key\n";
    $sorter->addSorted( $paired_info->{$key} );
}

$sorter->flush();
$jsonGen->generateTrack();

my $new_entry_json = {
                          'label' => $trackLabel,
                          'key' => $key,
                          'url' => "tracks/{refseq}/" . $trackLabel . "/trackData.json",
                          'type' => "FeatureTrack",
                     };
JsonGenerator::writeTrackEntry( "$data_dir/trackInfo.js", $new_entry_json );
