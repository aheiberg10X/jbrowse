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
open( my $OUTPUT, '>', $UPLOAD_DIR . "/" . "bam_output.txt" ) or die $!;
open ERROR,  '>', $UPLOAD_DIR . "/" . "bam_error.txt"  or die $!;
STDERR->fdopen( \*ERROR,  'w' ) or die $!;
#### DEBUGGING OUTPUT ###

my $stream = 0;

my $profiling = 1;
if( $profiling ){
    my $path = "/home/andrew/school/dnavis/jbrowse/profiling";
    my $option = "big";
    my $bam_file = "$path/profile_$option.bam";

#"/home/andrew/school/dnavis/svn/jbrowse/Apr28/hack/evidence.or.missFR.5.bam";
#"/home/andrew/school/dnavis/svn/jbrowse/dinh_seg_dups/bam_files/hack/reg_chr18:96741-100669.bam";

#"/home/andrew/school/dnavis/svn/jbrowse/dinh_seg_dups/bam_files/hack/reg_chr18:96741-100669.bam";
    my $host_chrom = "chr1";
    my $linking = "linking";
    my $histogram_filename = "$path/profile_big.hist";

    createTrack( $bam_file, $host_chrom, $linking, $histogram_filename );
    exit;
}
######################################################
#################### CGI STUFF #######################


my $cgi = CGI::new();
#my $bam_filename = $cgi->param('bam_filename');                       #input type=file
#my $bam_linking = defined $cgi->param('display_linking');             
#my $bam_histogram_filename = $cgi->param("bam_histogram_filename");   #input type=file
#my $host_chrom = $cgi->param("host_chrom");
##my $refseq_name = $cgi->param('refseq_name');  #sequence name the reads in $bam_filename pertain to


#if( $DEBUG ) {
    #print $OUTPUT "we getting the auto-histogram file?: $bam_histogram_filename\n";
    #print $OUTPUT $UPLOAD_DIR ."/". $bam_filename;
#}

#print $OUTPUT "printing headres\n";
print $cgi->header;
print "<html><body><textarea>\n";  #place a json response inside here"
my $host_chrom = $ARGV[0];
my $donor = $ARGV[1];
my $query_name = $ARGV[2];
my $linking = $ARGV[3];
opendir( my $target, "$DATA_DIR/tracks/$CHROM_PREFIX\_$host_chrom/$DONOR_PREFIX\_$donor" );
while(my $dir = readdir($target)) {
    print createTrack( "$target/$dir", $host_chrom, $query_name, $linking );
}
closedir( $target );
print "\n</textarea></body></html>";
##########################################################################
########################  UPLOADING  #####################################

##write the data to $UPLOAD_DIR, TODO: is it necessary to write to a file, can't we just save in a variable a la $pregen_histograms below?
##print $OUTPUT "writing upload file\n";
#my $bamFile =  "$UPLOAD_DIR/$bam_filename";
#open (OUTFILE, ">", $bamFile) or die "Couldn't open $bam_filename for writing: $!";
#while(<$bam_filename>){
  #print OUTFILE $_;
#}
#close OUTFILE;
#print $OUTPUT "done writing upload file\n";

#my $pregen_histograms;   #user has the option to upload pregenerated historgram data, saving JsonGenerator from having to figure it out
#if( defined $bam_histogram_filename and $bam_histogram_filename ne ''){
    #local $/=undef;
    #my $json_text = <$bam_histogram_filename>;
    
    ##delete whitespace
    #$json_text =~ s/\s+/ /g;
    ##delete what's leading up to the start of the object (i.e ' histogram = ')
    #$json_text =~ s/^.+= //;
    
    #$pregen_histograms = JSON::decode_json($json_text);
#}

########################## UPLOADING  ####################################
##########################################################################

