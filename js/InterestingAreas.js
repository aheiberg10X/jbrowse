///////////////////////////////////////////////////
// IANode
function IANode( trackKey, site ){
    this.belongsTo = trackKey;
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

// IANode
// ////////////////////////////////////////////////


///////////////////////////////////////////////////
//  Interesting Areas
function InterestingAreas( refstart, refend ){
    var sysTrackName = "systrack";
    var start = new IANode( sysTrackName, refstart );
    var end = new IANode( sysTrackName, refend );
    start.next = end;
    end.prev = start;
    start.nextType = end;
    this.firstNode = start;
    this.nextRight = end;
    this.nextLeft = start;
    this.activeTracks = {sysTrackName:0};
    
    //testing
    //this.addTrackTest();

}

InterestingAreas.prototype.addTrackTest = function(){
    trackKey = "test_track1";
    //this is coming from evidence.or.missFR.5
    ias = [89247786,90617135,97561402];
    this.addTrack( trackKey, ias );
    //do some console logging to test this out
    //
    this.next = ias[0];
};

//interestingAreas should be sorted asc
InterestingAreas.prototype.addTrack = function( trackKey, interestingAreas ){
    if( interestingAreas == null ){
        return;
    }
    this.activeTracks[trackKey] = 1;
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
    //integrate into big list
    var p1 = this.firstNode;
    var p2 = p1.next;
    while( head != null ){
        //console.log(p1,p2,head);
        if(  p1.site <= head.site && head.site <= p2.site ){
            //console.log("in between");
            head.next = p2;
            p2.prev = head;

            //console.log(head);
            p1.next = head;
            head.prev = p1;

            //console.log(p1);
            p1 = head;   
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
        head = head.nextType;
    }
    this.updateViewFrame    
    
};

InterestingAreas.prototype.removeTrack = function( trackKey ){
    return;
};

InterestingAreas.prototype.isValid = function( ianode ){
    return (this.activeTracks[ianode.belongsTo] == 1);
};

InterestingAreas.prototype.getNextValidRight = function(n) {
    do{
        if( n.next == null ){ break; }
        n = n.next;
    }        
    while( ! this.isValid(n) );

    return n;
};

InterestingAreas.prototype.getNextValidLeft = function(n) {
    do{
        if( n.prev == null ){ return n; }
        n = n.prev;
    }
    while( !this.isValid(n) );

    return n;
};

InterestingAreas.prototype.updateViewFrame = function( left, right ){
    //fiind the first node with site >= left
    var n = this.nextRight;
    if( n.site < right ){
        while( n.site < right ){
            n = this.getNextValidRight(n);
        }
        this.nextRight = n;
    }
    else if( n.site > right ){
        var last = n;
        while( n.site > right ){
            last = n;
            n = this.getNextValidLeft(n);
        }
        this.nextRight = last;
    }

    var p = this.nextRight.prev;
    console.log("Current p", p);
    while( left <= p.site && p.site <= right ){
        if( p.prev == null ){ break; }
        p = p.prev;
        console.log("prev p:", p);
    }
    this.nextLeft = p;
    
    console.log("New next right/left");
    console.log( this.nextRight );
    console.log( this.nextLeft );
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

//  Interesting Areas
///////////////////////////////////////////////////


