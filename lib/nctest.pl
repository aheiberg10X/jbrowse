#!/usr/bin/perl

use strict;
use warnings;

use CGI qw(:standard);

use FindBin qw($Bin);
use lib ("$Bin/../lib", "/usr/local/lib/perl5", "/usr/local/lib/perl5/site_perl","/home/aheiberg/perl5/lib/perl5/");

use Getopt::Long;
use JsonGenerator;
use NCList;
use LazyNCList;
use LazyNCList2;
use NCLSorter;
use JSON 2;
use Bio::DB::Sam;
use Bio::DB::Bam::AlignWrapper;
use File::Basename;
use IO::Handle;
use Data::Dumper;
use Cwd;
use GlobalConfig;
use PairStreamer;



##reading in an ersatz $paired_info for sandboxing with lazy loading of vertical data
#my %paired_info = %{readInJsonFile( "../data/ersatz_intervals_1.js" )};
#my $default;
#my %paired_info = %{JsonGenerator::readJSON( "../data/ersatz_intervals_1.js", $default, 0, 0)};
#my $one = $paired_info{"one"};
#my $two = $paired_info{"two"};
#my $three = $paired_info{"three"};
#my @features;
#$features[0] = $one;
#push( @features, $three );

#my $measure = sub {
    #return 1;
#};
#
#my $lazyPathTemplate = "/home/andrew/school/dnavis/jbrowse/test/lazyfeatures-{chunk}.json";
#my $output = sub {
    #my ($toWrite, $chunkId) = @_;
    ##print STDERR "writing chunk $chunkId\n";
    #(my $path = $lazyPathTemplate) =~ s/\{chunk\}/$chunkId/g;
    #JsonGenerator::writeJSON($path,
                             #$toWrite,
                             #{pretty => 0, max_depth => 2048},
                             #0);
#};
#
#
#my $LNCL = LazyNCList2->new(0,1,4,2,$measure,$output,1);
#$LNCL->addSorted( $one );
#$LNCL->addSorted( $two );
#$LNCL->addSorted( $three );
#$LNCL->finish();
#
#print Dumper($LNCL->topLevelList());
#


my $callback = sub {
    my ($callone, $calltwo) = @_;
    print "callback!" . Dumper([$callone,$calltwo]) . "\n";
};

my @names = ("one","two","one","two");
my @ints = ([0,10],[20,30],[50,100],[60,70]);
 
my $ps = PairStreamer->new($callback);
for( my $i=0; $i<scalar(@names); $i++ ){
    $ps->accept($names[$i],$ints[$i]);
}
