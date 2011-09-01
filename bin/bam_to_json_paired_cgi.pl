#!/usr/bin/perl

use strict;
use warnings;

use CGI qw(:standard);

use FindBin qw($Bin);
use lib ("$Bin/../lib", "/usr/local/lib/perl5", "/usr/local/lib/perl5/site_perl","/home/aheiberg/perl5/lib/perl5/");

use Getopt::Long;
use JsonGenerator;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;
use File::Basename;
use IO::Handle;
use PairStreamer;
use Cwd;
use GlobalConfig;
use Data::Dumper;
#### DEBUGGING OUTPUT ###
open( my $OUTPUT, '>', $DEBUG_DIR . "/" . "bam_output.txt" ) or die $!;
open ERROR,  '>', $DEBUG_DIR . "/" . "bam_error.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;
#### DEBUGGING OUTPUT ###

my $stream = 0;

my $profiling = 0;
if( $profiling ){
    my $path = "/home/andrew/school/dnavis/jbrowse/genomequery/biosql_compiler/biosql/dst/chr1"; #"/home/andrew/school/dnavis/jbrowse/profiling";
    my $option = "big";
    my $bam_file = "$path/out.evidence.bam.short"; #"$path/"$path/profile_$option.bam";

#"/home/andrew/school/dnavis/svn/jbrowse/Apr28/hack/evidence.or.missFR.5.bam";
#"/home/andrew/school/dnavis/svn/jbrowse/dinh_seg_dups/bam_files/hack/reg_chr18:96741-100669.bam";

#"/home/andrew/school/dnavis/svn/jbrowse/dinh_seg_dups/bam_files/hack/reg_chr18:96741-100669.bam";
    my $host_chrom = "chr1";
    my $linking = "linking";
    my $histogram_filename = "$path/out.evidence.hist";

    createTrack( $host_chrom, "NA18507", "test", "linking" );
    exit;
}
######################################################
#################### CGI STUFF #######################


my $cgi = CGI::new();
print $cgi->header;
print "<html><body><textarea>\n";  #place a json response inside here"
my $donor = $ARGV[0];
my $query_name = $ARGV[1];
my $linking = $ARGV[2];

#HERE IS WHERE WE CAN MULTI THREAD
my @chroms = qw/1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 X Y/;
foreach my $chromnum (@chroms) {
    my $chrom = "chr$chromnum";
    print createTrack( $chrom, $donor, $query_name, $linking );
}
#closedir( $target );
print "\n</textarea></body></html>";

