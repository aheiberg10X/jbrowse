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
                        ep.selectChild( "query_bc", true);
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
         onClick: uploadBAM,
         disabled: "disabled"
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

var deleteSubmit_old_drag_and_drop = function(brwsr) {

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

