/**
 * Construct a new Browser object.
 * @class This class is the main interface between JBrowse and embedders
 * @constructor
 * @param params a dictionary with the following keys:<br>
 * <ul>
 * <li><code>containerID</code> - ID of the HTML element that contains the browser</li>
 * <li><code>refSeqs</code> - list of reference sequence information items (usually from refSeqs.js)</li>
 * <li><code>trackData</code> - list of track data items (usually from trackInfo.js)</li>
 * <li><code>dataRoot</code> - (optional) URL prefix for the data directory</li>
 * <li><code>browserRoot</code> - (optional) URL prefix for the browser code</li>
 * <li><code>tracks</code> - (optional) comma-delimited string containing initial list of tracks to view</li>
 * <li><code>location</code> - (optional) string describing the initial location</li>
 * <li><code>defaultTracks</code> - (optional) comma-delimited string containing initial list of tracks to view if there are no cookies and no "tracks" parameter</li>
 * <li><code>defaultLocation</code> - (optional) string describing the initial location if there are no cookies and no "location" parameter</li>
 * </ul>
 */

var Browser = function(params) {
    dojo.require("dojo.dnd.Source");
    dojo.require("dojo.dnd.Moveable");
    dojo.require("dojo.dnd.Mover");
    dojo.require("dojo.dnd.move");
    dojo.require("dijit.layout.ContentPane");
    dojo.require("dijit.layout.BorderContainer");

    //my stuff
    dojo.require("dojo.cache");
    dojo.require("dijit.form.Form");
    dojo.require("dijit.form.Button");
    dojo.require("dijit.form.TextBox");
    dojo.require("dojox.form.FileInput");
    dojo.require("dijit.form.CheckBox");
    dojo.require("dojo.io.iframe");
    dojo.require("dojox.layout.ExpandoPane");
    dojo.require("dijit.layout.AccordionContainer");
    dojo.require("dijit.form.VerticalSlider");
    dojo.require("dijit.Tree");
    dojo.require("dijit.tree.dndSource");
    dojo.require("dijit.tree.TreeStoreModel");
    dojo.require("dojox.data.FileStore");
    // end my stuff

    var refSeqs = params.refSeqs;
    var trackData = params.trackData;
    var globals = params.globals;
    this.deferredFunctions = [];
    this.dataRoot = params.dataRoot;
    var dataRoot;
    if ("dataRoot" in params)
        dataRoot = params.dataRoot;
    else
        dataRoot = "";

    this.names = new LazyTrie(dataRoot + "/names/lazy-",
			      dataRoot + "/names/root.json");
    this.tracks = [];
    for( track in trackData ){
        this.tracks.push(trackData[track]["key"]);
    }
    var brwsr = this;
    brwsr.isInitialized = false;
    dojo.addOnLoad(
        function() {
            //set up top nav/overview pane and main GenomeView pane
            dojo.addClass(document.body, "tundra");
            brwsr.container = dojo.byId(params.containerID);
            brwsr.container.genomeBrowser = brwsr;
            var topPane = document.createElement("div");
            brwsr.container.appendChild(topPane);

            var overview = document.createElement("div");
            overview.className = "overview";
            overview.id = "overview";
            topPane.appendChild(overview);
            //try to come up with a good estimate of how big the location box
            //actually has to be
            var maxBase = refSeqs.reduce(function(a,b) {return a.end > b.end ? a : b;}).end;
            var navbox = brwsr.createNavBox(topPane, (2 * (String(maxBase).length + (((String(maxBase).length / 3) | 0) / 2))) + 2, params);

            var viewElem = document.createElement("div");
            brwsr.container.appendChild(viewElem);
            viewElem.className = "dragWindow";

            var containerWidget = new dijit.layout.BorderContainer({
                liveSplitters: false,
                design: "headline",
                gutters: false
            }, brwsr.container);

            var brdrclr = "#929292";
            var contentWidget = new dijit.layout.ContentPane({
                region: "top", 
                layoutPriority: "1",
                style: "border-left: solid "+brdrclr
            }, topPane);

            var browserWidget = new dijit.layout.ContentPane({
                region: "center",
                style: "border-left: solid "+brdrclr 
            }, viewElem);

            //for depth slider
            var sliderPane = document.createElement("div");
            brwsr.container.appendChild( sliderPane );
            var sliderPaneWidget = new dijit.layout.ContentPane(
                                        {region: "right",
                                         style: "width: 20px; padding-top: 15px;",
                                         layoutPriority: "2"}, sliderPane);

            var sliderDiv = document.createElement("div");
            sliderPane.appendChild( sliderDiv );
            brwsr.maxRender = 50;            
            absMaxRender = 300;
            //remember: want to have slider at the top mean render 0
            var slider = new dijit.form.VerticalSlider(
                            {name: "vertical",
                             value: absMaxRender - brwsr.maxRender,
                             minimum: 0,
                             maximum: absMaxRender,
                             intermediateChanges: false,
                             style: "height: 100%;",
                             onChange: function(value){ 
                                            brwsr.maxRender = parseInt(value);
                                            dojo.forEach( brwsr.view.tracks, 
                                                          function(track){ 
                                                              track.setMaxRender( absMaxRender-brwsr.maxRender );
                                                              track.clear(); 
                                                          } );
                                            brwsr.view.showVisibleBlocks(true);
                                        }
                            },
                            sliderDiv);

            //create location trapezoid
            brwsr.locationTrap = document.createElement("div");
            brwsr.locationTrap.className = "locationTrap";
            topPane.appendChild(brwsr.locationTrap);
            topPane.style.overflow="hidden";

            //set up ref seqs
            brwsr.allRefs = {};
            for (var i = 0; i < refSeqs.length; i++)
                brwsr.allRefs[refSeqs[i].name] = refSeqs[i];

            var refCookie = dojo.cookie(params.containerID + "-refseq");
            brwsr.refSeq = refSeqs[0];
            for (var i = 0; i < refSeqs.length; i++) {
                brwsr.chromList.options[i] = new Option(refSeqs[i].name,
                                                        refSeqs[i].name);
                if (refSeqs[i].name.toUpperCase() == String(refCookie).toUpperCase()) {
                    brwsr.refSeq = brwsr.allRefs[refSeqs[i].name];
                    brwsr.chromList.selectedIndex = i;
                }
            }

            dojo.connect(brwsr.chromList, "onchange", function(event) {
                    var oldLocMap = dojo.fromJson(dojo.cookie(brwsr.container.id + "-location")) || {};
                    var newRef = brwsr.allRefs[brwsr.chromList.options[brwsr.chromList.selectedIndex].value];

                    if (oldLocMap[newRef.name])
                        brwsr.navigateTo(newRef.name + ":"
                                         + oldLocMap[newRef.name]);
                    else
                        brwsr.navigateTo(newRef.name + ":"
                                         + (((newRef.start + newRef.end) * 0.4) | 0)
                                         + " .. "
                                         + (((newRef.start + newRef.end) * 0.6) | 0));
                        });

            //hook up GenomeView
            var gv = new GenomeView(viewElem, 250, brwsr.refSeq, 1/200);
            brwsr.view = gv;
            brwsr.viewElem = viewElem;
            //gv.setY(0);
            viewElem.view = gv;

            //hook up InterestingAreas
            console.log( "setting up end to be: " + brwsr.refSeq.end+1 );
            brwsr.interestingAreas = new InterestingAreas( brwsr.refSeq.start, brwsr.refSeq.end+1 );

            dojo.connect(browserWidget, "resize", function() {
                    gv.sizeInit();

                    brwsr.view.locationTrapHeight = dojo.marginBox(navbox).h;
                    gv.showVisibleBlocks();
                    gv.showFine();
                    gv.showCoarse();
                });
            brwsr.view.locationTrapHeight = dojo.marginBox(navbox).h;

            dojo.connect(gv, "onFineMove", brwsr, "onFineMove");
            dojo.connect(gv, "onCoarseMove", brwsr, "onCoarseMove");

            //set up track list
            //var trackListDiv = brwsr.createTrackList(brwsr.container, params);
            brwsr.createTrackList2( brwsr, containerWidget, params );

            containerWidget.startup();

            brwsr.isInitialized = true;

            //set initial location
            var oldLocMap = dojo.fromJson(dojo.cookie(brwsr.container.id + "-location")) || {};

            if (params.location) {
                brwsr.navigateTo(params.location);
            } else if (oldLocMap[brwsr.refSeq.name]) {
                brwsr.navigateTo(brwsr.refSeq.name
                                 + ":"
                                 + oldLocMap[brwsr.refSeq.name]);
            } else if (params.defaultLocation){
                brwsr.navigateTo(params.defaultLocation);
            } else {
                brwsr.navigateTo(brwsr.refSeq.name
                                 + ":"
                                 + ((((brwsr.refSeq.start + brwsr.refSeq.end)
                                      * 0.4) | 0)
                                    + " .. "
                                    + (((brwsr.refSeq.start + brwsr.refSeq.end)
                                        * 0.6) | 0)));
            }

            //if someone call/cs methods on this browser object
            //before it's fully initialized, then we defer
            //those functions until now
            for (var i = 0; i < brwsr.deferredFunctions.length; i++)
                brwsr.deferredFunctions[i]();
            brwsr.deferredFunctions = [];
        });
};

