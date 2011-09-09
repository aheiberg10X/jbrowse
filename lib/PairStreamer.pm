package PairStreamer;

use strict;
use warnings;
use Data::Dumper;

sub new {
    #callback : when a pair is completed it is released to callback()
    my ($class,$callback) = @_;

    #working_set :  the pairs that need to be kept in memory
    #               4 things in each row
    #               ix || order | left_read | right_read | fill
    #top_ix : the index of working_set that holds the left-most
    #         read
    #free_ixs : a stack open working_set indexes
    #size : records the size of working set
    #dorix : maps priority to index
    #dnamix : maps pair-name to index

    my $self = { "working_set" => [], 
                 "top_ix" => 0,
                 "free_ixs" => [],
                 "size" => 0,
                 "dorix" => {},
                 "highest_order" => -1,
                 "dnamix" => {},
                 "callback" => $callback};
    open STATS, '>', 'stats.txt';
    bless $self, $class;
    return $self;
}

sub acceptFullPair {
    my ($self,$lpair,$rpair) = @_;
    $self->{callback}->($lpair,$rpair); 
}

sub acceptHalfPair {
    my ($self, $pair_name, $half_feat) = @_;
    my $row;
    print STATS "\n$pair_name [ $half_feat->[0], $half_feat->[1] ]\n";
    if( defined $self->{dnamix}->{$pair_name} ){    
        my $rowix = $self->{dnamix}->{$pair_name};
        $row = $self->{working_set}->[$rowix];
        $row->[2] = $half_feat;
        $row->[3] = 1;
        while( $rowix == $self->{top_ix} && $row->[3] ){   
            print STATS "done\n"; 
            $self->{callback}->($row->[1], $row->[2]);
            push( @{$self->{free_ixs}}, $rowix );

            $rowix = $self->{dorix}->{($row->[0])+1};
            last if not defined $rowix;
            $self->{top_ix} = $rowix;
            $row = $self->{working_set}->[$rowix];
        }
    }
    else{
        $row = [];
        $row->[0] = ++$self->{highest_order};
        $row->[1] = $half_feat;
        $row->[3] = 0;
        
        my $newix = pop @{$self->{free_ixs}};
        if( ! defined $newix ){
            $newix = $self->{size}++;
        }
        $self->{working_set}->[$newix] = $row;
        $self->{dorix}->{$row->[0]} = $newix;
        $self->{dnamix}->{$pair_name} = $newix; 
    }

    my $elems = scalar(@{$self->{working_set}}) - scalar(@{$self->{free_ixs}});
    print STATS "Elems: $elems\n";
    print STATS "Tot: $self->{highest_order}\n";
}

1;

