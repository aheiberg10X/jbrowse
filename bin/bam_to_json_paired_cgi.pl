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


###CGI STUFF####

my $cgi = CGI::new();
my $bam_filename = $cgi->param('bam_filename');
my $bam_linking = defined $cgi->param('display_linking');
my $bam_histogram_filename = $cgi->param("bam_histogram_filename");
#if( not defined $bam_filename or $bam_filename eq '' ) {
#    print "not definennenenene\n";
#    $bam_filename = "evidence.illumina.c1.400.1M.5.bam";
#}

print $cgi->header;
#place a json response inside here"
print "<html><body><textarea>\n";

open( my $OUTPUT, '>', $upload_dir . "/" . "bam_output.txt" ) or die $!;
open ERROR,  '>', $upload_dir . "/" . "bam_error.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;

if( $DEBUG ) {
    print $OUTPUT "we getting the auto-histogram file?: $bam_histogram_filename\n";
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
    my $json_text = <$bam_histogram_filename>;
    
    #delete whitespace
    $json_text =~ s/\s+/ /g;
    #delete what's leading up to the start of the object (i.e ' histogram = ')
    $json_text =~ s/^.+= //;
    
    close OUTFILE;
    $pregen_histograms = JSON::decode_json($json_text);
}

###UPLOADING###
###############


my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $trackLabel, $nclChunk, $compress, $key, $featureCount);

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

my @bookmarks;
my ($cur_left, $cur_right) = (0,0);
foreach my $seqInfo (@refSeqs) {
    #hdr is the bam header
    my ($tid, $start, $end) = $hdr->parse_region($seqInfo->{name});
    print $OUTPUT "tid: $tid, start: $start, end: $end\n";
    
    mkdir("$trackDir/" . $seqInfo->{name})
        unless (-d "$trackDir/" . $seqInfo->{name});

    if (defined($tid)) {
        print $OUTPUT "trackDir: $trackDir\n, trackLabel: $trackLabel\n";
        my $outdir = "$trackDir/" . $seqInfo->{name} . "/" . $trackLabel;
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

        #the default
        #$index->fetch($bam, $tid, $start, $end,
        #              sub { $sorter->addSorted(align2array($_[0])) });

        if( $bam_linking ) {
            print $OUTPUT "linkingi\n";
            my %paired_info;
            # $_[0] is the alignment found by fetch
            # $_[1] is \%paired_info reference
            $index->fetch($bam, $tid, $start, $end, sub { linking_align2array( $_[0], $_[1]) }, \%paired_info);

            my @sorted = sort {$paired_info{$a}[0] <=> $paired_info{$b}[0]} keys %paired_info;
            foreach my $key (@sorted){
                updateBookmarks( $jsonGen, \$cur_left, \$cur_right, $paired_info{$key} );
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

            foreach my $alignment (sort {$a->[0] <=> $b->[0]} @tosort){
                updateBookmarks( $jsonGen, \$cur_left, \$cur_right, $alignment );
                $sorter->addSorted( $alignment );
            }
        }

        $sorter->flush();
        #catching error, i.e not finding alignments in BAM file and consequently dividing by 0
        eval {
            $jsonGen->generateTrack();
            1;
        } or do {
            print $OUTPUT $@;
            $bad_bam = 1;
        }
    }
}

########################################
########## bookmarks
#print $OUTPUT "bookmarks: @bookmarks\n";
#open(F,">","$outdir/bookmarks.js");
#print F "bookmarks = ";
#print F JSON::encode_json \@bookmarks;
#close(F);
########### bookmarks
#######################################3

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

sub linking_align2array {
    my $align = shift;
    my $paired_info = shift;

    my $left = $align->pos+1;
    my $right = $align->calend+1;

    my $strand = ($align->flag & 0x10) >> 4; #$align->strand; #$align->strand ? -1 : 1;
    my $this_style = $strand ? "reverse" : "forward";
    $strand = $strand ? -1 : 1;

    my $qname = $align->qname;

    #remember the -$hanging_fix on the main $right are so it doesnt poke out from the subfeature
    my $hanging_fix = 20;
    if( ! defined $paired_info->{$qname} ){
        $paired_info->{$qname} = [$left, $right, $strand, $this_style ];
    }
    else {
        my $mates_info = $paired_info->{$qname};
        
        if( $mates_info->[0] < $left ){
            $paired_info->{$qname} = [$mates_info->[0],$right-$hanging_fix,0,[$mates_info,[$left,$right,$strand,$this_style]]];
        }
        else{
            $paired_info->{$qname} = [$left,$mates_info->[1]-$hanging_fix,0,[[$left,$right,$strand,$this_style],$mates_info]];
        }
        #sanity check for overlap?
    }
}

sub updateBookmarks {
    my $jsonGen = shift;
    my $cur_left = shift;
    my $cur_right = shift;
    my $align_array = shift;

    my ($left,$right) = ($align_array->[0], $align_array->[1]);
    if( $$cur_left <= $left and $left <= $$cur_right ){
        if( $right > $$cur_right ){ 
            $$cur_right = $right;
        }
    }
    else {
        if( $left - $$cur_right > $BOOKMARK_THRESH ){
            $jsonGen->addInterestingArea( $left );
            #push( @$bookmarks, $left );
        }
        ($$cur_left, $$cur_right) = ($left,$right);
    }    
}
