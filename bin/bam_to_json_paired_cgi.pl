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
my $bam_linking = defined $cgi->param('bam_linking');
my $bam_histogram_filename = $cgi->param("bam_histogram_filename");
#if( not defined $bam_filename or $bam_filename eq '' ) {
#    print "not definennenenene\n";
#    $bam_filename = "evidence.illumina.c1.400.1M.5.bam";
#}

print $cgi->header;
#place a json response inside here"
print "<html><body><textarea>\n";

#moved to GlobalConfig
open( my $OUTPUT, '>', $upload_dir . "/" . "bam_output.txt" ) or die $!;
open ERROR,  '>', $upload_dir . "/" . "bam_error.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;

if( $DEBUG ) {
    print $OUTPUT $upload_dir ."/". $bam_filename;
}

###############
###UPLOADING###

open (OUTFILE, ">", "$upload_dir/$bam_filename") or die "Couldn't open $bam_filename for writing: $!";
while(<$bam_filename>){
  print OUTFILE $_;
}
close OUTFILE;

my $pregen_histograms;
if( defined $bam_histogram_filename and $bam_histogram_filename ne ''){
    local $/=undef;
    open( OUTFILE, '<', "$bam_histogram_filename");
    my $json_text = <OUTFILE>;
    close OUTFILE;
    $pregen_histograms = JSON::decode_json($json_text);
    print $OUTPUT "json_text: $json_text\n";
}

#if( $DEBUG ){
#    print $OUTPUT $pregen_histograms->[0]->{"basesPerBin"};
#}


###UPLOADING###
###############


my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $trackLabel, $nclChunk, $compress, $key);

my $defaultClass = "transcript";
my $defaultSubfeatureClasses = {"forward","forward-strand",
                                "reverse","reverse-strand",
                                "hanging","feature3",
                                "SNP","SNP"};

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
#catch divide by 0 errors
my $bad_bam = 0;


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
    print $OUTPUT "tid: $tid, start: $start, end: $end\n";
    
    mkdir("$trackDir/" . $seqInfo->{name})
        unless (-d "$trackDir/" . $seqInfo->{name});

    if (defined($tid)) {
        print $OUTPUT "trackDir: $trackDir\n, trackLabel: $trackLabel\n";
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

        if( $bam_linking ) {
            print $OUTPUT "linking\n";
            my %paired_info;
            $index->fetch($bam, $tid, $start, $end, sub { a2a( $_[0], $_[1]) }, \%paired_info);

            my @sorted = sort {$paired_info{$a}[0] <=> $paired_info{$b}[0]} keys %paired_info;
            foreach my $key (@sorted){
                $sorter->addSorted( $paired_info{$key} );
            }
        }
        else{
            print $OUTPUT "not linking\n";
            my @tosort;
            $index->fetch( $bam,
                           $tid,
                           $start,
                           $end,
                           sub{ align2array($_[0],$_[1])}, \@tosort);

            foreach my $read (sort {$a->[0] <=> $b->[0]} @tosort){
                $sorter->addSorted( $read );
            }
        }

        $sorter->flush();
        #catching error, i.e not finding alignments in BAM file and consequently dividing by 0
        eval {
            $jsonGen->generateTrack();
            1;
        } or do {
            $bad_bam = 1;
        }
    }
}

if( $bad_bam ){
    print $OUTPUT "bad bam\n";
    print '{"status":"ERROR", "message":"This BAM file can\'t be read by Bio::DB::Bam (reporting that there are 0 alignments)"}';
}
else{
    print $OUTPUT "good bam\n";
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


    print '{"status":"OK", "trackData":[';
    print JSON::to_json($new_entry_json, {pretty => 1});
    print ']}';
}

close $OUTPUT;
close ERROR;

print "\n</textarea></body></html>";

sub align2array {
    my $align = shift;
    my $tosort = shift;

    my $left = $align->pos+1;
    my $right = $align->calend+1;
    my $strand = ($align->flag & 0x10) >> 4;

    #if strand is one according to flag, that means it is reversed.  To jbrowse, reverse/minus is -1, forward/plus is 1.
    $strand = $strand ? -1 : 1;
    #my $qname = $align->qname;

    push(@$tosort, [$left,$right,$strand]); #[[$left,$right,$strand,$strand ? "reverse" : "forward"]]]);
}

sub a2a {
    my $align = shift;
    my $paired_info = shift;

    my $left = $align->pos+1;
    my $right = $align->calend+1;
    my $strand = ($align->flag & 0x10) >> 4; #$align->strand; #$align->strand ? -1 : 1;
    $strand = $strand ? -1 : 1;
    my $qname = $align->qname;

    #remember the -$hanging_fix on the main $right are so it doesnt poke out from the subfeature
    my $hanging_fix = 20;
    if( ! defined $paired_info->{$qname} ){
        $paired_info->{$qname} = [$left,$right-$hanging_fix,0,[$left,$right,$strand, $strand ? "reverse" : "forward" ]];
    }
    else {
        my $mates_info = $paired_info->{$qname};
        my $this_style;

        $this_style = $strand ? "reverse" : "forward";
        $mates_info->[3] = $mates_info->[2] ? "reverse" : "forward";

        if( $mates_info->[0] < $left ){
            $paired_info->{$qname} = [$mates_info->[0],$right-$hanging_fix,0,[$mates_info,[$left,$right,$strand,$this_style]]];
        }
        else{
            $paired_info->{$qname} = [$left,$mates_info->[1]-$hanging_fix,0,[[$left,$right,$strand,$this_style],$mates_info]];
        }
        #sanity check for overlap?
    }
}