sub createTrack {
    
    my ($host_chrom, $donor, $query_name, $bam_linking) = @_;
    
    my $template = $TRACK_TEMPLATE; #"tracks/$CHROM_PREFIX%s/$DONOR_PREFIX$donor/$QUERY_PREFIX$query_name";
    print "template: $template\n";
    my $targetdir = sprintf( "$DATA_DIR/$template", $host_chrom, $donor, $query_name );
    print "targetdir: $targetdir\n";


    #my $bamFile = "$targetdir/$query_name.bam";

    my $interval_file = "$targetdir/$query_name.intervals";
    if( ! -e $interval_file ){ return; }

    my $bam_histogram_filename = "$targetdir/$query_name.hist";
    $bam_linking = $bam_linking eq "linking";

    my $pregen_histograms;
    if( defined $bam_histogram_filename ){
        my $OLDSEP = $/;
        local $/=undef;
        open FILE, $bam_histogram_filename or die $!;
    
        my $json_text = <FILE>;
        #delete whitespace
        $json_text =~ s/\s+/ /g;
        #delete variable assignment (i.e ' histogram = ...')
        $json_text =~ s/^.+= //;

        #TODO OPTIMIZE
        #   tell Christos not to generate histogram data for bin sizes < 1000
        #   they take up huge room and forever to decode
        $pregen_histograms = JSON::decode_json($json_text);
        close FILE; 
        $/ = $OLDSEP;
    }

    my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $trackLabel, $nclChunk, $compress, $key, $featureCount);

    my $defaultClass = "transcript";
    my $defaultSubfeatureClasses = {"forward","forward-strand",
                                    "reverse","reverse-strand",
                                    "hanging","feature3",
                                    "SNP","SNP"};

    $cssClass = $defaultClass;
    $subfeatureClasses = $defaultSubfeatureClasses;

    $key = "$donor/$query_name";
    $trackLabel = $query_name;

    if (!defined($nclChunk)) {
        # default chunk size is 50KiB
        $nclChunk = 50000;
        # $nclChunk is the uncompressed size, so we can make it bigger if
        # we're compressing
        $nclChunk *= 4 if $compress;
    }

    my $trackRel = "tracks";
    my $trackDir = "$DATA_DIR/$trackRel";
    
    mkdir($DATA_DIR) unless (-d $DATA_DIR);
    mkdir($trackDir) unless (-d $trackDir);

    #my $index;
    #my $index = Bio::DB::Bam->index($bamFile, 1);
    #print "done with index, opening bamFIle\n";
    #my $bam = Bio::DB::Bam->open($bamFile);
    #my $bam = Bio::DB::Sam->new(-bam  => $bamFile);
    #print "bamFile opened\n";

    #my $hdr = $bam->header();
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

    my @refSeqs = @{JsonGenerator::readJSON("$DATA_DIR/refSeqs.js", [], 1)};
    my ($refseq_start,$refseq_end,$refseq_name) = 0,0,"";
    foreach my $seqInfo (@refSeqs) {
        if( $seqInfo->{name} eq $host_chrom ){
            $refseq_start = $seqInfo->{start};
            $refseq_end = $seqInfo->{end};
            $refseq_name = $host_chrom;
            last;
        }
    }
       
    my $jsonGen = JsonGenerator->new($targetdir,
                                     $nclChunk,
                                     $compress, 
                                     $trackLabel,
                                     $refseq_name,
                                     $refseq_start,
                                     $refseq_end,
                                     \%style, 
                                     \@bamHeaders, 
                                     \@subfeatureHeaders,
                                     $featureCount,
                                     $pregen_histograms);

    #ensures ties of left-sort are broken by right-end
    my $sorter = NCLSorter->new( sub { $jsonGen->addFeature($_[0]) },
                                 $startIndex, $endIndex);

    if( $bam_linking  ) {

        if( $stream ) {
            my $callback = sub {
                my ($callone,$calltwo) = @_;
                my ($absleft,$absright) = ($callone->[0], $calltwo->[1]);
                push( @$callone, $callone->[2]-1 ? "reverse" : "forward" );
                push( @$calltwo, $calltwo->[2]-1 ? "reverse" : "forward" );
                my $feature = [$absleft,$absright,0,[$callone,$calltwo]];
                #can somehow merge addFeature and updateBookmarks?  
                $sorter->addSorted($feature);
                updateBookmarks( $jsonGen,
                                 \$cur_left,
                                 \$cur_right,
                                 $feature );
            };

            my $streamer = PairStreamer->new( $callback );
#
            my $fetch_callback = sub { 
                passAlignmentToStreamer( $_[0], $streamer );
            };
            #print "starting stream...\n";
            #$index->fetch($bam,
                          #$tid,
                          #$start,
                          #$end,
                          #$fetch_callback);
            #$bam->fetch($tid,$start,$end,$fetch_callback);
            #print "done streaming\n";
        }
        else{

            my $callback = sub {
                my $feature = shift;
                updateBookmarks( $jsonGen,
                                 \$cur_left, 
                                 \$cur_right, 
                                 $feature );
                $sorter->addSorted( $feature ); 
            };

            print $OUTPUT "linkingi\n";

            open( FINT, '<', $interval_file );
            while( <FINT> ) {
                my $feat = makeFeature( $_ );
                $callback->( $feat );
            }
            
            # $_[0] is the alignment found by fetch
            # $_[1] is \%paired_info reference
            #print "beginning fetch\n";
            #while( my $align = $bam->read1 ){
                #new_linking_align2array( $align, $callback );
            #}
            #print "done fetching\n";
        }
    }
    else{
    }

    ##it could be that there are no gaps in reads, meaning updateBookmarks never adds anything to IAs
    #if thats the case, add the one giant interval herei
    #print "huh\n";
    my $perlIsGay = $jsonGen->{interestingAreas};
    my $countIA =  scalar @{ $perlIsGay };
    if( $countIA == 0 ){
        print $OUTPUT "no IAs, adding $cur_left, $cur_right\n";
        $jsonGen->addInterestingArea( $cur_left,$cur_right );
    }
    else {
        print $OUTPUT "what\n";
    }

    $sorter->flush();
    eval {
        $jsonGen->generateTrack();
        1;
    }
    or do {
        $bad_bam = 1;
    };
#
    my $status;
    if( $bad_bam ){
        $status = '{"status":"ERROR", "message":"Something went wrong with track generation."}';
        print $OUTPUT "bad bam\n";
    }
    else{
        print $OUTPUT "good bam\n";
        #add a new entry to trackInfo.js
        #what if we want to group specific tracks to particular reference sequences

        my $ext = ($compress ? "jsonz" : "json");
        my $new_entry_json = {
                              'label' => $trackLabel,
                              'key' => $key,
                              'url' => sprintf( "$template/trackData.$ext", "{refseq}", $donor, $query_name ),
                              #'url' => "$trackRel/{refseq}/query_" . $trackLabel . "/trackData.$ext",
                              'type' => "FeatureTrack",
                             };
        JsonGenerator::writeTrackEntry( "$DATA_DIR/trackInfo.js", $new_entry_json );

        $status = '{"status":"OK", "trackData":[';
        $status .=  JSON::to_json($new_entry_json, {pretty => 1});
        $status .= ']}';
    }

    print $OUTPUT "done\n";

    close $OUTPUT;
    close ERROR;

    return $status;
}

