package GlobalConfig;

use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw($ROOT_DIR $DATA_DIR $DEBUG_DIR $UPLOAD_DIR $DEBUG $INTERESTING_AREAS_GAP_THRESH $CHROM_PREFIX $DONOR_PREFIX $PRIVATE_PREFIX $QUERY_PREFIX);
use JSON 2;

use Cwd;


open(GC, "<", "/home/andrew/school/dnavis/jbrowse/lib/GlobalConfig.js") or die $!;
local $/=undef;
my $json_text = <GC>;
    
#delete whitespace
$json_text =~ s/\s+/ /g;
#delete what's leading up to the start of the object (i.e ' histogram = ')
$json_text =~ s/^.+= //;
    
close GC;

#open(DEBUGFILE,">","debug_globals.txt");
#print DEBUGFILE "asdfsdfasdf\n";

my $globals = JSON::decode_json($json_text);

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
#our $FAKING_REFSEQ = $globals->{FAKING_REFSEQ};

#print DEBUGFILE "$globals\n";
#print DEBUGFILE "rootdir: $ROOT_DIR\n";
#print DEBUGFILE "uploaddir: $UPLOAD_DIR\n";
#close(DEBUGFILE);

1;
