///////////////////////////////////////////////////
// IANode
function IANode( trackKey, site ){
    this.trackKey = trackKey;
    this.site = site;
    this.next = null;
    this.prev = null
    this.nextType = null;
}   

IANode.prototype.setNext = function( ianode ){
    this.next = ianode;
};
IANode.prototype.setNextType = function( ianode ){
    this.nextType = ianode;
};

IANode.prototype.belongsTo = function( trackKey ){
    return this.trackKey == trackKey;
}; 

IANode.prototype.disconnect = function( ianode ){
    ianode.prev.n
    r.next = null;
    r.prev = null;
    var n = r.nextType;
    r.nextType = null;
    r = n;
};

// IANode
// ////////////////////////////////////////////////


///////////////////////////////////////////////////
//  Interesting Areas
function InterestingAreas( refstart, refend, gap_thresh ){
    var sysTrackName = "systrack";
    var start = new IANode( sysTrackName, refstart );
    var end = new IANode( sysTrackName, refend );
    start.next = end;
    end.prev = start;
    start.nextType = end;
    this.firstNode = start;
    this.nextRight = end;
    this.nextLeft = start;
    this.activeTracks = {sysTrackName: start};
    this.inactiveTracks = {};
    this.gap_thresh = gap_thresh;

    //this.addTrackTest();

}


//interestingAreas should be sorted asc
InterestingAreas.prototype.addTrack = function( trackKey, interestingAreas ){
    if( interestingAreas == null ){
        return;
    }
    //create an IANode for each thing
    //point nextType of each to its successor
    interestingAreas.reverse();
    var prevNode = null;
    var head = null;
    //console.log(interestingAreas);
    for(ix in interestingAreas){
        //console.log(interestingAreas[ix]);
        head = new IANode( trackKey, interestingAreas[ix] );
        head.nextType = prevNode;
        prevNode = head;
    }
    this.activeTracks[trackKey] = head; 

    //integrate into big list
    var p1 = this.firstNode;
    var p2 = p1.next;
    while( head != null ){
        //console.log(p1,p2,head);
        if(  p1.site+this.gap_thresh <= head.site && head.site <= p2.site+this.gap_thresh ){
            //console.log("in between");
            head.next = p2;
            p2.prev = head;

            //console.log(head);
            p1.next = head;
            head.prev = p1;

            //console.log(p1);
            p1 = head;   
            head = head.nextType;
        }
        else{
            //console.log("outside");
            p1 = p2;
            if( p2.next != null ){
                p2 = p2.next;
            }
            else{
                break;
            }
        }
    }
    this.updateViewFrame    
    
};

InterestingAreas.prototype.removeTrack = function( trackKey ){
    var trackList = (trackKey in this.activeTracks) ? this.activeTracks : this.inactiveTracks;
    var r = trackList[trackKey];
    if( r == null ){ return; }
    var lastNonType = r.prev;
    while( r != null ){
        if( ! r.prev.belongsTo(trackKey) && lastNonType != r.prev ){            
            lastNonType.next = r.prev;
            r.prev.prev = lastNonType;
            lastNonType = r.prev;
        }
        if( r.nextType == null ){   //have to handle the last node separately
            lastNonType.next = r.next;
            r.next.prev = lastNonType;
        }
        r = r.nextType;
    }
    delete trackList[trackKey];
    return;
};

//can generalize these guys with a some kind of toggle class?
InterestingAreas.prototype.makeActive = function( trackKey ){
    this.activeTracks[trackKey] = this.inactiveTracks[trackKey];
    delete this.inactiveTracks[trackKey];
};

InterestingAreas.prototype.makeInactive = function( trackKey ){
    this.inactiveTracks[trackKey] = this.inactiveTracks[trackKey];
    delete this.activeTracks[trackKey];
};

InterestingAreas.prototype.isActive = function( ianode ){
    return (ianode.trackKey in this.activeTracks);
};
//private, used by updateViewFrame
InterestingAreas.prototype.getNextValidRight = function(n) {
    console.log( "starting getNextValidRight" );
    do{
        if( n.next == null ){ break; }
        n = n.next;
    }        
    while( ! this.isActive(n) );
    console.log( "ending getNextValidRight" );
    return n;
};

//private, used by updateViewFrame
InterestingAreas.prototype.getNextValidLeft = function(n) {
    do{
        if( n.prev == null ){ break; }
        n = n.prev;
    }
    while( !this.isActive(n) );

    return n;
};

InterestingAreas.prototype.updateViewFrame = function( left, right ){
    console.log("starting updateViewFrame");
    //fiind the first node with site >= left
    var n = this.nextRight;
    if( n.site < right ){
        console.log("n.site < right");
        while( n.site < right && !(n.trackKey == "systrack" && n.site > 0)){
            n = this.getNextValidRight(n);
        }
        this.nextRight = n;
    }
    else if( n.site > right ){
        console.log("n.site > right");
        var last = n;
        while( n.site > right ){
            last = n;
            n = this.getNextValidLeft(n);
        }
        if( this.isActive(last) ){
            this.nextRight = last;
        }
        else{
            this.nextRight = this.getNextValidRight(n);
        }
    }

    var p = this.nextRight.prev;

    console.log("Current p", p);
    while( left <= p.site && p.site <= right ){
        if( p.prev == null ){ break; }
        p = p.prev;
        //console.log("prev p:", p);
    }
    this.nextLeft = p;
    
    //console.log("New next right/left");
    //console.log( this.nextRight );
    //console.log( this.nextLeft );
    console.log( "ending updateViewFrame" );
    
    return;
};


InterestingAreas.prototype.getNextRightSite = function(){
    return this.nextRight.site;
};

InterestingAreas.prototype.getNextLeftSite = function(){
    return this.nextLeft.site;
};

InterestingAreas.prototype.getNextOfType = function( trackKey ){
    return;
};

////////////// Testing and Debugging //////////////////////
InterestingAreas.prototype.logNodes = function(){
    var n = this.firstNode;
    while( n != null ){
        console.log( n );
        n = n.next;
    }
};

InterestingAreas.prototype.logNodesAround = function( node, windowSize ){

    var n = node;
    for( i=1; i<=windowSize; i++){
        if( n.prev == null){ break; }
        n = n.prev;
    }
    for( i=1; i<= windowSize*2; i++ ){
        
        console.log(n);
        if( n.next == null ){break;}
        n = n.next;
    }
};


InterestingAreas.prototype.addTrackTest = function(){
    trackKey = "test_track1";
    //this is coming from evidence.or.missFR.5
    ias1 = [1000000,3000000,5000000];
    ias2 = [2000000,4000000,6000000];
    this.addTrack( "track1", ias1 );
    this.addTrack( "track2", ias2 );
    this.removeTrack( "track2");
    //do some console logging to test this out
    //
    this.logNodes();
    //this.next = ias[0];
};

//  Interesting Areas
///////////////////////////////////////////////////


