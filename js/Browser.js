

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
    dojo.require("dijit.form.ToggleButton");
    dojo.require("dijit.form.TextBox");
    dojo.require("dijit.form.Textarea");
    dojo.require("dojox.form.FileInput");
    dojo.require("dijit.form.CheckBox");
    dojo.require("dijit.form.ValidationTextBox");
    dojo.require("dijit.form.MultiSelect");
    dojo.require("dojo.io.iframe");
    dojo.require("dojox.layout.ExpandoPane");
    dojo.require("dijit.layout.AccordionContainer");
    dojo.require("dijit.form.VerticalSlider");
    dojo.require("dijit.Tree");
    dojo.require("dijit.tree.dndSource");
    dojo.require("dijit.tree.TreeStoreModel");
    dojo.require("dojox.data.FileStore");
    dojo.require("dijit.Menu");
    dojo.require("dijit.Dialog");
    dojo.require("dijit.ProgressBar");
    // end my stuff

    this.refSeqs = params.refSeqs;
    this.trackData = params.trackData;
    this.globals = params.globals;
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
    for( track in this.trackData ){
        this.tracks.push( this.trackData[track]["key"] );
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
            var initRefseq = "hg18";
            var maxBase = brwsr.refSeqs[initRefseq].reduce(function(a,b) {return a.end > b.end ? a : b;}).end;
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
            brwsr.maxRender = 5;            
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
                }, sliderDiv);

            //create location trapezoid
            brwsr.locationTrap = document.createElement("div");
            brwsr.locationTrap.className = "locationTrap";
            topPane.appendChild(brwsr.locationTrap);
            topPane.style.overflow="hidden";

            //close in the containerID
            brwsr.setRefseq = brwsr.closeSetRefseq(params.containerID);
            brwsr.setRefseq(initRefseq); 

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
            brwsr.createProjectExplorer( brwsr, containerWidget, params );

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

Browser.prototype.closeSetRefseq = function(containerID){
    return function(assembly){
        var brwsr = this;
        var refSeqs = brwsr.refSeqs[assembly];
        //set up ref seqs
        brwsr.allRefs = {};
        for (var i = 0; i < refSeqs.length; i++)
            brwsr.allRefs[refSeqs[i].name] = refSeqs[i];

        var refCookie = dojo.cookie(containerID + "-refseq");
        brwsr.refSeq = refSeqs[0];
        for (var i = 0; i < refSeqs.length; i++) 
        {
            brwsr.chromList.options[i] = new Option(refSeqs[i].name,
                                                    refSeqs[i].name);
            if (refSeqs[i].name.toUpperCase() 
                == String(refCookie).toUpperCase()) 
            {
                brwsr.refSeq = brwsr.allRefs[refSeqs[i].name];
                brwsr.chromList.selectedIndex = i;
            }
        }
        brwsr.assembly = assembly;
    }
};

