///////////////////////////////////////////////////
// IANode
function IANode( trackKey, locs ){
    this.trackKey = trackKey;

    this.leftLoc = parseInt(locs[0]);
    this.rightLoc = parseInt(locs[1]);
    //this.leftLoc = locs;

    this.next = null;
    this.prev = null
    this.nextType = null;
}   

IANode.prototype.belongsTo = function( trackKey ){
    return this.trackKey == trackKey;
}; 

//is this enough to get javascript to garbage collect?
//we worry about meta pointers nextRightNode, insideRightNode, and nextLeftNode in removeTrack
IANode.prototype.disconnect = function(){
    this.next = null;
    this.prev = null;
    this.nextType = null;
};

// IANode
// ////////////////////////////////////////////////


///////////////////////////////////////////////////
//  Interesting Area
//  A doubly-linked list of IANodes, arranged from left to right by position on the genome (no overlap by definition)
//  The Browser.js will call getNextRight and getNextLeftSite to get a position on the chromosome to pan to
//  This position is made available by mainting nextLeftNode, nextRightNode, and insideRightNode
//  These are pointers to IANodes in the list, and are updated by updateViewFrame (called by Browser.js onCoarseMove() )
//  A two IANodes called start and stop bookend the linked list
function InterestingAreas( refstart, refend ){
    var sysTrackName = "systrack";
    this.start = new IANode( sysTrackName, [refstart,refstart] );
    this.end = new IANode( sysTrackName, [refend,refend] );
    this.start.next = this.end;
    this.end.prev = this.start;
    this.start.nextType = this.end;

    //this.currentNode = start;    
    this.insideRightNode = this.end;
    this.nextLeftNode = this.start;
    this.nextRightNode = this.end;
    this.activeTracks = {sysTrackName: this.start};
    this.inactiveTracks = {};
    this.refstart = refstart;
    this.refend = refend;
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
    var p1 = this.start;
    var p2 = p1.next;
    while( head != null ){
        //console.log(p1,p2,head);
        console.log(p1.leftLoc);
        console.log(head.leftLoc);
        console.log(p2.leftLoc);
        var a = p1.leftLoc <= head.leftLoc;
        var b = head.leftLoc <= p2.leftLoc;
        console.log( a );
        console.log( b );
        if(  a && b ){
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
    var shouldbedone = 1;
    //this.updateViewFrame();
    
};

InterestingAreas.prototype.disconnect = function( r, lastNonType ){
    if( r == this.insideRightNode ){
        this.insideRightNode = this.getNextValidRight(r);
    }
    if( r == this.nextRightNode ){
        this.nextRightNode = this.getNextValidRight(r);
    }
    if( r == this.nextLeftNode ){
        this.nextLeftNode = lastNonType;
    }
    r.disconnect();
};

//Pretty sure this is not even close
InterestingAreas.prototype.removeTrack = function( trackkey ){
    this.makeInactive( trackkey );
    //var trackList = (trackkey in this.activeTracks) ? this.activeTracks : this.inactiveTracks;
    var r = this.inactiveTracks[trackkey];
    if( r == null ){ return; }
    while( r != null ){
        var lastNonType = r.prev;
        var n;
        while(r.next == r.nextType){
            n = r.next
            this.disconnect(r, lastNonType);            
            r = n;
        }
        lastNonType.next = r.next;
        r.next.prev = lastNonType;
        n = r.nextType;
        this.disconnect(r, lastNonType);
        r = n;
    }
    var shouldbedone = 1;
};


/////////////////////////////////////////////////////////////
 /*   while( r != null ){
        if( ! r.prev.belongsTo(trackkey) && lastNonType != r.prev ){            
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
    delete trackList[trackkey];
    return;
};*/

//can generalize these guys with a some kind of toggle class?
InterestingAreas.prototype.makeActive = function( trackKey ){
    this.activeTracks[trackKey] = this.inactiveTracks[trackKey];
    delete this.inactiveTracks[trackKey];
};

InterestingAreas.prototype.makeInactive = function( trackKey ){
    this.inactiveTracks[trackKey] = this.activeTracks[trackKey];
    delete this.activeTracks[trackKey];
};

InterestingAreas.prototype.isActive = function( ianode ){
    return (ianode.trackKey in this.activeTracks);
};
//private, used by updateViewFrame
InterestingAreas.prototype.getNextValidRight = function(n) {
    //console.log( "starting getNextValidRight" );
    do{
        if( n.next == null ){ break; }
        n = n.next;
    }        
    while( ! this.isActive(n) );
    //console.log( "ending getNextValidRight" );
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
    //set nextRightNode
    var n = this.nextRightNode;
    if( n.leftLoc < right ){
        //console.log("n.leftLoc < right");
        while( n.leftLoc < right && !(n.trackKey == "systrack" && n.leftLoc > 0)){
            n = this.getNextValidRight(n);
        }
        this.nextRightNode = n;
        
    }
    else if( n.leftLoc > right ){
        //console.log("n.leftLoc > right");
        var last = n;
        while( n.leftLoc > right ){
            last = n;
            n = this.getNextValidLeft(n);
        }
        
        //n is in the left,right interval, 
        this.nextRightNode = this.getNextValidRight(n);
    }

    //set currentNode
    //this.currentNode = this.getNextValidLeft(n);
    this.insideRightNode = this.getNextValidLeft( this.nextRightNode );

    //if( !(left <= this.insideRightNode.leftLoc && this.insideRightNode.leftLoc <= right) ){
    //this.insideRightNode = null;
    //}

    //set nextLeftNode
    var p = this.nextRightNode.prev;
    while( left <= p.leftLoc && p.leftLoc <= right ){
        if( p.prev == null ){ break; }
        p = p.prev;
    }
    this.nextLeftNode = p;
    return;
};

//TODO
//update bam_to_json to generate intervals, not just the left loc


InterestingAreas.prototype.getNextRightSite = function( viewFrameLeft, viewFrameRight){
    var insideRightNodeRunsOffRight = this.insideRightNode.leftLoc <= viewFrameRight && this.insideRightNode.rightLoc > viewFrameRight;
    if( this.nextRightNode.trackKey == "systrack" ){
        return -1; 
    }
    else if( !insideRightNodeRunsOffRight ){
        return this.nextRightNode.leftLoc;
    }
    else{
        return viewFrameRight + (viewFrameRight-viewFrameLeft)/2;
    }
};

InterestingAreas.prototype.getNextLeftSite = function( viewFrameLeft, viewFrameRight ){
    var nextLeftNodeRunsOffLeft = this.nextLeftNode.leftLoc < viewFrameLeft && this.nextLeftNode.rightLoc >= viewFrameLeft;    

    if( this.nextLeftNode.trackKey == "systrack" ){
        return -1;
    }
    else if( !nextLeftNodeRunsOffLeft ){
        return this.nextLeftNode.rightLoc;
    }
    else{
        return viewFrameLeft - (viewFrameRight-viewFrameLeft)/2;    
    }
};

InterestingAreas.prototype.getNextOfType = function( trackKey ){
    return;
};

////////////// Testing and Debugging //////////////////////
InterestingAreas.prototype.logNodes = function(){
    var n = this.start
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


