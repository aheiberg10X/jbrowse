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
use File::Basename;
use IO::Handle;
use PairStreamer;
use Cwd;
use GlobalConfig;
use Data::Dumper;

my $project = $ARGV[0];
my $donor = $ARGV[1];
my $query_name = $ARGV[2];
my $chromnum = $ARGV[3];
my $interval_file = $ARGV[4];
my $linking = $ARGV[5];
my $assembly = $ARGV[6];

if( not defined $chromnum ){ 
    $chromnum = "profile";
    $interval_file = "abc";}

#### DEBUGGING OUTPUT ###
open( my $OUTPUT, '>', $DEBUG_DIR . "/" . "bam_output_$chromnum.txt" ) or die $!;
open ERROR,  '>', $DEBUG_DIR . "/" . "bam_error_$chromnum.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;
#### DEBUGGING OUTPUT ###

#for building our response to server
my ($trackkey, $message);
my @messages = ();

#my $stream = 0;

my $profiling = 0; 
if( $profiling ){
    my $path = "/home/andrew/school/dnavis/jbrowse/genomequery/biosql_compiler/biosql/dst/chr1"; #"/home/andrew/school/dnavis/jbrowse/profiling";
    my $option = "big";
    my $bam_file = "$path/out.evidence.bam.short"; #"$path/"$path/profile_$option.bam";
    my $host_chrom = "chr1";
    my $linking = "linking";
    my $histogram_filename = "$path/out.evidence.hist";

    ($trackkey, $message) = createTrack( "dev", "NA18506", "delconc", "1", "/home/andrew/jbrowse/data/tracks/project_dev/donor_NA18506/query_delconc/chrom_chr1/out_center+1/out_center+1.interval", "linking", "hg18", 0 );
    print $OUTPUT "message: ", $message, "\n";
    exit;
}

my $compress = 0;

($trackkey, $message) = createTrack( $project, $donor, $query_name, $chromnum, $interval_file, $linking, $assembly, $compress );

print $OUTPUT "return message: $message\n";
print $message;
close $OUTPUT;
close ERROR;