sub convertStrand {
    return shift eq 'F' ? (1,"forward") : (-1,"reverse");
}

sub makeFeature {
    my $intervals = shift;
    my @s = split( '\t', $intervals );
    my ($lstrand,$lstyle) = convertStrand($s[2]);
    my ($rstrand,$rstyle) = convertStrand($s[5]);
    my ($ll,$rr,$lr,$rl) = (int($s[0]), int($s[4]), int($s[1]), int($s[3]));
    return [$ll,$rr,0,[[$ll,$lr,$lstrand,$lstyle],[$rl,$rr,$rstrand,$rstyle]]];
}

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


sub new_linking_align2array{
    my ($alignment,$callback) = @_; 
    my $start = $alignment->pos+1;
    my $end = $alignment->calend+1;
    my $strand = $alignment->strand;
    my $mstart = $alignment->mate_start+1;
    my $mstrand = $alignment->mstrand;
    
    if( $start < $mstart ) {  
        my $mend = $mstart + ($end-$start +1 ) - 1;
        my $style = $strand+1 ? "forward" : "reverse";
        my $mstyle = $mstrand+1 ? "forward" : "reverse";
        my $feature = [$start,$mend,0,
                       [[$start,$end,$strand,$style],
                        [$mstart,$mend,$mstrand,$mstyle]]];
        $callback->($feature);
    }

        #5th bit is strand bit. 1 means reverse, 0 means forward
        #my $strand = ($alignment->flag & 0x10) >> 4;
        #$strand = $strand ? -1 : 1;
    

    #}
};



sub passAlignmentToStreamer {
    my ($alignment, $streamer) = @_;
    my $paired = $alignment->paired;
    my $proper_pair = $alignment->proper_pair;

    my $start = $alignment->pos+1;
    my $end = $alignment->calend+1;
    my $reversed = $alignment->reversed;
    my $strand = $alignment->strand;

    #my $mstart = $alignment->mate_start;
    #my $mend = $alignment->mate_end;
    #my $mreversed = $alignment->mreversed;
    #my $mstrand = $alignment->mstrand;

    #5th bit is strand bit. 1 means reverse, 0 means forward
    #my $strand = ($alignment->flag & 0x10) >> 4;
    #$strand = $strand ? -1 : 1;
    
    my $pair_name = $alignment->qname;

    #if( defined($mend) ){
        #if( $start < $mstart ){
            #$streamer->acceptFullPair( [$start,$end,$strand], 
                                       #[$mstart,$mend,$mstrand] );
        #}
    #}
    #else{
        $streamer->acceptHalfPair( $pair_name, [$start,$end,$strand] );
    #}

}

sub linking_align2array {
    my $align = shift;
    my $paired_info = shift;

    my $left = $align->pos+1;
    my $right = $align->calend+1;

    #5th bit is the strand bit. 1 means reverse, -1 means forward
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

#updating cur_right and cur_left through references
sub updateBookmarks {
    my $jsonGen = shift;
    my $cur_left = shift;
    my $cur_right = shift;
    my $align_array = shift;

    #print $OUTPUT "$align_array->[0], $align_array->[1]\n";
    #print $OUTPUT "curleft: $$cur_left, curright: $$cur_right\n";

    my ($left,$right) = ($align_array->[0], $align_array->[1]);
    if( $$cur_left <= $left and $left <= $$cur_right ){
        if( $right > $$cur_right ){ 
            $$cur_right = $right;
        }
    }
    else {
        if( $left - $$cur_right > 0 && $$cur_right > 0 ){ #> $INTERESTING_AREAS_GAP_THRESH ){
            $jsonGen->addInterestingArea( $$cur_left,$$cur_right );
        }
        ($$cur_left, $$cur_right) = ($left,$right);
    }    
    #print $OUTPUT "\n";
}

sub readInJsonFile {
    my $filename = shift;
    open( DERP, "<", $filename );
    local $/=undef;
    my $json_text = <DERP>;
    
    #delete whitespace
    $json_text =~ s/\s+/ /g;
    #delete what's leading up to the start of the object (i.e ' histogram = ')
    $json_text =~ s/^.+= //;
    
    return JSON::decode_json($json_text);
}



1;
