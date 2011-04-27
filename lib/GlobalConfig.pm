package GlobalConfig;

use strict;
use warnings;

use Exporter 'import';

our @EXPORT = qw($root_dir $data_dir $upload_dir $HISTOGRAMMING);

our $root_dir = "/home/andrew/school/dnavis/jbrowse";
our $data_dir = $root_dir . "/data";
our $upload_dir = $root_dir . "/uploads";
our $HISTOGRAMMING = 0;

1;
