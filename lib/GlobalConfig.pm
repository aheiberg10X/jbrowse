package GlobalConfig;

use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw($ROOT_DIR $DATA_DIR $DEBUG_DIR $UPLOAD_DIR $DEBUG $INTERESTING_AREAS_GAP_THRESH $CHROM_PREFIX $DONOR_PREFIX $PRIVATE_PREFIX $QUERY_PREFIX $TRACK_TEMPLATE $UNBOUND_CHROM $PROJECT_PREFIX);
use JSON 2;

use Cwd;

my @tries = ("GlobalConfig.js", "../lib/GlobalConfig.js","lib/GlobalConfig.js");
my $good = 0;
my $json_text;
foreach my $try (@tries) {
    if( -e $try ){
        open(GC, "<", $try) or die $!;
	$good = 1;
        local $/=undef;
        $json_text = <GC>;
        close GC;
        last;
    }
}

if( ! $good ){ die "cannot find GlobalConfig.js for GlobalConfig.pl"; }
    
#delete whitespace
$json_text =~ s/\s+/ /g;
#delete what's leading up to the start of the object (i.e ' histogram = ')
$json_text =~ s/^.+= //;
    
close GC;

#open(DEBUGFILE,">","debug_globals.txt");
#print DEBUGFILE "asdfsdfasdf\n";

my $globals = JSON::decode_json($json_text);

our $PROJECT_PREFIX = $globals->{PROJECT_PREFIX};
our $CHROM_PREFIX = $globals->{CHROM_PREFIX};
our $DONOR_PREFIX = $globals->{DONOR_PREFIX};
our $PRIVATE_PREFIX = $globals->{PRIVATE_PREFIX};
our $QUERY_PREFIX = $globals->{QUERY_PREFIX};

our $ROOT_DIR = $globals->{ROOT_DIR};
our $DATA_DIR = $ROOT_DIR . $globals->{DATA_DIR};
our $UPLOAD_DIR = $ROOT_DIR . $globals->{UPLOAD_DIR};
our $DEBUG_DIR = $ROOT_DIR . $globals->{DEBUG_DIR};
our $DEBUG = $globals->{DEBUG};
our $INTERESTING_AREAS_GAP_THRESH = $globals->{INTERESTING_AREAS_GAP_THRESH};
our $TRACK_TEMPLATE = sprintf($globals->{TRACK_TEMPLATE},$DONOR_PREFIX,$QUERY_PREFIX,$CHROM_PREFIX);
our $UNBOUND_CHROM = $globals->{UNBOUND_CHROM};

#print DEBUGFILE "$globals\n";
#print DEBUGFILE "rootdir: $ROOT_DIR\n";
#print DEBUGFILE "uploaddir: $UPLOAD_DIR\n";
#close(DEBUGFILE);

1;