Browser.prototype.createTrackList2 = function(brwsr, parent, params) {
    
    //var ep = new dojox.layout.ExpandoPane(
    var ep = new dijit.layout.AccordionContainer(
        {id: "accordion",  //7 
	     title: "accordion",
         region: "left",
         style: "width: 15%; background-color:#0000FF; border-style: none solid none none; border-color: #929292",
         splitter: "false"
        }).placeAt(parent);

    var form_pane =  new dijit.layout.ContentPane( //new dojox.layout.ExpandoPane(
        {id: "form_pane", //8
         title: "Manage",
         //region: "bottom",
         style: "background-color:#efefef;",
         //splitter: "true"
        }).placeAt(ep);


   //var track_pane = new dijit.layout.ContentPane(
   var track_pane = new dijit.layout.BorderContainer(
        {id:"track_pane", //9
         title: "View",
         //region: "top",
         style: "background-color: #efefef; border-style: none solid none none; border-color: #929292",
         //splitter: "true",
         selected: "true"
        }).placeAt(ep);

   var sandbox_pane = new dijit.layout.ContentPane(
           {id:"sandbox_pane",
            title: "Sandbox",
            style: "background-color: #efefef; border-style: none solid none none; border-color: #929292"
           }).placeAt(ep);

    var deleteSubmit = function(brwsr) {

        //brwsr.chromList.options[brwsr.chromList.selectedIndex].value;
        var current_chrom = brwsr.refSeq.name;
        var tracks_in_trash = [];
        var ids_to_trash = [];
        brwsr.trash_drop.forInItems(function(obj, id, map) {
            tracks_in_trash.push(obj.data.key);
            ids_to_trash.push(id);

            //current tracklist bookkeeping
            var ix = brwsr.tracks.indexOf(obj.data.key);
            if( ix != -1 ){
                brwsr.tracks.splice(ix,1);
            }
        });
        
        var args = {chrom: current_chrom,
                    delete_track: tracks_in_trash};

        var url = "../bin/remove_track.py?" + dojo.objectToQuery(args);

        var xhrArgs = {
            url: url,
            form: dojo.byId("track_manager_form"),
            handleAs: "text",
            load: function(data,ioargs) {
                //remove track object
                brwsr.trash_drop.selectAll();
                brwsr.trash_drop.deleteSelectedNodes();
                dojo.byId("track_manager_status").innerHTML = "Message posted.";
            },
            error: function(error) {
                dojo.byId("track_manager_status").innerHTML = "fail";
            }
        }
        //Call the asynchronous xhrPost
        var deferred = dojo.xhrPost(xhrArgs);
        dojo.byId("track_manager_status").innerHTML = "posted"
    };

    var trackkeyFromFilename = function( path ){
        //handily it will always appear as C:\fakepath\<filename>
        var splt = path.split('\\');
        var filename = splt[splt.length-1].split('.');
        var name = filename.slice(0, filename.length-1).join('.');
        return name;
    }

    //0 : OK
    //1 : malformed name
    //2 : duplicate name
    //3 : cannot find histogram file
    var hasNameConflict = function(name) {
        if( name == '' ){ 
            alert("Filename is empty");
            return 1; 
        }
        else{
            for( trackkey in brwsr.tracks ){
                if( name == brwsr.tracks[trackkey] ){
                    alert("There is already a track with that name");
                    return 2;
                 }
            }
        }
        return 0;
        //compared to ?? (other tracks)
    }

    var uploadBAM = function() {
        var display_linking = dojo.byId("display_linking").value;
        //TODO: should pass 'name' to form, rather than deducing the trackname anew in each language
        var name = trackkeyFromFilename( dojo.byId("bam_filename").value );
        var isConflict = hasNameConflict( name );
        if( !isConflict ){
            dojo.io.iframe.send({
                url: "bin/bam_to_json_paired_cgi.pl",
                method: "post",
                handleAs: "json",
                form: dojo.byId("track_manager_form"),
                load: function(data,ioArgs) {
                    if( data['status'] == "ERROR" ) {
                        alert(data['message']);
                    }
                    else{
                        brwsr.trackListWidget.insertNodes(false,data['trackData']);
                        dojo.byId("track_manager_status").innerHTML = "bam posted";
                        brwsr.tracks.push(name);
                        ep.selectChild( "track_pane", true);
                    }
                },
                error: function(response, ioArgs){
                    alert(response);
                }

            });
        }
    };

    var uploadRegion = function(brwsr) {
        //TODO: should pass 'name' to form, rather than deducing the trackname anew in each language
        var name = trackkeyFromFilename( dojo.byId("region_filename").value );
        var isConflict = hasNameConflict(name);
        if( !isConflict ){
            alert( "not working right now due to chromosome switch" );
            //dojo.io.iframe.send({
            //url: "bin/region_to_json.pl",
            //method: "post",
            //handleAs: "json",
            //form: dojo.byId("track_manager_form"),
            //load: function(data) {
            //if( data['status'] == "ERROR" ) {
            //alert(data['message']);
            //}
            //else{
            //brwsr.trackListWidget.insertNodes(false, data['trackData']);                        
            //dojo.byId("track_manager_status").innerHTML = "region posted";
            //}
            //}
            //});
        }
    };

    
    //////////// start creating trackListDiv ///////////////////////////////////////
    var trackListDiv = document.createElement("div");
    trackListDiv.id = "tracksAvail";
    trackListDiv.className = "container handles";
    //trackListDiv.style.cssText =
    //  "width: 90%; height: 90%; overflow-x: hidden; overflow-y: auto;";
    trackListDiv.innerHTML =
        "Available Tracks:<br/>(Drag <img src=\""
        + (params.browserRoot ? params.browserRoot : "")
        + "img/right_arrow.png\"/> to view)<br/><br/>";
    trackListDiv.style.cssText = "background-color: #FF0000; ";

    var brwsr = this;
    
    var trackListCreate = function(track, hint) {
        var node = document.createElement("div");
        node.className = "tracklist-label";
        node.innerHTML = track.key;
        //in the list, wrap the list item in a container for
        //border drag-insertion-point monkeying
        if ("avatar" != hint) {  //dropped back into list
            var container = document.createElement("div");
            container.className = "tracklist-container";
            container.style.cssText = "width: 90%;";
            container.appendChild(node);
            node = container;
            brwsr.interestingAreas.removeTrack( track.key );
            brwsr.navigateTo(brwsr.locationBox.value);
        }
        node.id = dojo.dnd.getUniqueId();
        return {node: node, data: track, type: ["track"]};
    };

    this.trackListWidget = new dojo.dnd.Source(trackListDiv,
                                               {creator: trackListCreate,
						accept: ["track"],
						withHandles: false});
    
    var initCallback = function( trackKey, tracksInterestingAreas ) {
        brwsr.interestingAreas.addTrack( trackKey, tracksInterestingAreas );
        brwsr.navigateTo(brwsr.locationBox.value);
    };
    var changeCallback = function() {
        brwsr.view.showVisibleBlocks(true);
    };

    var trackCreate = function(track, hint) {
        var node;
        if ("avatar" == hint) {
            return trackListCreate(track, hint);
        } 
        else {
            var replaceData = {refseq: brwsr.refSeq.name};
            var url = track.url.replace(/\{([^}]+)\}/g, function(match, group) {return replaceData[group];});
            var klass = eval(track.type);
            var newTrack = new klass(track, url, brwsr.refSeq,
                                     {
                                         maxRender: brwsr.maxRender,
                                         //see FeatureTrack ctor for explanation
                                         initCallback: initCallback,
                                         //calls GenomeView showVisibleBlocks()
                                         changeCallback: changeCallback,
                                         trackPadding: brwsr.view.trackPadding,
                                         baseUrl: brwsr.dataRoot,
                                         charWidth: brwsr.view.charWidth,
                                         seqHeight: brwsr.view.seqHeight
                                     });
            node = brwsr.view.addTrack(newTrack);
            
        }
        return {node: node, data: track, type: ["track"]};
    };

    this.viewDndWidget = new dojo.dnd.Source(this.view.zoomContainer,
                                       {
                                           creator: trackCreate,
                                           accept: ["track"],
                                           withHandles: true
                                       });

    dojo.subscribe("/dnd/drop", function(source,nodes,iscopy){
                       brwsr.onVisibleTracksChanged();
                       //multi-select too confusing?
                       //brwsr.viewDndWidget.selectNone();
                   });

    this.trackListWidget.insertNodes(false, params.trackData);
    var oldTrackList = dojo.cookie(this.container.id + "-tracks");
    if (params.tracks) {
        this.showTracks(params.tracks);
    } else if (oldTrackList) {
        this.showTracks(oldTrackList);
    } else if (params.defaultTracks) {
        this.showTracks(params.defaultTracks);
    }

   
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //            Build Up trackListDiv
    /////////////////////////////////////////////////////////////////////////////////////////////////// 

    var tracklist_pane =  new dijit.layout.ContentPane( //new dojox.layout.ExpandoPane(
        {id:"tracklist_pane", 
         title: "Tracks",
         region: "top",
         style: "background-color:#efefef;",
         layoutPriority: "1"
         //splitter: "true"
        }, trackListDiv).placeAt(track_pane);

    var query_pane = new dijit.layout.ContentPane( 
        {id : "query_pane",
         title : "Query",
         region : "center",
         layoutPriority: "0",
         style : "background-color: #efefef"
        }).placeAt(track_pane);

    var trashcan_pane =  new dijit.layout.ContentPane( //new dojox.layout.ExpandoPane(
        {id: "trashcan_pane", 
         title: "Trash Can",
         region: "bottom",
         style: "background-color:#efefef;",
         layoutPriority: "2"
         //splitter: "true"
        }).placeAt(track_pane);


    var query_div = document.createElement("div");
    query_div.id = "query_div";
    query_pane.domNode.appendChild( query_div );
 
    var query_box = new dijit.form.TextBox(
                        {id : "query_box",
                         name: "query_box",
                         style: "height: 12em; width: 90%"}
                    ).placeAt( query_div );

    var query_bam = new dijit.form.TextBox(
                        {id: "query_bam",
                         name: "query_bam",
                         value: "/home/andrew/school/dnavis/jbrowse/genomequery/biosql_compiler/biosql/indexing/indexed/evidence.dist.1000.1M.5.sorted.bam",
                         type: "hidden"}
                    ).placeAt( query_div );
    
    var runQuery = function(){
        var xhrArgs = {
            url: "bin/sample.py",
            form: dojo.byId("query_form"),
            handleAs: "json",
            load: function(data,ioargs){
                if( data["status"] == "ok" ){
                    alert("all's well, but where is the generated BAM?");
                }
                else{
                    alert( data["message"] );
                }
            },
            error: function(error) {
                alert(error);
            }
        }
        //Call the asynchronous xhrPost
        var deferred = dojo.xhrPost(xhrArgs);

    };

    var query_button = new dijit.form.Button(
                            {id: "query_button", 
                             label: "Query Tracks",
                             style: "align-text: right;",
                             onClick: runQuery  }
                       ).placeAt( query_div );
 
    var query_form = new dijit.form.Form(
                         {id: "query_form",
                          encType : "multipart/form-data"},
                     query_div )

        
                        


    var trashcan_div = document.createElement("div");
    trashcan_div.id = "trashcan_div";
    trashcan_div.style.cssText = "background-color: #FF00FF; text-align: center;";
    trashcan_div.innerHTML = "trash drop";


    //tracklist_pane.domNode.appendChild( trackListDiv);
    trashcan_pane.domNode.appendChild( trashcan_div );

    this.trash_drop = new dojo.dnd.Source(trashcan_div, 
                                          {creator: trackListCreate,
                                           accept:["track"]/*, 
                                           withHandles:"false"*/
                                          });

    var delete_button = new dijit.form.Button({id: "delete_button", 
                                        label: "Delete Tracks",
                                        style: "align-text: right;",
                                        onClick: function(){ deleteSubmit(brwsr) }
                                       }).placeAt( trashcan_pane.domNode );


    /////////////////////////////////////////////////////////////////////////////////////////
    //           Build form 
    /////////////////////////////////////////////////////////////////////////////////////////

    var track_manager_form = document.createElement("form");
    track_manager_form.id = "track_manager_form";
    track_manager_form.method = "post";
    track_manager_form.enctype = "multipart/form-data";
    track_manager_form.innerHTML = '<h3>BAM</h3>' +
                                   '<div id="bam_controls">' + 
                                   '<p id="bamfile">BAM File</p>' + 
                                   '<p id="bamhistogram">Histogram Data (opt)</p>' +
                                   '<input type="checkbox" name="display_linking" id="display_linking" value="1" checked=true/>Display Links<br/>' +
                                   '<input type="hidden" name="host_chrom" id="host_chrom" value="'+brwsr.refSeq.name+'" />' +
                                   
                                   '</div>' +
                                   '<h3>Region</h3>'+
                                   '<div id="region_controls">' +
                                   '<p id="regionfile">File</p>' +
                                   '</div>';


    form_pane.domNode.appendChild( track_manager_form );

    var bam_input = document.createElement("input");
    bam_input.type = "file";
    bam_input.id = "bam_filename";
    bam_input.name = "bam_filename";
    bam_input.style.cssText = "border-top: 10px;";
    dojo.byId("bamfile").appendChild( bam_input );

    var bam_histogram = document.createElement("input");
    bam_histogram.type = "file";
    bam_histogram.name = "bam_histogram_filename";
    bam_histogram.id = "bam_histogram_filename";
    //bam_histogram.style.cssText = "visibility:hidden; position: absolute; top:0; left:0;";
    dojo.byId("bamhistogram").appendChild( bam_histogram );

    var bam_upload = new dijit.form.Button(
        {name: "bam_upload", 
         label: "Upload BAM", 
         style: "margin-top: 0px;",
         onClick: uploadBAM
        }).placeAt( dojo.byId("bam_controls") );

    var input_regionfile = document.createElement("input");
    input_regionfile.type = "file";
    input_regionfile.id = "region_filename";
    input_regionfile.name = "region_filename";
    //input_regionfile.style.cssText = "visibility: hidden; position: absolute; top:0; left:0;";
    dojo.byId("regionfile").appendChild( input_regionfile );

    var upload_regionfile = new dijit.form.Button(
            {id: "upload_regionfile", 
             label: "Upload Region", 
             style: "margin-top: -10px;",
             onClick: function() {uploadRegion(brwsr)}
            }).placeAt( dojo.byId("region_controls") );

    
    var track_manager_status = document.createElement("div");
    track_manager_status.id = "track_manager_status";
    track_manager_status.innerHTML = "status";
    form_pane.domNode.appendChild( track_manager_status );


    /////////////////
    //Sandbox

    var sandbox_div = document.createElement("div");
    sandbox_div.id = "sandbox_div";
    //sandbox_div.class = " claro ";
    sandbox_pane.domNode.appendChild( sandbox_div );

    var fs = new dojox.data.FileStore( 
                {"id" : "fs",
                 "url" : "bin/filestore_dojotree.py",
                 "pathAsQueryParam" : "true"}
             );
    
    var model = new dijit.tree.ForestStoreModel(
                     {"id" : "model",
                      "store" : fs,
                    "rootId" : "projects",
                    "rootLabel" : "projects" }
                    );       
    
    var tree = new dijit.Tree(
            {"id" : "tree",
             "model" : model,
             "dndController" : "dijit.tree.dndSource"}
        ).placeAt( sandbox_div ); 
    

            
};