Browser.prototype.createProjectExplorer = function(brwsr, parent, params) {
    
////////////////////////////////////////////////////////////////////////
   //                     Query Stuff
////////////////////////////////////////////////////////////////////////
    
    //the div to make bordercontainer out of
    var query_dialog_div = document.createElement("div");
    query_dialog_div.id = "query_dialog_div";
    //query_div.appendChild( query_dialog_div );

    // to make nested border container
    var query_right_div = document.createElement("div");
    query_dialog_div.appendChild( query_right_div );

    //holds the uploaded tables stuff, goes on the bottom right
    var query_interval_table_p = document.createElement("div");
    query_right_div.appendChild( query_interval_table_p );

    //holds the list of donor genomes
    var query_donor_list_div = document.createElement("div");
    query_right_div.appendChild( query_donor_list_div );

    //query name, goes on the top
    var query_name_div = document.createElement("div");
    query_name_div.id = "query_name_div";
    
    var query_name_p = document.createElement("p");
    //query_name_div.appendChild( query_name_p );

    query_name_p.innerHTML = "Name <br />";
    //query_name_div.appendChild( query_name_p );

    query_dialog_div.appendChild( query_name_p );

    var query_name = new dijit.form.ValidationTextBox(
            {id: "query_name",
             label: "Query Name",
             name: "query_name",
             regExp: '\\w+',
             invalidMessage: 'Only alphanumeric characters' }
        ).placeAt( query_name_p );

    //query box, goes in the center
    //var query_box_p = document.createElement("p");
    //query_box_p.id = "query_box_p";
    query_name_p.innerHTML += "<br /><br />Query<br />";
    query_dialog_div.appendChild( query_name_p );

    var query_box = new dijit.form.Textarea(
                        {id : "query_box",
                         name: "query_box",
                         style: "height: 12em; width: 90%"}
                    ).placeAt( query_name_p );
 
    //div for the button, made into a contentPane below runQuery()
    //var query_button_div = document.createElement("div");
    //query_button_div.id = "query_button_div";
    //query_dialog_div.appendChild( query_button_div );

   var query_bc = new dijit.layout.BorderContainer(
            {id:"query_bc",
             title: "Query",
             design: 'headline',
             style: "width: 500px; height: 500px; background-color: #FFFFFF;"
            }, query_dialog_div) ;
   
   var query_right_bc = new dijit.layout.BorderContainer(
            {id:"query_right_bc",
             title: "Query",
             design: 'headline',
             region: "right",
             style: "width: 200px; height: 500px; background-color: #ffffff;"
            }, query_right_div) ;
 
    
   var query_interval_table_cp = new dijit.layout.ContentPane(
            {id: "query_interval_table_cp",
             style: "height: 50%; background-color: #efefef",
             region: "top"} 
             , query_interval_table_p);

   var query_donor_list_cp = new dijit.layout.ContentPane(
            {id: "query_donor_list_cp",
             style: "height: 50%; background-color: #efefef",
             region: "center"} 
             , query_donor_list_div);

   //var query_form = new dijit.form.Form(
   //{id: "query_form",
   //encType : "multipart/form-data"},
   //query_name_p);

    var query_name_cp = new dijit.layout.ContentPane(
            {id: "query_name_cp",
                style: "background-color: #efefef",
             region: "center", 
             layoutPriority: "1"}, query_name_p);
    
    
    //var query_box_cp = new dijit.layout.ContentPane(
    //{id: "query_box_cp",
    //style: "background-color: #efefef",
    //region: "center", 
    //layoutPriority: "1"}, query_box_p);

    
   

    brwsr.running_query = false;
    //deprecated
    /*var queryChromosomes = function( project, donor, chroms, trackkey, progress_chrom, messages ){*/
    /*var query_name = dojo.byId("query_name").value;*/
    /*var query_box = dojo.byId("query_box").value;*/
    /*var args = {"query_donor" : donor, */
    /*"query_chrom" : chroms[0],*/
    /*"query_box" : query_box,*/
    /*"query_name" : query_name,*/
    /*"query_project" : project,*/
    /*"assembly" : brwsr.assembly};*/
    /*var url = "bin/run_query.py?" + dojo.objectToQuery(args);*/
    /*//TODO: security security SECURITY!! could manually pass in supposedly inaccessible donor name*/
    /*var xhrArgs = {*/
    /*url: url,*/
    /*form: dojo.byId("query_form"),*/
    /*handleAs: "json",*/
    /*load: function(data,ioargs){*/
    /*if( data["status"] == "OK" ){*/
    /*var progress = progress_bar.get("progress");*/
    /*progress_bar.update({'indeterminate': true, 'label': "Chroms [1.."+progress_chrom+"] complete"});*/
    /*messages.push(data["message"]);*/
    /*var entry = data["trackData"];*/
    /*if( entry['key'] != trackkey ){*/
    /*alert( 'internal: trackkeys do not line up' );*/
    /*}*/

    /*if( dojo.indexOf( brwsr.tracks, trackkey ) == -1 ){*/
    /*brwsr.trackData.push( entry );*/
    /*brwsr.tracks.push( trackkey );*/
    /*refreshTree();*/
    /*}*/

    /*if( chroms.length > 1 && brwsr.running_query ){*/
    /*var n = chroms.slice(1);*/
    /*queryChromosomes( project, donor, n, trackkey, progress_chrom+1, messages);*/
    /*}*/
    /*else{*/
    /*alert( messages.join('\n') );*/
    /*dojo.attr(progress_bar.domNode, 'hidden', true);*/
    /*dojo.style(dijit.byId('stop_button').domNode, {*/
    /*visibility: 'hidden',*/
    /*display: 'none'*/
    /*});*/
    /*setRunningQuery( false );*/
    /*}*/
    /*}*/
    /*else {*/
    /*alert(data["message"]);*/
    /*dojo.attr(progress_bar.domNode, 'hidden', true);*/
    /*dojo.style(dijit.byId('stop_button').domNode, {*/
    /*visibility: 'hidden',*/
    /*display: 'none'*/
    /*});*/
    /*//dojo.attr(stop_button.domNode, 'hidden', true);*/
    /*setRunningQuery( false );*/
    /*//progress_bar.update({'progress': 0});*/
    /*}*/
    /*},*/
    /*error: function(error) {*/
    /*alert(error);*/
    /*dojo.attr(progress_bar.domNode, 'hidden', true);*/
    /*dojo.style(dijit.byId('stop_button').domNode, {*/
    /*visibility: 'hidden',*/
    /*display: 'none'*/
    /*});*/
    /*dojo.attr(stop_button.domNode, 'hidden', true);*/
    /*setRunningQuery( false );*/
    /*//progress_bar.update({'progress': 0});*/
    /*}*/
    /*};*/
    /*//Call the asynchronous xhrPost*/
    /*var deferred = dojo.xhrPost(xhrArgs);*/
    /*}*/

    var runQuery = function(){
        if( tree.clickedItem.prefix != brwsr.globals["PROJECT_PREFIX"] ){
            alert("Cannot run query from anything other than a project.");
            return;
        }
        var project = tree.clickedItem.name;
        var query_name =  dojo.byId("query_name").value;


        var trackkey = project + "/" + query_name;
        if( dojo.byId("query_box").value == "" ){
            alert("You must enter a query");
        }
        else if( query_name == "" ){
            alert("You must enter a name for ths query");
        }
        else if( hasNameConflict( trackkey ) ){
            alert( "There is already a query with that name" );
        }
        else {
            query_dialog.hide();
            chroms = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,'X','Y']; 
            var messages = [];
            if( brwsr.running_query ){
                alert("Already a query running. Should not have been possible to get a query dialog");
                return;
            }
            setRunningQuery( true );
            alert("Your query is running. The progress bar indicates the completed chromosomes (which can be visualized).  Do not refresh.");

            dojo.attr(progress_bar.domNode, 'hidden', false);
            dojo.style(dijit.byId('stop_button').domNode, {
                  visibility: 'visible',
                  display: 'block'
            });
            progress_bar.update({'indeterminate': true, 
                                 'label': 'Working on chr1...'});
            
            var query_name = dojo.byId("query_name").value;
            var query_box = dojo.byId("query_box").value;
            var args = {"query_box" : query_box,
                        "query_name" : query_name,
                        "query_project" : project,
                        "assembly" : tree.selectedItem.assembly};
            var url = "bin/run_query.py?" + dojo.objectToQuery(args);
            //TODO: security security SECURITY!! could manually pass in supposedly inaccessible donor name
            var xhrArgs = {
                url: url,
                form: dojo.byId("query_form"),
                handleAs: "json",
                load: function(data,ioargs){
                    //query has finished
                    if( data["status"] == "OK" ){
                        var progress = progress_bar.get("progress");
                        progress_bar.update({'indeterminate': true, 'label': "All done"});
                        for( i in data["trackData"] ){
                            //if( dojo.indexOf( brwsr.tracks, trackkey ) == -1 ){
                            brwsr.trackData.push( data["trackData"][i] );
                            brwsr.tracks.push( data["trackData"][i].key );
                        }

                        alert( data["message"] );
                        dojo.attr(progress_bar.domNode, 'hidden', true);
                        dojo.style(dijit.byId('stop_button').domNode, {
                              visibility: 'hidden',
                              display: 'none'
                        });
                        setRunningQuery( false );
                        refreshTree();
                    }
                    else {
                        alert(data["message"]);
                        dojo.attr(progress_bar.domNode, 'hidden', true);
                        dojo.style(dijit.byId('stop_button').domNode, {
                              visibility: 'hidden',
                              display: 'none'
                        });
                        //dojo.attr(stop_button.domNode, 'hidden', true);
                        setRunningQuery( false );
                        //progress_bar.update({'progress': 0});
                    }
                },
                error: function(error) {
                    alert(error);
                    dojo.attr(progress_bar.domNode, 'hidden', true);
                        dojo.style(dijit.byId('stop_button').domNode, {
                              visibility: 'hidden',
                              display: 'none'
                        });
                    dojo.attr(stop_button.domNode, 'hidden', true);
                    setRunningQuery( false );
                    //progress_bar.update({'progress': 0});
                }
            };
            //Call the asynchronous xhrPost
            var deferred = dojo.xhrPost(xhrArgs);

            //queryChromosomes( project, donor, chroms, trackkey, 1, messages );
        }
    };


    var query_button = new dijit.form.Button(
            {id: "query_button", 
             label: "Run Query",
             style: "align-text: right; margin-top: 30px;",
             onClick: runQuery
            }).placeAt( query_name_p) ;

    //var query_button_cp = new dijit.layout.ContentPane(
    //{id: "query_button_cp",
    //style: "background-color: #efefef",
    //region: "bottom", 
    //layoutPriority: "1"}, query_button_div);

 /////////////// upload table stuff //////////////////////////////////
    
    var table_dialog_div = document.createElement("div");
    table_dialog_div.id = "table_dialog_div";

    var upload_div = document.createElement("div");
    upload_div.id = "upload_div";
    table_dialog_div.appendChild( upload_div );

    var interval_table_p = document.createElement("p");
    interval_table_p.id = "interval_table_p";
    interval_table_p.style.cssText = "border-top: solid 3px #cdcdcd; padding-top: 10px";
    upload_div.appendChild( interval_table_p );
    

    var interval_table = document.createElement("input");
    interval_table.type = "file";
    interval_table.id = "interval_table";
    interval_table.name = "interval_table";
    interval_table.style.cssText = "border-top: 10px;";
    upload_div.appendChild( interval_table );

    var upload_button = new dijit.form.Button(
            {id: "upload_button", 
             label: "Upload",
             style: "align-text: right;",
             onClick: function(){ 
                 args = {"project_name" : tree.selectedItem.name};
                 url = "bin/upload_interval_table.py?" 
                       + dojo.objectToQuery(args);
                 dojo.io.iframe.send({
                     url: url,
                     method: "post",
                     handleAs: "json",
                     form: dojo.byId("upload_form"),
                     load: function(data,ioArgs) {
                         if( data['status'] == 'OK' ){
                             alert("Table: '" 
                                   + data['message'] 
                                   + "' uploaded" );
                             table_dialog.hide();
                         }
                         else{
                             alert( data['message'] );
                         }
                     },
                     error: function(response, ioArgs){
                         alert(response);
                     }        
                })
             }
         }).placeAt( upload_div );

    var upload_form = new dijit.form.Form(
            {id: "upload_form",
             method: "post",
             encType : "multipart/form-data"},
        upload_div );
   
    /////////////////// New project form /////////////////////////////
    //
    var new_project_dialog_div = document.createElement("div");
    new_project_dialog_div.id = "new_project_dialog_div";
    
    var project_name_p = document.createElement("p");
    project_name_p.id = "project_name_p";
    project_name_p.innerHTML = "Project Name: <br />";
    new_project_dialog_div.appendChild( project_name_p );

    var project_name = new dijit.form.ValidationTextBox(
                        {id: "project_name",
                         label: "Project Name",
                         name: "project_name",
                         regExp: '\\w+',
                         invalidMessage: 'Only alphanumeric characters' }
                     ).placeAt( project_name_p );

    //TODO: prompt user to specific which assembly it references
    var new_project_button = new dijit.form.Button(
            {id: "new_project_button", 
             label: "Create Project",
             style: "align-text: right;",
             onClick: function(){ 
                var args = {"project_name" : dijit.byId("project_name").value};
                var url = "bin/make_new_project.py?" + dojo.objectToQuery(args);
                var xhrArgs = {
                    url: url,
        //form: dojo.byId("new_project_form"),
                    handleAs: "json",
                    load: function(data,ioargs) {
                        if( data["status"] == "ok" ){
                            project_dialog.hide();
                            refreshTree();
                        }
                        else{       
                            alert(data["message"]);
                        }
                    },
                    error: function(error) {
                       alert(error); 
                    }
                }
                //Call the asynchronous xhrPost
                var deferred = dojo.xhrPost(xhrArgs);
             }

       }).placeAt( new_project_dialog_div );


    ////////////////////////////////////////////////////////////////////////////////////////////////
    //                        Explorer Stuff
    ///////////////////////////////////////////////////////////////////////////////////////////////

    var explorer_bc = new dijit.layout.BorderContainer(
           {id:"explorer_bc",
            title: "Explorer",
            style: "width: 20%; background-color: #efefef; border-style: none solid none none; border-color: #929292",
    //splitter: "true",
            region: "left"

           }).placeAt(parent);
   
    //holds 1) Tree
    //      2) Progress/status bar 
    var explorer_cpane = dijit.layout.ContentPane(
            {id : "explorer_cpane",
             region : "top",
             style : "height: 95%; background-color: #efefef; border-style: none solid none none; border-color: #929292;"} //overflow: hidden; background-color: #efefef; height: 70%;"}
        ).placeAt(explorer_bc.domNode );



    //interacts with filestore_dojotree to building working tree.  
    //This depends entirely on specific directory structure and naming
    var store = new dojox.data.FileStore( 
                {id : "store",
                 url : "bin/filestore_dojotree.py",
                 pathAsQueryParam : "true"}     
             );
    
    var model = new dijit.tree.ForestStoreModel(
                     {id : "model",
                      store : store,
                      rootId : "tree_root",
                      rootLabel : "Projects" }
                    );       

    ///////////////////////////////////
    //  Menu Stuff
    var pMenu = new dijit.Menu( {leftClickToOpen: true} );
    
    var visualize_menuitem =     
        new dijit.MenuItem({
            label: "Visualize",
            prefix: "query_",
            hidden: false,
            onClick: function(e) {
                var item = tree.clickedItem;
                var trackkey = item.key;
                var assembly = item.assembly;
                visualize(trackkey, assembly);
            }
        });
    pMenu.addChild( visualize_menuitem );
    var recall_menuitem = 
        new dijit.MenuItem({
            label: "Recall",
            prefix: "query_",
            hidden: true,
            onClick: function(e) {
                var item = tree.clickedItem;
                recall( item.key );
            }
        });
    pMenu.addChild( recall_menuitem) ;
    pMenu.addChild(
        new dijit.MenuItem({
            label: "Download",
            prefix: "query_",
            hidden: false,
            onClick: function(e) {
                var selected = tree.clickedItem;
                var query_name = selected.name; 
                var project_name = selected.project;
                var donor_name = selected.donor;
                var host_chrom = brwsr.refSeq.name;
                var chromnum = host_chrom.substring(3);
                var url = "data/" + 
                           sprintf( sprintf( globals.TRACK_TEMPLATE, 
                                             globals.PROJECT_PREFIX,
                                             globals.DONOR_PREFIX, 
                                             globals.QUERY_PREFIX,
                                             globals.CHROM_PREFIX ),
                                    project_name, 
                                    donor_name, 
                                    query_name,
                                    brwsr.refSeq.name ) + 
                           "/" + query_name + "_" + chromnum + ".bam";
                window.location = url;
            }
        }));
    pMenu.addChild(
            //TODO: don't want the project prefix in front of the imported
            //a la 'main_genes'.  Maybe change this in run_query, instead 
            //of printing the file we give the compiler, print the query 
            //we got from the user
        new dijit.MenuItem({
            label: "View Text",
            prefix: "query_",
            hidden: false,
            onClick: function(e) {
                var item = tree.clickedItem;
                var host_chrom = brwsr.refSeq.name;
                var chromnum = host_chrom.substring(3);
                var query_name = item.name;
                var donor_name = item.donor;
                var project_name = item.project;
                var url = "data/" + 
                           sprintf( sprintf( globals.TRACK_TEMPLATE, 
                                             globals.PROJECT_PREFIX,
                                             globals.DONOR_PREFIX, 
                                             globals.QUERY_PREFIX,
                                             globals.CHROM_PREFIX ), 
                                    project_name,
                                    donor_name, 
                                    query_name,
                                    brwsr.refSeq.name ) +  
                           "/../" + query_name + ".gq";
                 dojo.xhrGet({
                    url: url,
                    handleAs: "text",
                    load: function(data,args){
                        alert(data);
                    },
                    error: function(data,args){
                       alert("trouble retrieving the generating query");
                    }
                });
            }
        }));
    pMenu.addChild(
        new dijit.MenuItem({
            label: "Delete",
            prefix: "query_",
            iconClass: "dijitEditorIcon dijitEditorIconCut",
            hidden: false,
            onClick: function(e) {
                var item = tree.clickedItem;
                deleteQuery( item.project, item.donor, item.name ); 
        }}));

    
    var table_dialog = new dijit.Dialog({
                    id: "table_dialog",
                    title: "Upload Interval Table",
                    style: "width: 500px;",
                    content: table_dialog_div
                });
     
    var project_dialog = new dijit.Dialog({
                    id: "project_dialog",
                    title: "Create New Project",
        //style: "width: 500px;",
                    content: new_project_dialog_div
                });
 
    var query_dialog = new dijit.Dialog({
                    id : "query_dialog",
                    title: "New Query (under construction)",
        //style: "width: 500px; height: 200px",
                    content: query_dialog_div
                });

    var pleasewait_menuitem = new dijit.MenuItem({
        label: "(Please wait for the current query to finish)",
        hidden: true,
        disabled: true
    });
    pMenu.addChild( pleasewait_menuitem );

    var query_menuitem = new dijit.MenuItem({
            label: "New Query",
            prefix: "project_",
            hidden: false,
            onClick: function(e) {
                fillWithIntervalTables( query_interval_table_p );
                fillWithDonors( query_donor_list_div );
                query_dialog.show();   
        }});
    pMenu.addChild( query_menuitem );

    var fillWithIntervalTables = function( html_elem ){
        args = {"project_name" : tree.selectedItem.name};
        url = "bin/list_interval_tables.py?"+dojo.objectToQuery(args);
        dojo.xhrGet({
            url: url,
            handleAs: "json",
            load: function(data,args){
                var widgets = dijit.findWidgets(html_elem);
                dojo.forEach(widgets, function(w) {
                        w.destroyRecursive(true);
                });
                html_elem.innerHTML = "Use Tables: <br />";
                if( data['status'] == "empty" ){
                }
                else if( data["status"] == 'ok' ){
                    var button, scheme_link;
                    for ( i in data["message"] ){
                        button = dijit.form.ToggleButton(
                            {id : "table_button_" + i,
                             label : data["message"][i],
                             onClick : function(){
                                 if( this.checked ){
                                     alert("take some action to insert 'use' statement into query box");
                                 }
                                 else {
                                     alert("remove any 'use ...' from query box");
                                 }
                             },
                             iconClass : "dijitCheckBoxIcon"
                        }).placeAt(html_elem);

                        schema_link = document.createElement("a");
                        schema_link.id = "schema_link_" + i;
                        schema_link.appendChild(document.createTextNode("?"));
                        schema_link.style.cssText = "color: blue; text-decoration: underline; cursor: hand";

                        //dojo.connect(schema_link,
                        //"onmouseover",
                        //function(){
                        //this.getNode().style.cursor = "hand";
                        //}
                        //);
                        dojo.connect(schema_link,
                                     "onclick", 
                                      function(){ 
                                          alert("display schema"); 
                                      }
                                     );
                        html_elem.appendChild( schema_link );

                        html_elem.appendChild( document.createElement('br'));
                    }
                }
                else{
                    alert( data["message"] );
                }
            },
            error: function(data,args){
               alert(data);
               alert("trouble fetching the uploaded tables");
            }
        });
    };

    var fillWithDonors = function( html_elem ){
        args = {"project_name" : tree.selectedItem.name};
        url = "bin/list_donors.py?"+dojo.objectToQuery(args);
        dojo.xhrGet({
            url: url,
            handleAs: "json",
            load: function(data,args){

                var widgets = dijit.findWidgets(html_elem);
                dojo.forEach(widgets, function(w) {
                        w.destroyRecursive(true);
                });
                html_elem.innerHTML = "Query Donors: <br />";
                if( data['status'] == "EMPTY" ){
                    alert("empty");
                }
                else if( data["status"] == 'ok' ){
                    var button, scheme_link;
                    for ( i in data["message"] ){
                        button = dijit.form.ToggleButton(
                            {id : "donor_button_" + i,
                             label : data["message"][i],
                             onClick : function(){
                                 if( this.checked ){
                                     alert("change the query or set some interval variable to let the compiler know to only use the marked donors");
                                 }
                                 else {
                                     alert("remove the marked donor from the query");
                                 }
                             },
                             iconClass : "dijitCheckBoxIcon"
                        }).placeAt(html_elem);

                        html_elem.appendChild( document.createElement('br'));
                    }
                }
                else{
                    alert( data["message"] );
                }
            },
            error: function(data,args){
               alert("trouble fetching the donor list");
            }
        });
    };

    var table_menuitem = new dijit.MenuItem({
        label: "Upload Interval Table",
        prefix: "project_",
        hidden: false,
        onClick: function(e) {
            //fillWithIntervalTables( interval_table_p );    
            table_dialog.show();   
        }
    });
    pMenu.addChild( table_menuitem );

    var donor_upload_menuitem = new dijit.MenuItem({
        label: "Upload Donor Genome",
        prefix: "project_",
        hidden: false,
        onClick: function(e) {
            alert("Email aheiberg@ucsd.edu");
        }
    });
    pMenu.addChild( donor_upload_menuitem );
    
    var new_project_menuitem = new dijit.MenuItem({
        label: "New Project",
        prefix: "root_",
        hidden: false,
        onClick: function(e) {
            project_dialog.show();
                    }
    });
    pMenu.addChild( new_project_menuitem );

    pMenu.startup();

    //call this whenever somehting is added or deleted, will rebuild directory
    //could find nothing more elegant than rebuilding the whole damn thing
    //each time.
    var makeTree = function(){
        var tree = new dijit.Tree(
            {id : "tree",
             model : model,
             style: "height: 500px; background-color: #efefef;",
             onClick :
                function(selected,e){ 
                    this.clickedItem = selected;
                    var isVisualized = brwsr.view.isVisualized( selected.key );
                    if( isVisualized ){ 
                        visualize_menuitem.hidden = true;
                        recall_menuitem.hidden = false;
                    }
                    else { 
                        visualize_menuitem.hidden = false;
                        recall_menuitem.hidden = true;
                    }

                    if( brwsr.running_query ){
                        query_menuitem.hidden = true;
                        pleasewait_menuitem.hidden = false;
                    }
                    else {
                        query_menuitem.hidden = false;
                        pleasewait_menuitem.hidden = true;
                    }

                    //hack to make left click work on tree
                    //openOnLeftClick: true | is insufficient for whatever reason
                    //when dealing with dijit.Trees
                    pMenu._openMyself();
                }
                    //var trackkey = store.getValue(this.selectedItem, 'key');
                    //var isVisualized = brwsr.view.isVisualized( trackkey ); //f.length == 1;
                    //}
            })
        tree.placeAt( explorer_cpane.domNode ); 
        pMenu.bindDomNode(dojo.byId("tree"));
        return tree;
    };

    var tree = makeTree();
 
    //selective presentation of menu children depending on the tree.clickedItem 
    dojo.connect(pMenu, "_openMyself", this, function(e){
        var treeItem = dijit.getEnclosingWidget(e.target).item;
        if( treeItem.root ){
            treeItem.prefix = "root_";
        }
        var children = pMenu.getChildren();
        for( i in children ){
            child = children[i];
            //set hidden or not depending on the menuitems prefix
            dojo.attr(child.domNode, 'hidden', (treeItem.prefix != child.prefix || child.hidden));
        } 
    });

    //code stolen from Google
    //don't know if it is over/under kill
    var refreshTree = function(){
        dijit.byId("tree").model.store.clearOnClose = true;
        dijit.byId("tree").model.store.close();
   
        dijit.byId("tree").model.constructor(dijit.byId("tree").model)

        dijit.byId("tree").destroyRecursive();
        tree =  makeTree();
    };

    //TODO
    //delteed query, tried again with old name, got denied
    //problem is here?
    var deleteQuery = function( project, donor, query_name ){
        var args = {project: project, donor: donor, query_name: query_name};
        var url = "bin/remove_track.py?" + dojo.objectToQuery(args);
        var trackkey = donor +'/'+ query_name;

        var xhrArgs = {
            url: url,
            form: dojo.byId("track_manager_form"),
            handleAs: "json",
            load: function(data,ioargs) {
                if( data["status"] == "ok" ){
                    refreshTree();
                    recall( trackkey );
                    var ix = brwsr.tracks.indexOf(trackkey);
                    if( ix != -1 ){
                        brwsr.tracks.splice(ix,1);
                        brwsr.trackData.splice(ix,1);
                    }
                }
                else{       
                    alert(data["message"]);
                }
            },
            error: function(error) {
               alert(error); 
            }
        }
        //Call the asynchronous xhrPost
        var deferred = dojo.xhrPost(xhrArgs);
    };

    var visualize = function(trackkey, assembly){
        var tester = function(item){
            return item["key"] == trackkey;
        }
        var matches = dojo.filter( brwsr.trackData, tester );
        if( matches.length == 0 ){ alert(" no matches"); }
        else if( matches.length == 1 ){
            if( assembly != brwsr.assembly ){
                alert( "assembly doesn't match, recalling everything" );

                //not what we want, use brwsr.tracks?
                //var visualized_tracks = brwsr.viewDndWidget.getAllNodes();
                //for( i in visualized_tracks ){
                //visualized_tracks[i];
                //}
                brwsr.setRefseq( assembly );
            }
            brwsr.viewDndWidget.insertNodes( false, [matches[0]] );
            brwsr.onVisibleTracksChanged();
        }
        else{ alert(" too many matches"); }
    };

    var recall = function(trackkey){
        var isVisualized = brwsr.view.isVisualized( trackkey );
        if( isVisualized ){
            brwsr.viewDndWidget.delItem( 'track_'+trackkey );
            brwsr.view.zoomContainer.removeChild( 
                dojo.byId( 'track_'+trackkey )
            );
            brwsr.interestingAreas.removeTrack( trackkey );
        }
        //TODO, if all empty, set brwsr.assembly so that any track
        //can go in without a warning
        brwsr.onVisibleTracksChanged();

    };

    var status_cpane = dijit.layout.ContentPane(
            {id : "status_cpane",
             region : "bottom",
             style : "background-color: #efefef; margin-bottom: 20px; border-style: none solid none none; border-color: #929292;"}
        ).placeAt(explorer_bc.domNode );


    var progress_bar = dijit.ProgressBar(
            {id: "progress_bar",
             style: "width: 100%;" ,
            }
            ).placeAt( status_cpane.domNode );

    var stop_button = dijit.form.Button(
            {id: "stop_button",
             label: "Stop",
             style: "",
             onClick: 
                function(){
                    setRunningQuery( false );
                    dojo.attr(progress_bar.domNode, 'hidden', true);
                    dojo.style(dijit.byId('stop_button').domNode, {
                          visibility: 'hidden',
                          display: 'none'
                    });
                    //progress_bar.update({'indeterminate': true, label: '0/23'});       
                }
            }
            ).placeAt( status_cpane.domNode );
            
    dojo.attr(progress_bar.domNode, 'hidden', true);
    dojo.style(dijit.byId('stop_button').domNode, {
          visibility: 'hidden',
          display: 'none'
    });

    var setRunningQuery = function( tf ){
        brwsr.running_query = tf;
        stop_button.set( "disabled", !tf )
    };



    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //            Some Helper functions
    ///////////////////////////////////////////////////////////////////////////////////////////////////////// 

    var toggler = function( property, value ){
        return function(thing,i){
            thing.set(property,value);
        };
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
    var hasNameConflict = function(trackkey) {
        if( trackkey == '' ){ 
            alert("Filename is empty");
            return 1; 
        }
        else{
            for( tk in brwsr.tracks ){
                if( trackkey == brwsr.tracks[tk] ){
                    alert("There is already a track with that name");
                    return 2;
                 }
            }
        }
        return 0;
        //compared to ?? (other tracks)
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //            GenomeView creator setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////
   
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
            alert("Dont' want to have to do return a trackListCreate here");
            //return trackListCreate(track, hint);
        } 
        else {
            var replaceData = {refseq: brwsr.refSeq.name, 
                               assembly: "42"};
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

//this.trackListWidget.insertNodes(false, params.trackData);
    var oldTrackList = dojo.cookie(this.container.id + "-tracks");
    if (params.tracks) {
        this.showTracks(params.tracks);
    } else if (oldTrackList) {
        this.showTracks(oldTrackList);
    } else if (params.defaultTracks) {
        this.showTracks(params.defaultTracks);
    }

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

                for (var i = 0; i < this.chromList.options.length; i++){
                    if (this.chromList.options[i].text == refName){
                        this.chromList.selectedIndex = i;
                    }
                }
                this.refSeq = this.allRefs[refName];
                this.interestingAreas = new InterestingAreas( this.refSeq.start, this.refSeq.end );
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
        //this.trackListWidget.forInItems(function(obj, id, map) {
        var anon = function(entry,i) {
            if (trackNames[n] == entry.label) {
                brwsr.viewDndWidget.insertNodes(false, [entry]);
                removeFromList.push(i);
            }
        };
        dojo.forEach( brwsr.trackData, anon );
    }
    //var movedNode;
    //for (var i = 0; i < removeFromList.length; i++) {
    //this.trackListWidget.delItem(removeFromList[i]);
    //movedNode = dojo.byId(removeFromList[i]);
    //movedNode.parentNode.removeChild(movedNode);
    //}
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
