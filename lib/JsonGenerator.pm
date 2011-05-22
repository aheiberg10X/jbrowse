package JsonGenerator;

use base 'Exporter';
our @EXPORT_OK = qw/ readJSON writeJSON modifyJSFile /;

use strict;
use warnings;

use NCList;
use LazyNCList;
use JSON 2;
use IO::File;
use Fcntl ":flock";
use POSIX qw(ceil floor);
use List::Util qw(min max sum reduce);
use PerlIO::gzip;
use constant MAX_JSON_DEPTH => 2048;

use GlobalConfig;

#this series of numbers is used in JBrowse for zoom level relationships
my @multiples = (1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000,
                 10_000, 20_000, 50_000, 100_000, 200_000, 500_000, 1_000_000);

my $startIndex = 0;
my $endIndex = 1;
#position of the lazy subfeature file name in the fake feature.
my $lazyIndex = 2;

my $histChunkSize = 10_000;

my $density_estimate = 0.1;

my %builtinDefaults =
  (
   "class"        => "feature"
  );

sub readJSON {
    my ($file, $default, $skipAssign, $compress) = @_;
    if (-s $file) {
        my $OLDSEP = $/;
        my $fh = new IO::File $file, O_RDONLY
            or die "couldn't open $file: $!";
        binmode($fh, ":gzip") if $compress;
        flock $fh, LOCK_SH;
        # optionally skip variable assignment line
        $fh->getline() if $skipAssign;
        undef $/;
        $default = JSON::from_json(<$fh>);
        $fh->close()
            or die "couldn't close $file: $!";
        $/ = $OLDSEP;
    }
    return $default;
}

sub writeJSON {
    my ($file, $toWrite, $opts, $compress) = @_;

    # create JSON object
    my $json = new JSON;
    # set opts
    if (defined($opts) and ref($opts) eq 'HASH') {
        for my $method (keys %$opts) {
            $json->$method( $opts->{$method} );
        }
    }

    # check depth
    #my $depth = findDepth($toWrite);
    #my $maxDepth = $json->get_max_depth;
    # if ($depth >= $maxDepth) {
    #     my @deepPath = @{deepestPath($toWrite)};
    #     my $warning = "WARNING: found deep path (depth = " . $depth . ", max depth allowed = " . $maxDepth . ")\n";
    #     warn $warning;
    #     for my $n (0..$#deepPath) {
    #         my $elem = $deepPath[$n];
    #         my $type = ref($elem);
    #         if ($type eq 'HASH') {
    #     	warn $n, ": { ", join(", ", map("$_ => $$elem{$_}", keys %$elem)), " }\n";
    #         } elsif ($type eq 'ARRAY') {
    #     	warn $n, ": [ ", join(", ", map(defined($_) ? $_ : "undef", @$elem)), " ]\n";
    #         } else {
    #     	warn $n, ": ", $elem, "\n";
    #         }
    #     }
    #     warn $warning;  # repeat the warning after printing the trace
    # }

    # write
    my $fh = new IO::File $file, O_WRONLY | O_CREAT
      or die "couldn't open $file: $!";
    flock $fh, LOCK_EX;
    $fh->seek(0, SEEK_SET);
    $fh->truncate(0);
    if ($compress) {
        binmode($fh, ":gzip")
            or die "couldn't set binmode: $!";
    }
    $fh->print($json->encode($toWrite));
    $fh->close()
      or die "couldn't close $file: $!";

}

sub modifyJSFile {
    my ($file, $varName, $callback) = @_;
    my ($data, $assign);
    my $fh = new IO::File $file, O_RDWR | O_CREAT
      or die "couldn't open $file: $!";
    flock $fh, LOCK_EX;
    # if the file is non-empty,
    if (($fh->stat())[7] > 0) {
        # get variable assignment line
        $assign = $fh->getline();
        # get data
        my $jsonString = join("", $fh->getlines());
        $data = JSON::from_json($jsonString) if (length($jsonString) > 0);
        # prepare file for re-writing
        $fh->seek(0, SEEK_SET);
        $fh->truncate(0);
    }
    # add assignment line
    $fh->print("$varName = \n");
    # modify data, write back
    $fh->print(JSON::to_json($callback->($data), {pretty => 1}));
    $fh->close()
      or die "couldn't close $file: $!";
}