/**
 * @private
 */
Browser.prototype.onFineMove = function(startbp, endbp) {
    var length = this.view.ref.end - this.view.ref.start;
    var trapLeft = Math.round((((startbp - this.view.ref.start) / length)
                               * this.view.overviewBox.w) + this.view.overviewBox.l);
    var trapRight = Math.round((((endbp - this.view.ref.start) / length)
                                * this.view.overviewBox.w) + this.view.overviewBox.l);
    var locationTrapStyle;
    if (dojo.isIE) {
        //IE apparently doesn't like borders thicker than 1024px
        locationTrapStyle =
            "top: " + this.view.overviewBox.t + "px;"
            + "height: " + this.view.overviewBox.h + "px;"
            + "left: " + trapLeft + "px;"
            + "width: " + (trapRight - trapLeft) + "px;"
            + "border-width: 0px";
    } else {
        locationTrapStyle =
            "top: " + this.view.overviewBox.t + "px;"
            + "height: " + this.view.overviewBox.h + "px;"
            + "left: " + this.view.overviewBox.l + "px;"
            + "width: " + (trapRight - trapLeft) + "px;"
            + "border-width: " + "0px "
            + (this.view.overviewBox.w - trapRight) + "px "
            + this.view.locationTrapHeight + "px " + trapLeft + "px;";
    }

    this.locationTrap.style.cssText = locationTrapStyle;
};