#in: bam file, chrom, linking, histogram file    
#out: json  
sub createTrack {
    
    my ($targetdir, $host_chrom, $query_name, $bam_linking) = @_;
    
    my $bamFile = "$targetdir/$query_name.bam";
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

    #my ($name, $path, $extension) = fileparse( $bamFile, (".bam") );
    #print $OUTPUT "bamFIle: $bamFile, name: $name, path: $path, ext: $extension\n";
    ##treat the filename, sans extension, as the key and trackLabel
    #my $reversed = reverse $bam_filename;
    #my ($extension, $folders) = split(/\./, $reversed, 2);
    #$extension = reverse $extension;
    #$folders = reverse $folders;
    #my @folders = split(/\//,$folders);
    $key = $query_name;
    $trackLabel = $key;
    print $OUTPUT "key: $key\n";
    #print $OUTPUT "after deducing filename\n";

    if (!defined($nclChunk)) {
        # default chunk size is 50KiB
        $nclChunk = 50000;
        # $nclChunk is the uncompressed size, so we can make it bigger if
        # we're compressing
        $nclChunk *= 4 if $compress;
    }

    my $trackRel = "tracks";
    my $trackDir = "$DATA_DIR/$trackRel";
    #mkdir($DATA_DIR) unless (-d $DATA_DIR);
    #mkdir($trackDir) unless (-d $trackDir);

    #my $bam = 7;
    #my $hdr = 7;

    #print "building index\n";
    my $index;
    #my $index = Bio::DB::Bam->index($bamFile, 1);
    #print "done with index, opening bamFIle\n";
    my $bam = Bio::DB::Bam->open($bamFile);
    #my $bam = Bio::DB::Sam->new(-bam  => $bamFile);
    print "bamFile opened\n";

    print $OUTPUT "opening upload file\n";
    my $hdr = $bam->header();
    print $OUTPUT "after header\n";
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

    print $OUTPUT "data_dir: $DATA_DIR\n";
    my @refSeqs = @{JsonGenerator::readJSON("$DATA_DIR/refSeqs.js", [], 1)};
    print $OUTPUT "host_chrom: $host_chrom\n";
    my ($refseq_start,$refseq_end,$refseq_name) = 0,0,"";
    foreach my $seqInfo (@refSeqs) {
        if( $seqInfo->{name} ne $host_chrom ){
            next;
        }
        else{
            $refseq_start = $seqInfo->{start};
            $refseq_end = $seqInfo->{end};
            $refseq_name = $host_chrom;
            last;
        }
    }
    if( $refseq_name eq "" ){
        my $a = 1;
        #badbad
    }
       
    print $OUTPUT "refseq_name: $refseq_name\n";
    my ($tid,$start,$end) = $hdr->parse_region($refseq_name);

    print $OUTPUT "(bogus) tid: $tid, start: $start, end: $end\n";

    my $DBHNDL;
    open $DBHNDL, ">", "$UPLOAD_DIR/bam2ncl.txt";
    print $DBHNDL "test rpint\n";

    if ( defined($tid) ) {
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
        my $sorter = NCLSorter->new(sub { $jsonGen->addFeature($_[0]) },
                                    $startIndex, $endIndex,
                                    $DBHNDL);

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
                print "starting stream...\n";
                #$index->fetch($bam,
                              #$tid,
                              #$start,
                              #$end,
                              #$fetch_callback);
                $bam->fetch($tid,$start,$end,$fetch_callback);
                print "done streaming\n";
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
                
                # $_[0] is the alignment found by fetch
                # $_[1] is \%paired_info reference
                print "beginning fetch\n";
                while( my $align = $bam->read1 ){
                    new_linking_align2array( $align, $callback );
                }
                print "done fetching\n";
                #$index->fetch($bam,
                              #$tid,
                              #0, #$start,
                              #10000000, #$end,
                              #sub { new_linking_align2array( $_[0], $sorter) });
                              ##sub { linking_align2array( $_[0], $_[1]) }, \%paired_info);
                #print "done with fetch, starting sort\n";
   # 
                #my @sorted = sort {$paired_info{$a}[0] <=> $paired_info{$b}[0]} 
                             #keys %paired_info;
                #print "done with sort, starting to add to sorter\n";
                #foreach my $key (@sorted){
                    #updateBookmarks( $jsonGen,
                                     #\$cur_left, 
                                     #\$cur_right, 
                                     #$paired_info{$key} );
                    #$sorter->addSorted( $paired_info{$key} );
                #}
                #print "done adding to sorter\n";
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

        #it could be that there are no gaps in reads, meaning updateBookmarks never adds anything to IAs
        #if thats the case, add the one giant interval herei
        print "huh\n";
        my $perlIsGay = $jsonGen->{interestingAreas};
        print $OUTPUT "$perlIsGay\n";
        my $countIA =  scalar @{ $perlIsGay };
        print $OUTPUT "count IA: $countIA \n";
        if( $countIA == 0 ){
            print $OUTPUT "no IAs, adding $cur_left, $cur_right\n";
            $jsonGen->addInterestingArea( $cur_left,$cur_right );
        }
        else {
            print $OUTPUT "what\n";
        }

        $sorter->flush();
        print "lkasdlkfasjdf\n";
        #catching error, i.e not finding alignments in BAM file and consequently dividing by 0
        eval {
            print "starting generate track\n";
            $jsonGen->generateTrack();
            print "done with generate track\n";
            1;
        } or do {
            print $OUTPUT $@;
            $bad_bam = 1;
        }
    }

    my $status;
    if( $bad_bam ){
        $status = '{"status":"ERROR", "message":"This BAM file can\'t be read by Bio::DB::Bam (reporting that there are 0 alignments)"}';
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
                              'url' => "$trackRel/{refseq}/query_" . $trackLabel . "/trackData.$ext",
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
    close $DBHNDL;

    return $status;
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
    #my $paired = $alignment->paired;
    my $start = $alignment->pos+1;
    my $end = $alignment->calend+1;
    #my $length = $alignment->length;
    my $strand = $alignment->strand;
    #my $reversed = $alignment->reversed;
    #my $mreversed = $alignment->mreversed;
    my $mstart = $alignment->mate_start+1;
    #my $mate_end = $alignment->mate_end;
    #my $mate_len = $alignment->mate_len;
    my $mstrand = $alignment->mstrand;
    #my $pair_name = $alignment->qname;
    #my $unmapped = $alignment->unmapped;
    #my $munmapped = $alignment->munmapped;    
    
    #print "$pair_name - [$left,$right] UNMapped: ($unmapped|$munmapped) Rev: ($reversed|$mreversed) Len: ($length | $mate_len) \n";
    #print "$pair_name, $left\n";
    #if( $left == 5989217 ){
        #print "SPECIAL$paired - [$left,$right]($reversed) ---> [$mate_start,$mate_end]($mreversed)\n";
    #}
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
