#!/usr/bin/perl

use strict;
use warnings;

use CGI qw(:standard);

use FindBin qw($Bin);
use lib "$Bin/../lib";

use Getopt::Long;
use JsonGenerator;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;

use IO::Handle;

use Cwd;
use GlobalConfig;

###CGI STUFF####

my $cgi = CGI::new();
my $bam_filename = $cgi->param('bam_filename');
#if( not defined $bam_filename or $bam_filename eq '' ) {
#    print "not definennenenene\n";
#    $bam_filename = "evidence.illumina.c1.400.1M.5.bam";
#}

print $cgi->header;
print "<html><body><textarea>\n";

open OUTPUT, '>', $upload_dir . "/" . "bam_output.txt" or die $!;
open ERROR,  '>', $upload_dir . "/" . "bam_error.txt"  or die $!;

print OUTPUT $cgi->header("application/json");

#STDOUT->fdopen( \*OUTPUT, 'w' ) or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;

print OUTPUT $upload_dir ."/". $bam_filename;
open (OUTFILE, ">", "$upload_dir/$bam_filename") or die "Couldn't open $bam_filename for writing: $!";
while(<$bam_filename>){
  print OUTFILE $_;
}
close OUTFILE;

### / CGI #####

print OUTPUT getcwd();


my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $trackLabel, $nclChunk, $compress, $key);

my $defaultClass = "transcript";
my $defaultSubfeatureClasses = {"left","readleft",
                                "right","readright",
                                "hanging","feature3"};

$cssClass = $defaultClass;
$subfeatureClasses = $defaultSubfeatureClasses;

my $reversed = reverse $bam_filename;
my ($extension, $folders) = split(/\./, $reversed, 2);
$extension = reverse $extension;
$folders = reverse $folders;
my @folders = split(/\//,$folders);
$key = $folders[-1];
$trackLabel = $key;

my $bamFile = $upload_dir . "/" . $bam_filename;

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

my $bam = Bio::DB::Bam->open($bamFile);
my $hdr = $bam->header();
my $index = Bio::DB::Bam->index($bamFile, 1);


my @bamHeaders = ("start", "end", "strand","subfeatures");
my @subfeatureHeaders = ("start","end","strand","type");
my $startIndex = 0;
my $endIndex = 1;

my %style = ("class" => $cssClass,
             "subfeature_classes" => $subfeatureClasses,
             "key" => $key);

$style{clientConfig} = JSON::from_json($clientConfig)
    if (defined($clientConfig));

#addig CSS stuff
#if ($cssClass eq $defaultClass) {
#    $style{clientConfig}->{featureCss} = "background-color: #668; height: 8px;"
#        unless defined($style{clientConfig}->{featureCss});
#    $style{clientConfig}->{histCss} = "background-color: #88F"
#        unless defined($style{clientConfig}->{histCss});
#    $style{clientConfig}->{histScale} = 2
#        unless defined($style{clientConfig}->{histScale});
#}

foreach my $seqInfo (@refSeqs) {
    #hdr is the bam header
    my ($tid, $start, $end) = $hdr->parse_region($seqInfo->{name});
    print OUTPUT "tid: $tid, start: $start, end: $end\n";
    
    mkdir("$trackDir/" . $seqInfo->{name})
        unless (-d "$trackDir/" . $seqInfo->{name});

    if (defined($tid)) {
        print OUTPUT "trackDir: $trackDir\n, trackLabel: $trackLabel\n";
        my $jsonGen = JsonGenerator->new("$trackDir/" . $seqInfo->{name}
                                         . "/" . $trackLabel,
                                         $nclChunk,
                                         $compress, $trackLabel,
                                         $seqInfo->{name},
                                         $seqInfo->{start},
                                         $seqInfo->{end},
                                         \%style, \@bamHeaders, \@subfeatureHeaders);

        my $sorter = NCLSorter->new(sub { $jsonGen->addFeature($_[0]) },
                                    $startIndex, $endIndex);

        #the default
        #$index->fetch($bam, $tid, $start, $end,
        #              sub { $sorter->addSorted(align2array($_[0])) });


        my %paired_info;
        my $ix = 0;
        $index->fetch($bam, $tid, $start, $end, sub { a2a( \$ix, $_[0], $_[1]) }, \%paired_info);

        print OUTPUT "starting sort\n";
        my @sorted = sort {$paired_info{$a}[0] <=> $paired_info{$b}[0]} keys %paired_info;
        print OUTPUT "done with sort\n";

        foreach my $key (@sorted){
          $sorter->addSorted( $paired_info{$key} );
        }
        
        $sorter->flush();
        $jsonGen->generateTrack();
    }
}



#add a new entry to trackInfo.js
#what if we want to group specific tracks to particular reference sequences

my $ext = ($compress ? "jsonz" : "json");
my $new_entry_json = {
        'label' => $trackLabel,
        'key' => $key,
        'url' => "$trackRel/{refseq}/" . $trackLabel . "/trackData.$ext",
        'type' => "FeatureTrack",
    };
JsonGenerator::writeTrackEntry( "$data_dir/trackInfo.js", $new_entry_json );

close OUTPUT;
close ERROR;

print 'trackInfo = [';
print JSON::to_json($new_entry_json, {pretty => 1});
print ']';
print "\n</textarea></body></html>";

sub a2a {
    my $ix = shift;
    my $align = shift;
    my $paired_info = shift;

    my $left = $align->pos;
    my $right = $align->calend+1;
    my $len = $align->data_len;
    my $mleft = $align->mate_start;
    
    if( $left < $mleft ){
        my $reversed = $align->reversed ? -1 : 1;
        $paired_info->{$align->qname} = [$left,42,1,[$left,$right,$reversed,"left"],[42,42,42,"right"]];
    }
    else{
      my $reversed = $align->reversed ? -1 : 1;
      my $arref = $paired_info->{$align->qname};
      $arref->[1] = $right;
      $arref->[4]->[0] = $left;
      $arref->[4]->[1] = $right;
      $arref->[4]->[2] = $reversed;
    }
}