/**
 * @private
 */
/*Browser.prototype.createTrackList = function(parent, params) {*/
/*var leftPane = document.createElement("div");*/
/*leftPane.style.cssText="width: 10em";*/
/*parent.appendChild(leftPane);*/

/*//.///// this is me//////////////*/

/*var deleteSubmit = function() {*/
/*var xhrArgs = {*/
/*url: "$root_dir/bin/dostuff.pl",*/
/*//postData: {name: "om text"},*/
/*form: dojo.byId("track_manager_form"),*/
/*//content: {key: },*/
/*handleAs: "text",*/
/*load: function(data,ioargs) {*/
/*dojo.byId("track_manager_status").innerHTML = "Message posted.";*/
/*},*/
/*error: function(error) {*/
/*dojo.byId("track_manager_status").innerHTML = "fail";*/
/*}*/
/*}*/
/*//Call the asynchronous xhrPost*/
/*var deferred = dojo.xhrPost(xhrArgs);*/
/*dojo.byId("track_manager_status").innerHTML = "posted"*/
/*};*/


/*var uploadBAM = function() {*/

/*dojo.io.iframe.send({*/
/*url: "../bin/bam_to_json_paired_cgi.pl",*/
/*method: "post",*/
/*handleAs: "text",*/
/*form: dojo.byId("track_manager_form"),*/
/*load: function(data) {*/
/*//alert(data);*/
/*dojo.byId("track_manager_status").innerHTML = "bam posted"*/
/*}*/
/*});*/
/*};*/