sub writeTrackEntry {
    my ($file, $entry) = @_;
    modifyJSFile($file, "trackInfo",
        sub {
            my $origTrackList = shift;
            my @trackList = grep { exists($_->{'label'}) } @$origTrackList;
            my $i;
            for ($i = 0; $i <= $#trackList; $i++) {
                last if ($trackList[$i]->{'label'} eq $entry->{'label'});
            }
            $trackList[$i] = $entry;
            
            #first hacky attempt to do autorefesh when done processing
            #open NEW_TRACK, '>', "$upload_dir/newly_added.js";
            #print NEW_TRACK "trackInfo = [";
            #print NEW_TRACK JSON::to_json($entry, {pretty => 1});
            #print NEW_TRACK "]";
            #close NEW_TRACK;

            return \@trackList;
        });
}

# turn perl subs from the config file into callable functions
sub evalSubStrings {
    my $hashref = shift;
    foreach my $key (keys %{$hashref}) {
        next if ("CODE" eq (ref $hashref->{$key}));

        if ("HASH" eq (ref $hashref->{$key})) {
            evalSubStrings($hashref->{$key});
        } else {
            $hashref->{$key} = eval($hashref->{$key})
              if (defined($hashref->{$key}) && $hashref->{$key} =~ /^\s*sub\s*{.*}\s*$/);
        }
    }
}

sub new {
    my ($class, $outDir, $chunkBytes, $compress, $label, $segName,
        $refStart, $refEnd, $setStyle, $headers, $subfeatHeaders,
        $featureCount, $pregen_histograms) = @_;

    my %style = ("key" => $label,
                 %builtinDefaults,
		 %$setStyle);

    evalSubStrings(\%style);

    my $self = {
        style          => \%style,
        label          => $label,
        outDir         => $outDir,
        chunkBytes     => $chunkBytes,
        compress       => $compress,
        sublistIndex   => $#{$headers} + 1,
        curMapHeaders  => $headers,
        subfeatHeaders => $subfeatHeaders,
        ext            => ($compress ? "jsonz" : "json"),
        refStart       => $refStart,
        refEnd         => $refEnd,
        count          => 0,
        pregen_histograms => $pregen_histograms,
        interestingAreas => []
    };

    # $featureCount is an optional parameter; if we don't know it,
    # then arbitrarily estimate that there's 0.25 features per base
    # (0.25 features/base is pretty dense, which gives us
    # a relatively high-resolution histogram; we can always throw
    # away the higher-resolution histogram data later, so a dense
    # estimate is conservative.  A dense estimate does cost more RAM, though)
    $featureCount = $refEnd * $density_estimate unless defined($featureCount);

    # $histBinThresh is the approximate the number of bases per
    # histogram bin at the zoom level where FeatureTrack.js switches
    # to the histogram view by default
    my $histBinThresh = ($refEnd * $density_estimate * 100) / $featureCount;
    $self->{histBinBases} = $multiples[0];
    foreach my $multiple (@multiples) {
        $self->{histBinBases} = $multiple;
        last if $multiple > $histBinThresh;
    }
    open( my $OUTPUT, '>', $upload_dir . "/" . "jsongen_output2.txt" ) or die $!;
    my $temp = $self->{histBinBases};
    print $OUTPUT "histBinThresh: $histBinThresh\n";
    print $OUTPUT "histBinBases: $temp\n";

    $self->{hists} = [];
    if( ! defined $pregen_histograms ){
        # initialize histogram arrays to all zeroes
        for (my $i = 0; $i <= $#multiples; $i++) {
            my $binBases = $self->{histBinBases} * $multiples[$i];
            $self->{hists}->[$i] = [(0) x ceil($refEnd / $binBases)];
            my $temp = ceil($refEnd / $binBases);

            print $OUTPUT "zoom level $i has $temp bins\n";

            # somewhat arbitrarily cut off the histograms at 100 bins
            last if $binBases * 100 > $refEnd;
        }
    }
    #else do nothing, wait until generateTrack to pull in pregen data
    close $OUTPUT;

    
    mkdir($outDir) unless (-d $outDir);
    unlink (glob $outDir . "/hist*");
    unlink (glob $outDir . "/lazyfeatures*");
    unlink $outDir . "/trackData.json";

    my $lazyPathTemplate = "$outDir/lazyfeatures-{chunk}." . $self->{ext};

    # $output writes out the feature JSON chunk file
    my $output = sub {
        my ($toWrite, $chunkId) = @_;
        #print STDERR "writing chunk $chunkId\n";
        (my $path = $lazyPathTemplate) =~ s/\{chunk\}/$chunkId/g;
        writeJSON($path,
                  $toWrite,
                  {pretty => 0, max_depth => MAX_JSON_DEPTH},
                  $compress);
    };

    # $measure measures the size of the feature in the final JSON
    my $measure = sub {
        # add 1 for the comma between features
        # (ignoring, for now, the extra characters for sublist brackets)
        return length(JSON::to_json($_[0])) + 1;
    };

    $self->{sublistIndex} += 1 if ($self->{sublistIndex} == $lazyIndex);
    $self->{features} = LazyNCList->new($startIndex, $endIndex,
                                        $self->{sublistIndex},
                                        $lazyIndex,
                                        $measure,
                                        $output,
                                        $chunkBytes);

    bless $self, $class;
    return $self;
}

