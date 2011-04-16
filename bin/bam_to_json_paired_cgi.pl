#!/usr/bin/perl

use strict;
use warnings;

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

use Cwd;

###CGI STUFF####

my $cgi = CGI::new();
my $bam_filename = $cgi->param('bam_filename');
if( not defined $bam_filename or $bam_filename eq '' ) {
    $bam_filename = "evidence.illumina.c1.400.1M.5.bam";
}

print $cgi->header;

open OUTPUT, '>', "bam_output.txt" or die $!;
open ERROR,  '>', "bam_error.txt"  or die $!;

#STDOUT->fdopen( \*OUTPUT, 'w' ) or die $!;
#STDERR->fdopen( \*ERROR,  'w' ) or die $!;

#open (OUTFILE, ">", "$bam_filename") or die "Couldn't open $bam_filename for writing: $!";
#while(<$bam_filename>){
#  print OUTFILE $_;
#}
#close OUTFILE;

### / CGI #####

print getcwd();


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

my $rootdir = "/var/www/html/jbrowse";
my $outdir = $rootdir . "/data";
my $bamFile = $rootdir . "/bin/$bam_filename"; #"/bin/" . $bam_filename;

if (!defined($nclChunk)) {
    # default chunk size is 50KiB
    $nclChunk = 50000;
    # $nclChunk is the uncompressed size, so we can make it bigger if
    # we're compressing
    $nclChunk *= 4 if $compress;
}

my $trackRel = "tracks";
my $trackDir = "$outdir/$trackRel";
mkdir($outdir) unless (-d $outdir);
mkdir($trackDir) unless (-d $trackDir);


my @refSeqs = @{JsonGenerator::readJSON("$outdir/refSeqs.js", [], 1)};

print "bamFile: $bamFile\n";
print "wer\n";
print "heoihwef\n";
print "oiuwerub\n";
print "238h9f9\n";

my $bam = Bio::DB::Bam->open($bamFile);
print $bam;
print "\n";
my $hdr = $bam->header();
print $hdr;
print "\n";
my $index = Bio::DB::Bam->index($bamFile, 1);
print "whhhhhattttttttttttt\n";
print "index created: $index\n";

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

#my @pairs = $sam->features(-type=>'read_pair');

foreach my $seqInfo (@refSeqs) {
    #hdr is the bam header
    my ($tid, $start, $end) = $hdr->parse_region($seqInfo->{name});
    print "tid: $tid, start: $start, end: $end\n";
    
    mkdir("$trackDir/" . $seqInfo->{name})
        unless (-d "$trackDir/" . $seqInfo->{name});

    if (defined($tid)) {
        print "trackDir: $trackDir\n, trackLabel: $trackLabel\n";
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

        foreach my $key (sort {$paired_info{$a}[0] <=> $paired_info{$b}[0]} keys %paired_info){
          $sorter->addSorted( a2a_helper( $paired_info{$key} ) );
        }
        
        $sorter->flush();
        $jsonGen->generateTrack();
    }
}

#add a new entry to trackInfo.js
#what if we want to group specific tracks to particular reference sequences

my $ext = ($compress ? "jsonz" : "json");
JsonGenerator::writeTrackEntry(
    "$outdir/trackInfo.js",
    {
        'label' => $trackLabel,
        'key' => $key,
        'url' => "$trackRel/{refseq}/" . $trackLabel . "/trackData.$ext",
        'type' => "FeatureTrack",
    }
);

#default
# sub align2array {
#   my $align = shift;
#   my $thing1 = ["dude","what the heck"];
#   my $thing2 = ["aabbaa","ccdd"];
#     return [$align->pos,
#             $align->calend + 1,
#             $align->reversed ? -1 : 1,
#            [$thing1,$thing2]];
# }

sub a2a_helper {
  my $arref = shift;
  #ix zero is the order index
  return [$arref->[1],$arref->[2],$arref->[3],
          [[$arref->[4],$arref->[5],$arref->[6],$arref->[7]],
           [$arref->[8],$arref->[9],$arref->[10],$arref->[11]]]];
}

sub a2a {
    #print "a2a called\n";
    my $ix = shift;
    my $align = shift;
    my $paired_info = shift;

    my $left = $align->pos;
    my $right = $align->calend+1;
    my $len = $align->data_len;
    my $mleft = $align->mate_start;
    my $mright = $align->mate_end;
    my $mlen = $align->mate_len;

    if( $left < $mleft ){
        my $reversed = $align->reversed ? -1 : 1;
        $paired_info->{$align->qname} = [$$ix,$left,42,1,$left,$right,$reversed,"left",42,42,42,"right"];
        $$ix = $$ix + 1;
    }
    else{
      my $reversed = $align->reversed ? -1 : 1;
      my $arref = $paired_info->{$align->qname};
      $arref->[2] = $right;
      $arref->[8] = $left;
      $arref->[9] = $right;
      $arref->[10] = $reversed;
    }
}