/*var uploadRegion = function() {*/
/*dojo.io.iframe.send({*/
/*url: "../bin/region_to_json.pl",*/
/*method: "post",*/
/*handleAs: "text",*/
/*form: dojo.byId("track_manager_form"),*/
/*load: function(data) {*/
/*//alert(data);*/
/*dojo.byId("track_manager_status").innerHTML = "region posted"*/
/*}*/
/*});*/
/*};*/

/*var track_manager_div = document.createElement("div");*/
/*track_manager_div.id = "track_manager_div";*/
/*track_manager_div.style.cssText = "padding: 10px; border-bottom:thick solid #000000;";*/

/*var track_manager_status = document.createElement("div");*/
/*track_manager_status.id = "track_manager_status";*/
/*track_manager_status.innerHTML = "status";*/

/*var track_manager_form = document.createElement("form");*/
/*track_manager_form.id = "track_manager_form";*/
/*track_manager_form.method = "post";*/
/*track_manager_form.enctype = "multipart/form-data";*/

/*var input_bamfile = document.createElement("input");*/
/*input_bamfile.type = "file";*/
/*input_bamfile.name = "bam_filename";*/

/*var upload_bamfile = new dijit.form.Button({id: "upload_bamfile", label: "Upload BAM", onClick: uploadBAM});*/

/*var input_regionfile = document.createElement("input");*/
/*input_regionfile.type = "file";*/
/*input_regionfile.name = "region_filename";*/

/*var upload_regionfile = new dijit.form.Button({id: "upload_regionfile", label: "Upload Region", onClick: uploadRegion})*/

/*var delete_button = new dijit.form.Button({id: "delete_button", label: "Delete Track", onClick: deleteSubmit});*/

/*track_manager_form.appendChild( input_bamfile );*/
/*track_manager_form.appendChild( upload_bamfile.domNode );*/
/*track_manager_form.appendChild( input_regionfile );*/
/*track_manager_form.appendChild( upload_regionfile.domNode );*/
/*track_manager_form.appendChild( delete_button.domNode );*/

/*track_manager_div.appendChild( track_manager_status );*/
/*track_manager_div.appendChild( track_manager_form );*/
/*leftPane.appendChild( track_manager_div );*/

/*//// end my stuff/////////////////////*/