sub addInterestingArea {
    my ($self, $site) = @_;
    my $iaref =  $self->{interestingAreas};
    push( @$iaref, $site );
}

sub addFeature {
    my ($self, $feature) = @_;

    $self->{features}->addSorted($feature);
    $self->{count}++;

    if( ! defined $self->{pregen_histograms} ){
        my $histograms = $self->{hists};
        my $curHist;
        my $start = max(0, min($feature->[$startIndex], $self->{refEnd}));
        my $end = min($feature->[$endIndex], $self->{refEnd});
        return if ($end < 0);

        for (my $i = 0; $i <= $#multiples; $i++)
        {
            my $binBases = $self->{histBinBases} * $multiples[$i];
            $curHist = $histograms->[$i];
            last unless defined($curHist);

            my $firstBin = int($start / $binBases);
            my $lastBin = int($end / $binBases);
            for (my $bin = $firstBin; $bin <= $lastBin; $bin++)
            {
                $curHist->[$bin] += 1;
            }
        }
    }
}

sub featureCount {
    my ($self) = @_;
    return $self->{count};
}

sub hasFeatures {
    my ($self) = @_;
    return $self->{count} >= 0;
}

sub generateTrack {
    my ($self) = @_;
    open( my $OUTPUT, '>', $upload_dir . "/" . "jsongen_output.txt" ) or die $!;

    my $ext = $self->{ext};
    my $features = $self->{features};
    $features->finish();

    # approximate the number of bases per histogram bin at the zoom level where
    # FeatureTrack.js switches to histogram view, by default
    my $histBinThresh = ($self->{refEnd} * $density_estimate * 10) / $self->{count};

    # find multiple of base hist bin size that's just over $histBinThresh
    my $i;
    for ($i = 1; $i <= $#multiples; $i++) {
        last if ($self->{histBinBases} * $multiples[$i]) > $histBinThresh;
    }
    
    if( $DEBUG ){ 
        print $OUTPUT "histBinThresh: $histBinThresh\n"; 
        my $temp = $self->{histBinBases};
        print $OUTPUT "histBinBases: $temp\n";
        print $OUTPUT "i: $i\n";
    }

    my @histogramMeta;
    #############################
    ### pregen hisogram meta ###
    if( defined $self->{pregen_histograms} ){
        my @sorted = sort { $a->{basesPerBin} <=> $b->{basesPerBin} } @{$self->{pregen_histograms}};
        $self->{pregen_histograms} = \@sorted;
        print $OUTPUT "sorted it\n";
        my $num_pregen_hists = scalar @{$self->{pregen_histograms}};
        #my $num_hists = scalar @{$self->{hists}};
        print $OUTPUT "numhistlevels: $num_pregen_hists\n";

        #they are order in descending order (bpPerBin), want to insert then in ascending
        #$self->{pregen_histograms} = reverse($self->{pregen_histograms});
        for( my $k = 0; $k < $num_pregen_hists; $k++ ){
            $self->{hists}->[$k] = $self->{pregen_histograms}->[$k]->{counts};
        }

        $i = 1; #($#multiples + 1) - $num_pregen_hists + 1;
        #set $i to reflect how many histograms were pre-generated
        if( $DEBUG ){ print $OUTPUT "new i is $i\n"; }
    }
    ### pregen histogram meta ###
    #############################
   

    # Generate more zoomed-out histograms so that the client doesn't
    # have to load all of the histogram data when there's a lot of it.
    for (my $j = $i - 1; $j <= $#multiples; $j += 1) {
        my $curHist = $self->{hists}->[$j];
        last unless defined($curHist);

        my $histBases;
        if( ! defined $self->{pregen_histograms} ){
            $histBases = $self->{histBinBases} * $multiples[$j];
        }
        else{
            $histBases = $self->{pregen_histograms}->[$j]->{basesPerBin};
        }
    
        if( $DEBUG ){
            print $OUTPUT "    binBases[$j]: $histBases\n";
        }

        my $chunks = chunkArray($curHist, $histChunkSize);
        for (my $i = 0; $i <= $#{$chunks}; $i++) {
            writeJSON($self->{outDir} . "/hist-$histBases-$i.$ext",
                      $chunks->[$i],
                      {pretty => 0},
                      $self->{compress});
        }
        push @histogramMeta,
            {
                basesPerBin => $histBases,
                arrayParams => {
                    length => $#{$curHist} + 1,
                    urlTemplate => "hist-$histBases-{chunk}.$ext",
                    chunkSize => $histChunkSize
                }
            };
    }

    my @histStats;
    for (my $j = $i - 1; $j <= $#multiples; $j++) {
        last unless defined($self->{hists}->[$j]);
        my $binBases;
        if( ! defined $self->{pregen_histograms} ){
            $binBases = $self->{histBinBases} * $multiples[$j];
        }
        else{
            $binBases = $self->{pregen_histograms}->[$j]->{basesPerBin};
        }
        push @histStats, {'bases' => $binBases,
                          arrayStats($self->{hists}->[$j])};
    }

    my $trackData = {
                     'label' =>
                         $self->{label},
                     'key' =>
                         $self->{style}->{key},
                     'sublistIndex' =>
                         $self->{sublistIndex},
                     'lazyIndex' =>
                         $lazyIndex,
                     'headers' =>
                         $self->{curMapHeaders},
                     'featureCount' =>
                         $self->{count},
                     'type' =>
                         "FeatureTrack",
                     'className' =>
                         $self->{style}->{class},
                     'subfeatureClasses' =>
                         $self->{style}->{subfeature_classes},
                     'subfeatureHeaders' =>
                         $self->{subfeatHeaders},
                     'arrowheadClass' =>
                         $self->{style}->{arrowheadClass},
                     'clientConfig' =>
                         $self->{style}->{clientConfig},
                     'featureNCList' =>
                         $self->{features}->topLevelList,
                     'lazyfeatureUrlTemplate' =>
                         "lazyfeatures-{chunk}.$ext",
                     'histogramMeta' =>
                         \@histogramMeta,
                     'histStats' =>
                         \@histStats,
                     'interestingAreas' => 
                         $self->{interestingAreas}
                    };
    $trackData->{urlTemplate} = $self->{style}->{urlTemplate}
      if defined($self->{style}->{urlTemplate});
    writeJSON($self->{outDir} ."/trackData.$ext",
              $trackData,
              {pretty => 0, max_depth => MAX_JSON_DEPTH},
              $self->{compress});

    close $OUTPUT;
}

sub arrayStats {
    my $arr = shift;
    my $max = max(@$arr);
    my $sum = sum(@$arr);
    my $mean = $sum / ($#{$arr} + 1);
#    my $var = sum(map {($_ - $mean) ** 2} @$arr) / ($#{$arr} + 1);
#    return ('max' => $max, 'sum' => $sum,
#            'mean' => $mean, 'var' => $var,
#            'stddev' => sqrt($var));
    return ('max' => $max, 'mean' => $mean);
}

sub chunkArray {
    my ($bigArray, $chunkSize) = @_;

    my @result;
    for (my $start = 0; $start <= $#{$bigArray}; $start += $chunkSize) {
        my $lastIndex = $start + $chunkSize;
        $lastIndex = $#{$bigArray} if $lastIndex > $#{$bigArray};

        push @result, [@{$bigArray}[$start..$lastIndex]];
    }
    return \@result;
}

# findDepth returns the depth of the deepest element(s) in the structure
# the code is the iterative form of findDepth($obj) = 1 + max(map(findDepth($_), childArray($obj)))
# where childArray($obj) = values(%$obj) [for a hash], @$obj [for an array] or the empty list [for a scalar]
sub findDepth {
    my ($obj) = @_;
    my ($depth, $childArray, $childIndex);
    my @stack;

  FD_NEW_OBJ:
    my $type = ref($obj);
    $childArray = $type eq 'HASH' ? [values %$obj] : ($type eq 'ARRAY' ? $obj : []);
    $depth = 0;
    $childIndex = 0;
  FD_CHILD_LOOP:
    if ($childIndex < @$childArray) {
	push @stack, [$depth, $childArray, $childIndex];
	$obj = $childArray->[$childIndex];
	goto FD_NEW_OBJ;
    } elsif (@stack) {
	my $childDepth = $depth + 1;
	my $vars = pop @stack;
	($depth, $childArray, $childIndex) = @$vars;
	if ($childDepth > $depth) {
	    $depth = $childDepth;
	}
	++$childIndex;
	goto FD_CHILD_LOOP;
    }

    return $depth + 1;
}

# deepestPath returns the path to (the first of) the deepest element(s) in the structure
# the code is the iterative form of deepestPath($obj) = ($obj, longest(map(deepestPath($_), childArray($obj))))
# where childArray($obj) = values(%$obj) [for a hash], @$obj [for an array] or the empty list [for a scalar]
# and longest(@x1, @x2, ... @xn) returns the longest of the given arrays (or the first such, in the event of a tie)
sub deepestPath {
    my ($obj) = @_;
    my ($trace, $childArray, $childIndex);
    my @stack;

  DP_NEW_OBJ:
    my $type = ref($obj);
    $childArray = $type eq 'HASH' ? [values %$obj] : ($type eq 'ARRAY' ? $obj : []);
    $trace = [];
    $childIndex = 0;
  DP_CHILD_LOOP:
    if ($childIndex < @$childArray) {
	push @stack, [$obj, $trace, $childArray, $childIndex];
	$obj = $childArray->[$childIndex];
	goto DP_NEW_OBJ;
    } elsif (@stack) {
	my $childTrace = [$obj, @$trace];
	my $vars = pop @stack;
	($obj, $trace, $childArray, $childIndex) = @$vars;
	if (@$childTrace > @$trace) {
	    $trace = $childTrace;
	}
	++$childIndex;
	goto DP_CHILD_LOOP;
    }

    return [$obj, @$trace];
}

1;

=head1 AUTHOR

Mitchell Skinner E<lt>mitch_skinner@berkeley.eduE<gt>

Copyright (c) 2007-2009 The Evolutionary Software Foundation

This package and its accompanying libraries are free software; you can
redistribute it and/or modify it under the terms of the LGPL (either
version 2.1, or at your option, any later version) or the Artistic
License 2.0.  Refer to LICENSE for the full license text.

=cut