#TODO
#incorporate $query_indices into visualization, right now it is ignored
sub createTrack {
    
    my ($project, $donor, $query_name, $chromnum, $interval_file, $bam_linking, $assembly, $compress ) = @_;

    my $host_chrom = "chr$chromnum";    
    my $template = $TRACK_TEMPLATE; 

    #my $targetdir = sprintf( "$DATA_DIR/$template", $project, $donor, $query_name, $host_chrom );
    my @splt = split( "/", $interval_file );
    print $OUTPUT @splt[0..$#splt-1];
    my $targetdir = join( "/", @splt[0..$#splt-1] );
    print $OUTPUT "targetdir: $targetdir\n";
    
    $bam_linking = $bam_linking eq "linking";
    print $OUTPUT "bam_linking: $bam_linking\n";
    print $OUTPUT "assembly: $assembly\n";
    print $OUTPUT "interval file $interval_file\n";

    my ($tracks, $cssClass, $arrowheadClass, $subfeatureClasses, $clientConfig, $trackLabel, $nclChunk, $key);
    $key = "$project/$query_name";
    $trackLabel = $query_name;

    #TODO
    #do all of them?  do the last (currently)?
    #my @splt = split( ",", $query_indices );
    #my $query_index = @splt[-1];
    #my $interval_file = "$targetdir/out+$query_index.interval";
    #my $interval_file = "$targetdir/$query_name\_$chromnum.intervals";
    if( ! -e $interval_file ){ 
        print $OUTPUT "interval file $interval_file does not exist\n";
        return ($key, "Nothing to visualize"); 
    }

    #my $bam_histogram_filename = "$targetdir/$query_name\_$chromnum.hist";

    my @splt = split("\\.", $interval_file);
    print $OUTPUT "i hate perl @splt";
    print $OUTPUT $splt[0];
    my $interval_file_name = $splt[0];
    my $bam_histogram_filename = "$interval_file_name.hist";
    print $OUTPUT "histo file $bam_histogram_filename\n";
    my $pregen_histograms;
    #if( defined $bam_histogram_filename ){
    if( ! -e $bam_histogram_filename ) {
        print $OUTPUT "ussing dummy hist in $DATA_DIR\n";
        $bam_histogram_filename = "$DATA_DIR/dummy.hist";
    }
    print $OUTPUT "historgram file exists\n";
    my $OLDSEP = $/;
    local $/=undef;
    open FILE, $bam_histogram_filename or die $!;
     
    my $json_text = <FILE>;
    #delete whitespace
    $json_text =~ s/\s+/ /g;
    #delete variable assignment (i.e ' histogram = ...')
    $json_text =~ s/^.+= //;
    
    $pregen_histograms = JSON::decode_json($json_text);
    close FILE; 
    $/ = $OLDSEP;

    my $defaultClass = "transcript";
    my $defaultSubfeatureClasses = {"forward","forward-strand",
                                    "reverse","reverse-strand",
                                    "hanging","hanging",
                                    "SNP","SNP"};

    $cssClass = $defaultClass;
    $subfeatureClasses = $defaultSubfeatureClasses;


    if (!defined($nclChunk)) {
        # default chunk size is 50KiB
        $nclChunk = 50000;
        # $nclChunk is the uncompressed size, so we can make it bigger if
        # we're compressing
        $nclChunk *= 4 if $compress;
    }

    print $OUTPUT "we here?\n";

    my $trackRel = "tracks";
    my $trackDir = "$DATA_DIR/$trackRel";
    
    mkdir($DATA_DIR) unless (-d $DATA_DIR);
    mkdir($trackDir) unless (-d $trackDir);

    my $bad_bam = 0;

    my @bamHeaders = ("start", "end", "strand","subfeatures", "depth");
    my @subfeatureHeaders = ("start","end","strand","type", "depth");

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


    #my @refSeqs = @{JsonGenerator::readJSON("$DATA_DIR/refSeqs.js", [], 1)};
    print $OUTPUT "getting refSeqs\n";
    my @refSeqs = @{JsonGenerator::readJSON("$DATA_DIR/refSeqs.js", [], 1)->{$assembly}};
    print $OUTPUT "got refSeqs\n", Dumper(@refSeqs);
    #TODO: grab out the entry corresponding to the correct assembly
    # will have to pass this into the script 

    my ($refseq_start,$refseq_end,$refseq_name) = 0,0,"";
    foreach my $seqInfo (@refSeqs) {
        #print $OUTPUT "onething: ", Dumper($seqInfo);
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
                                     $pregen_histograms);

    #ensures ties of left-sort are broken by right-end
    my $startIndex = 0;
    my $endIndex = 1;
    my $sorter = NCLSorter->new( sub { $jsonGen->addFeature($_[0]) },
                                 $startIndex, $endIndex);

    open( FINT, '<', $interval_file );
    my $line = <FINT>;
    @splt = split('\t', $line);
    my $len = scalar(@splt);
    print $OUTPUT "splt is @splt and length is $len";
    my $is_single_reads = $len < 6;

    my ($cur_left, $cur_right, $feature_count) = (0,0,0);
    
    my $callback = sub {
        my $feature = shift;
        updateInterestingAreas( $jsonGen,
                         \$cur_left, 
                         \$cur_right, 
                         \$feature_count,
                         $feature );
        $sorter->addSorted( $feature ); 
    };

    if( $is_single_reads ){
        print $OUTPUT "single reads, not linking\n";
        do{
            $line =~ s/\s*$//g; 
            my $feat = makeSingleFeature( $line );
            $callback->( $feat );
        }
        while( $line = <FINT> );
    }
    else {
        print $OUTPUT "linkingi\n";
        do {
            $line =~ s/\s*$//g; 
            my $feat = makePairedFeature( $line );
            $callback->( $feat );
        }
        while( $line = <FINT> ); 
    }
    close FINT;
    

    if( $feature_count <= 0 ){
        return ($key, "There are 0 features");
    }
    #it could be that there are no gaps in reads,
    #meaning updateInterestingAreas never adds anything to IAs
    #if thats the case, add the one giant interval herei
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
        $jsonGen->generateTrack($feature_count);
        1;
    }
    or do {
        $bad_bam = 1;
    };
#
    if( $bad_bam ){
        $message = "$host_chrom - $interval_file: Error with track generation";
    }
    else{
        $message = "$host_chrom - $interval_file: Track generated successfully"
    }
    
    return ($key, $message);
}

sub convertStrand {
    my ($leftpos,$strand) = @_;
    if( $leftpos == -1 ){
        return (0,"unmapped");
    }
    else {
        return $strand eq 'F' ? (1,"forward") : (-1,"reverse");
    }
}

sub makeSingleFeature {
    my $intervals = shift;
    my @s = split( '\t', $intervals );
    my ($strand,$style) = convertStrand(42, $s[2]);
    my ($ll,$rr) = (int($s[0]), int($s[1]));
    return [$ll,$rr,$strand,$style];
}

sub makePairedFeature {
    my $intervals = shift;
    my @s = split( '\t', $intervals );
    my ($ll,$rr,$lr,$rl) = (int($s[0]), int($s[4]), int($s[1]), int($s[3]));
    my ($lstrand,$lstyle) = convertStrand($ll,$s[2]);
    my ($rstrand,$rstyle) = convertStrand($rl,$s[5]);

    #left read unmapped, right all alone
    if( $lstrand == 0 ){
        [$rl,$rr,0,[[$rl,$rr,0,"hanging"]]];
    }
    #right read unmapped, left all alone
    elsif( $rstrand == 0 ){
        [$ll,$lr,0,[[$ll,$lr,0,"hanging"]]];
    }
    #both present
    else{
        return [$ll,$rr,0,[[$ll,$lr,$lstrand,$lstyle],[$rl,$rr,$rstrand,$rstyle]]];
    }
}


#updating cur_right and cur_left through references
sub updateInterestingAreas {
    my $jsonGen = shift;
    my $cur_left = shift;
    my $cur_right = shift;
    my $feature_count = shift;
    my $align_array = shift;

    $$feature_count += 1;

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
}


1;



###################################################################################
######### OLD STUFF for when we interacted with BAM files #########################
###################################################################################

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