/*var leftWidget = new dijit.layout.ContentPane({region: "left", splitter: true}, leftPane);*/
/*var trackListDiv = document.createElement("div");*/
/*trackListDiv.id = "tracksAvail";*/
/*trackListDiv.className = "container handles";*/
/*trackListDiv.style.cssText =*/
/*"width: 100%; height: 100%; overflow-x: hidden; overflow-y: auto;";*/
/*trackListDiv.innerHTML =*/
/*"Available Tracks:<br/>(Drag <img src=\""*/
/*+ (params.browserRoot ? params.browserRoot : "")*/
/*+ "img/right_arrow.png\"/> to view)<br/><br/>";*/
/*leftPane.appendChild(trackListDiv);*/

/*var brwsr = this;*/

/*var changeCallback = function() {*/
/*brwsr.view.showVisibleBlocks(true);*/
/*};*/

/*var trackListCreate = function(track, hint) {*/
/*var node = document.createElement("div");*/
/*node.className = "tracklist-label";*/
/*node.innerHTML = track.key;*/
/*//in the list, wrap the list item in a container for*/
/*//border drag-insertion-point monkeying*/
/*if ("avatar" != hint) {*/
/*var container = document.createElement("div");*/
/*container.className = "tracklist-container";*/
/*container.appendChild(node);*/
/*node = container;*/
/*}*/
/*node.id = dojo.dnd.getUniqueId();*/
/*return {node: node, data: track, type: ["track"]};*/
/*};*/

/*this.trackListWidget = new dojo.dnd.Source(trackListDiv,*/
/*{creator: trackListCreate,*/
/*accept: ["track"],*/
/*withHandles: false});*/

/*var trackCreate = function(track, hint) {*/
/*var node;*/
/*if ("avatar" == hint) {*/
/*return trackListCreate(track, hint);*/
/*} else {*/
/*var replaceData = {refseq: brwsr.refSeq.name};*/
/*var url = track.url.replace(/\{([^}]+)\}/g, function(match, group) {return replaceData[group];});*/
/*var klass = eval(track.type);*/
/*var newTrack = new klass(track, url, brwsr.refSeq,*/
/*{*/
/*changeCallback: changeCallback,*/
/*trackPadding: brwsr.view.trackPadding,*/
/*baseUrl: brwsr.dataRoot,*/
/*charWidth: brwsr.view.charWidth,*/
/*seqHeight: brwsr.view.seqHeight*/
/*});*/
/*node = brwsr.view.addTrack(newTrack);*/
/*}*/
/*return {node: node, data: track, type: ["track"]};*/
/*};*/
/*this.viewDndWidget = new dojo.dnd.Source(this.view.zoomContainer,*/
/*{*/
/*creator: trackCreate,*/
/*accept: ["track"],*/
/*withHandles: true*/
/*});*/
/*dojo.subscribe("/dnd/drop", function(source,nodes,iscopy){*/
/*brwsr.onVisibleTracksChanged();*/
/*//multi-select too confusing?*/
/*//brwsr.viewDndWidget.selectNone();*/
/*});*/

/*this.trackListWidget.insertNodes(false, params.trackData);*/
/*var oldTrackList = dojo.cookie(this.container.id + "-tracks");*/
/*if (params.tracks) {*/
/*this.showTracks(params.tracks);*/
/*} else if (oldTrackList) {*/
/*this.showTracks(oldTrackList);*/
/*} else if (params.defaultTracks) {*/
/*this.showTracks(params.defaultTracks);*/
/*}*/

/*return trackListDiv;*/
/*};*/

/**
 * @private
 */
Browser.prototype.onVisibleTracksChanged = function() {
    this.view.updateTrackList();
    var trackLabels = dojo.map(this.view.tracks,
                               function(track) { return track.name; });
    dojo.cookie(this.container.id + "-tracks",
                trackLabels.join(","),
                {expires: 60});
    this.view.showVisibleBlocks();
};

/**
 * @private
 * add new tracks to the track list
 * @param trackList list of track information items
 * @param replace true if this list of tracks should replace any existing
 * tracks, false to merge with the existing list of tracks
 */

Browser.prototype.addTracks = function(trackList, replace) {
    if (!this.isInitialized) {
        var brwsr = this;
        this.deferredFunctions.push(
            function() {brwsr.addTracks(trackList, show); }
        );
	return;
    }

    this.tracks.concat(trackList);
    if (show || (show === undefined)) {
        this.showTracks(dojo.map(trackList,
                                 function(t) {return t.label;}).join(","));
    }
};

/**
 * navigate to a given location
 * @example
 * gb=dojo.byId("GenomeBrowser").genomeBrowser
 * gb.navigateTo("ctgA:100..200")
 * gb.navigateTo("f14")
 * @param loc can be either:<br>
 * &lt;chromosome&gt;:&lt;start&gt; .. &lt;end&gt;<br>
 * &lt;start&gt; .. &lt;end&gt;<br>
 * &lt;center base&gt;<br>
 * &lt;feature name/ID&gt;
 */
