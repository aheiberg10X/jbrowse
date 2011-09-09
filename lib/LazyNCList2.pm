package LazyNCList2;

use strict;
use warnings;
use Data::Dumper;

sub new {
    #print "\nInside new\n";
    my ($class, $start, $end, $sublistIndex, $lazyIndex,
        $measure, $output, $sizeThresh) = @_;
    my $self = {'topList' => [],
                'measure' => $measure,
                'output' => $output,
                'size' => 0, 
                'sizeThresh' => $sizeThresh,
                'NCL' => NCList->new($start,$end,$sublistIndex),
                'id' => 0};

    bless $self, $class;
    return $self;
}

#TODO OPTIMIZE
# the measure function of counting characters takes too long
# replace with a simple counter, or counter * max_feature_length
sub addSorted {
    #print "\nIn addSorted\n";
    my ($self, $feat) = @_;
    if ($self->{size} >= $self->{sizeThresh}) {
        $self->finish();        
    }
    $self->{NCL}->addFeatures([$feat]);
    $self->{size} += 60; # ~ 7*6 pos numbers + 3 strands + 8 []
    #$self->{size} += $self->{measure}->($feat);
}

sub finish {
    #print "\nIn finish\n";
    my $self = shift;
    if( $self->{size} > 0 ){
        my $id = $self->{id};
        #write the chunk out
        $self->{output}->($self->{NCL}->nestedList, $id);

        my $NCL = $self->{NCL};

        #update topList with minStart and maxEnd, chunk
        my ($minStart,$maxEnd) = ($NCL->{minStart}, $NCL->{maxEnd});
        my $fakeFeat = [$minStart, $maxEnd, {"chunk",$id}];  #lazy index is de facto 2
        push( @{$self->{topList}}, $fakeFeat );
        #print Dumper($self->{topList});
        #reset 
        $self->{size} = 0;
        #or call flush() on the existing NCL?
        $self->{NCL} = NCList->new($NCL->{startIndex},
                                    $NCL->{endIndex},
                                    $NCL->{sublistIndex});
        $self->{id} += 1;
        
    }
}

sub topLevelList {
    return shift->{topList};
}

1;
