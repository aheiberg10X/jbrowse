package GlobalConfig;

use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw($root_dir $data_dir $upload_dir $DEBUG $BOOKMARK_THRESH );
use JSON 2;

open(GC, "<", "../lib/GlobalConfig.js") or die $!;
local $/=undef;
my $json_text = <GC>;
    
#delete whitespace
$json_text =~ s/\s+/ /g;
#delete what's leading up to the start of the object (i.e ' histogram = ')
$json_text =~ s/^.+= //;
    
close GC;

open(DEBUG,">","debug_globals.txt");
print DEBUG "asdfsdfasdf\n";
my $globals = JSON::decode_json($json_text);


our $root_dir = $globals->{root_dir};
our $data_dir = $root_dir . $globals->{data_dir};
our $upload_dir = $root_dir . $globals->{upload_dir};
our $DEBUG = $globals->{DEBUG};
our $BOOKMARK_THRESH = $globals->{BOOKMARK_THRESH};

print DEBUG "$globals\n";
print DEBUG "rootdir: $root_dir\n";
print DEBUG "uploaddir: $upload_dir\n";
close(DEBUG);

#our $root_dir = "/home/andrew/school/dnavis/jbrowse";
#our $data_dir = $root_dir . "/data";
#our $upload_dir = $root_dir . "/uploads";
#our $DEBUG = 1;
#our $BOOKMARK_THRESH = 5000;

1;