Browser.prototype.navigateTo = function(loc) {
    if (!this.isInitialized) {
        var brwsr = this;
        this.deferredFunctions.push(function() { brwsr.navigateTo(loc); });
	return;
    }

    loc = dojo.trim(loc);
    //                                (chromosome)    (    start      )   (  sep     )     (    end   )
    var matches = String(loc).match(/^(((\S*)\s*:)?\s*(-?[0-9,.]*[0-9])\s*(\.\.|-|\s+))?\s*(-?[0-9,.]+)$/i);
    //matches potentially contains location components:
    //matches[3] = chromosome (optional)
    //matches[4] = start base (optional)
    //matches[6] = end base (or center base, if it's the only one)
    if (matches) {
	if (matches[3]) {
	    var refName;
	    for (ref in this.allRefs) {
		if ((matches[3].toUpperCase() == ref.toUpperCase())
                    ||
                    ("CHR" + matches[3].toUpperCase() == ref.toUpperCase())
                    ||
                    (matches[3].toUpperCase() == "CHR" + ref.toUpperCase())) {

		    refName = ref;
                }
            }
	    if (refName) {
		dojo.cookie(this.container.id + "-refseq", refName, {expires: 60});
		if (refName == this.refSeq.name) {
		    //go to given start, end on current refSeq
		    this.view.setLocation(this.refSeq,
					  parseInt(matches[4].replace(/[,.]/g, "")),
					  parseInt(matches[6].replace(/[,.]/g, "")));
		} else {
		    //new refseq, record open tracks and re-open on new refseq
                    var curTracks = [];
                    this.viewDndWidget.forInItems(function(obj, id, map) {
                            curTracks.push(obj.data);
                        });

		    for (var i = 0; i < this.chromList.options.length; i++)
			if (this.chromList.options[i].text == refName)
			    this.chromList.selectedIndex = i;
		    this.refSeq = this.allRefs[refName];
		    //go to given refseq, start, end
		    this.view.setLocation(this.refSeq,
					  parseInt(matches[4].replace(/[,.]/g, "")),
					  parseInt(matches[6].replace(/[,.]/g, "")));

                    this.viewDndWidget.insertNodes(false, curTracks);
                    this.onVisibleTracksChanged();
		}
		return;
	    }
	} else if (matches[4]) {
	    //go to start, end on this refseq
	    this.view.setLocation(this.refSeq,
				  parseInt(matches[4].replace(/[,.]/g, "")),
				  parseInt(matches[6].replace(/[,.]/g, "")));
	    return;
	} else if (matches[6]) {
	    //center at given base
	    this.view.centerAtBase(parseInt(matches[6].replace(/[,.]/g, "")));
	    return;
	}
    }
    //if we get here, we didn't match any expected location format

    var brwsr = this;
    this.names.exactMatch(loc, function(nameMatches) {
	    var goingTo;
	    //first check for exact case match
	    for (var i = 0; i < nameMatches.length; i++) {
		if (nameMatches[i][1] == loc)
		    goingTo = nameMatches[i];
	    }
	    //if no exact case match, try a case-insentitive match
            if (!goingTo) {
                for (var i = 0; i < nameMatches.length; i++) {
                    if (nameMatches[i][1].toLowerCase() == loc.toLowerCase())
                        goingTo = nameMatches[i];
                }
            }
            //else just pick a match
	    if (!goingTo) goingTo = nameMatches[0];
	    var startbp = goingTo[3];
	    var endbp = goingTo[4];
	    var flank = Math.round((endbp - startbp) * .2);
	    //go to location, with some flanking region
	    brwsr.navigateTo(goingTo[2]
			     + ":" + (startbp - flank)
			     + ".." + (endbp + flank));
	    brwsr.showTracks(brwsr.names.extra[nameMatches[0][0]]);
	});
};

/**
 * load and display the given tracks
 * @example
 * gb=dojo.byId("GenomeBrowser").genomeBrowser
 * gb.showTracks("DNA,gene,mRNA,noncodingRNA")
 * @param trackNameList {String} comma-delimited string containing track names,
 * each of which should correspond to the "label" element of the track
 * information dictionaries
 */
Browser.prototype.showTracks = function(trackNameList) {
    if (!this.isInitialized) {
        var brwsr = this;
        this.deferredFunctions.push(
            function() { brwsr.showTracks(trackNameList); }
        );
	return;
    }

    var trackNames = trackNameList.split(",");
    var removeFromList = [];
    var brwsr = this;
    for (var n = 0; n < trackNames.length; n++) {
        this.trackListWidget.forInItems(function(obj, id, map) {
                if (trackNames[n] == obj.data.label) {
                    brwsr.viewDndWidget.insertNodes(false, [obj.data]);
                    removeFromList.push(id);
                }
            });
    }
    var movedNode;
    for (var i = 0; i < removeFromList.length; i++) {
        this.trackListWidget.delItem(removeFromList[i]);
        movedNode = dojo.byId(removeFromList[i]);
        movedNode.parentNode.removeChild(movedNode);
    }
    this.onVisibleTracksChanged();
};

/**
 * @returns {String} string representation of the current location<br>
 * (suitable for passing to navigateTo)
 */
Browser.prototype.visibleRegion = function() {
    return this.view.ref.name + ":" + Math.round(this.view.minVisible()) + ".." + Math.round(this.view.maxVisible());
};

/**
 * @returns {String} containing comma-separated list of currently-viewed tracks<br>
 * (suitable for passing to showTracks)
 */
Browser.prototype.visibleTracks = function() {
    var trackLabels = dojo.map(this.view.tracks,
                               function(track) { return track.name; });
    return trackLabels.join(",");
};

/**
 * @private
 */
Browser.prototype.onCoarseMove = function(startbp, endbp) {
    this.interestingAreas.updateViewFrame( startbp, endbp );
    var length = this.view.ref.end - this.view.ref.start;
    var trapLeft = Math.round((((startbp - this.view.ref.start) / length)
                               * this.view.overviewBox.w) + this.view.overviewBox.l);
    var trapRight = Math.round((((endbp - this.view.ref.start) / length)
                                * this.view.overviewBox.w) + this.view.overviewBox.l);

    this.view.locationThumb.style.cssText =
    "height: " + (this.view.overviewBox.h - 4) + "px; "
    + "left: " + trapLeft + "px; "
    + "width: " + (trapRight - trapLeft) + "px;"
    + "z-index: 20";

    //since this method gets triggered by the initial GenomeView.sizeInit,
    //we don't want to save whatever location we happen to start at
    if (! this.isInitialized) return;
    var locString = Util.addCommas(Math.round(startbp)) + " .. " + Util.addCommas(Math.round(endbp));
    this.locationBox.value = locString;
    this.goButton.disabled = true;
    this.locationBox.blur();
    var oldLocMap = dojo.fromJson(dojo.cookie(this.container.id + "-location"));
    if ((typeof oldLocMap) != "object") oldLocMap = {};
    oldLocMap[this.refSeq.name] = locString;
    dojo.cookie(this.container.id + "-location",
                dojo.toJson(oldLocMap),
                {expires: 60});

    document.title = this.refSeq.name + ":" + locString;
};

/**
 * @private
 */
Browser.prototype.createNavBox = function(parent, locLength, params) {
    var brwsr = this;
    var navbox = document.createElement("div");
    var browserRoot = params.browserRoot ? params.browserRoot : "";
    navbox.id = "navbox";
    parent.appendChild(navbox);
    navbox.style.cssText = "text-align: center; padding: 2px; z-index: 10;";

    if (params.bookmark) {
        this.link = document.createElement("a");
        this.link.appendChild(document.createTextNode("Link"));
        this.link.href = window.location.href;
        dojo.connect(this, "onCoarseMove", function() {
                         brwsr.link.href = params.bookmark(brwsr);
                     });
        dojo.connect(this, "onVisibleTracksChanged", function() {
                         brwsr.link.href = params.bookmark(brwsr);
                     });
        this.link.style.cssText = "float: right; clear";
        navbox.appendChild(this.link);
    }

    var getNewCenterClosure = function(arrow_direction){
        var callback = function(event){
            dojo.stopEvent(event);   
            var new_center = (arrow_direction=="left") ? 
                             brwsr.interestingAreas.getNextLeftSite( brwsr.view.minVisible(), brwsr.view.maxVisible() ) : 
                             brwsr.interestingAreas.getNextRightSite( brwsr.view.minVisible(), brwsr.view.maxVisible() );
            if( new_center >= 0 ){
                brwsr.view.centerAtBase( new_center );
            }
        }
        return callback;
    }

    var moveLeft = document.createElement("input");
    moveLeft.type = "image";
    moveLeft.src = browserRoot + "img/slide-left.png";
    moveLeft.id = "moveLeft";
    moveLeft.className = "icon nav";
    moveLeft.style.height = "40px";
    dojo.connect(moveLeft, "click", getNewCenterClosure("left") 
            /*function(event) {*/
            /*dojo.stopEvent(event);*/
            /*brwsr.view.slide(0.9);*/
            /*}*/
                );
    navbox.appendChild(moveLeft);

    var moveRight = document.createElement("input");
    moveRight.type = "image";
    moveRight.src = browserRoot + "img/slide-right.png";
    moveRight.id="moveRight";
    moveRight.className = "icon nav";
    moveRight.style.height = "40px";
    dojo.connect(moveRight, "click", getNewCenterClosure("right")
            /*function(event) {*/
            /*dojo.stopEvent(event);*/
            /*brwsr.view.slide(-0.9);*/
            /*}*/
                );
    navbox.appendChild(moveRight);

    navbox.appendChild(document.createTextNode("\u00a0\u00a0\u00a0\u00a0"));

    var bigZoomOut = document.createElement("input");
    bigZoomOut.type = "image";
    bigZoomOut.src = browserRoot + "img/zoom-out-2.png";
    bigZoomOut.id = "bigZoomOut";
    bigZoomOut.className = "icon nav";
    bigZoomOut.style.height = "40px";
    navbox.appendChild(bigZoomOut);
    dojo.connect(bigZoomOut, "click",
                 function(event) {
                     dojo.stopEvent(event);
                     brwsr.view.zoomOut(undefined, undefined, 2);
                 });

    var zoomOut = document.createElement("input");
    zoomOut.type = "image";
    zoomOut.src = browserRoot + "img/zoom-out-1.png";
    zoomOut.id = "zoomOut";
    zoomOut.className = "icon nav";
    zoomOut.style.height = "40px";
    dojo.connect(zoomOut, "click",
                 function(event) {
                     dojo.stopEvent(event);
                     brwsr.view.zoomOut();
                 });
    navbox.appendChild(zoomOut);

    var zoomIn = document.createElement("input");
    zoomIn.type = "image";
    zoomIn.src = browserRoot + "img/zoom-in-1.png";
    zoomIn.id = "zoomIn";
    zoomIn.className = "icon nav";
    zoomIn.style.height = "40px";
    dojo.connect(zoomIn, "click",
                 function(event) {
                     dojo.stopEvent(event);
                     brwsr.view.zoomIn();
                 });
    navbox.appendChild(zoomIn);

    var bigZoomIn = document.createElement("input");
    bigZoomIn.type = "image";
    bigZoomIn.src = browserRoot + "img/zoom-in-2.png";
    bigZoomIn.id = "bigZoomIn";
    bigZoomIn.className = "icon nav";
    bigZoomIn.style.height = "40px";
    dojo.connect(bigZoomIn, "click",
                 function(event) {
                     dojo.stopEvent(event);
                     brwsr.view.zoomIn(undefined, undefined, 2);
                 });
    navbox.appendChild(bigZoomIn);

    navbox.appendChild(document.createTextNode("\u00a0\u00a0\u00a0\u00a0"));
    this.chromList = document.createElement("select");
    this.chromList.id="chrom";
    navbox.appendChild(this.chromList);
    this.locationBox = document.createElement("input");
    this.locationBox.size=locLength;
    this.locationBox.type="text";
    this.locationBox.id="location";
    dojo.connect(this.locationBox, "keydown", function(event) {
            if (event.keyCode == dojo.keys.ENTER) {
                brwsr.navigateTo(brwsr.locationBox.value);
                //brwsr.locationBox.blur();
                brwsr.goButton.disabled = true;
                dojo.stopEvent(event);
            } else {
                brwsr.goButton.disabled = false;
            }
        });
    navbox.appendChild(this.locationBox);

    this.goButton = document.createElement("button");
    this.goButton.appendChild(document.createTextNode("Go"));
    this.goButton.disabled = true;
    dojo.connect(this.goButton, "click", function(event) {
            brwsr.navigateTo(brwsr.locationBox.value);
            //brwsr.locationBox.blur();
            brwsr.goButton.disabled = true;
            dojo.stopEvent(event);
        });
    navbox.appendChild(this.goButton);

    return navbox;
};

/*

Copyright (c) 2007-2009 The Evolutionary Software Foundation

Created by Mitchell Skinner <mitch_skinner@berkeley.edu>

This package and its accompanying libraries are free software; you can
redistribute it and/or modify it under the terms of the LGPL (either
version 2.1, or at your option, any later version) or the Artistic
License 2.0.  Refer to LICENSE for the full license text.

*/
