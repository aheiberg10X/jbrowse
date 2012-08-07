var Login = function(containerID){
    var login = this;
    dojo.addOnLoad( function(){
        dojo.require("dojo.cookie");
        dojo.require("dijit.form.ValidationTextBox");
        dojo.require("dijit.form.Button");
        dojo.require("dijit.Dialog");

        //TODO:
        //why no style?
        //http://jorgealbaladejo.com/2011/04/10/workaround-to-dynamically-load-css-resources/
 
        //check if login info stored in cookie
        var user_info = dojo.fromJson( dojo.cookie("login_info") );
        
        //build the login form dialog
        var container = dojo.byId(containerID);

        var login_div = document.createElement("div");
        login_div.id = "login_div";
        container.appendChild( login_div );
        
        var user_name_textbox = new dijit.form.ValidationTextBox(
            {id: "user_name_textbox",
             label: "User Name",
             regExp: '\\w+',
             invalidMessage: 'Only alphanumeric characters' 
            }
        ).placeAt( login_div );

        var passwd_textbox = new dijit.form.ValidationTextBox(
            {id: "passwd_textbox",
             label: "Password",
             regExp: '\\w+',
             invalidMessage: 'Only alphanumeric characters' 
            }
        ).placeAt( login_div );
        
        var login_button = new dijit.form.Button(
            {id: "login_button", 
             label: "Login",
             style: "align-text: left; margin-top: 30px;",
             onClick: function(){
                login.user_name = user_name_textbox.value;
                login.passwd = passwd_textbox.value;
                var valid = login.validate( login.user_name, login.passwd );
                if( valid ){
                    login_dialog.hide();
                    login.showBrowser(); //alert("show browser");
                }
                else{
                    alert("try again");
                }
             }
            }).placeAt( login_div ) ;

        var login_dialog = new dijit.Dialog({
                        id : "login_dialog",
                        title: "Login",
                       // style: "width: 70%;",
                        content: login_div
        });

        if( ! user_info ){
            login_dialog.show();
        }
        else{
           login.user_name = user_info["user_name"];
           login.passwd = user_info["passwd"];
           var valid = login.validate( login.user_name, login.passwd );
           if( valid ){
               login.showBrowser();
           }
           else{
               alert("bad cookie");
               login_dialog.show();
           }
        }
    });
}


Login.prototype.showBrowser = function(){
    var login = this;
   console.log(sprintf( "hello %s", "fagel" ));
   var queryParams = dojo.queryToObject(window.location.search.slice(1));
   var bookmarkCallback = function(brwsr) {
       return window.location.protocol
              + "//" + window.location.host
              + window.location.pathname
              + "?loc=" + brwsr.visibleRegion()
              + "&tracks=" + brwsr.visibleTracks();
   }

//TODO:
//user login interface

    var user_info = {"user_name" : login.user_name,
                     "passwd" : login.passwd};
    dojo.cookie("user_info",
                user_info,
                {expires: 3});

    var args = {task: "getTrackData",
                user_name: login.user_name};
    var url = "bin/track_data.py?" + dojo.objectToQuery(args);
    dojo.xhrGet({
        url: url,
        handleAs: "json",
        load: function(data,args){
            trackInfo = data;
            var b = new Browser({
                           containerID: "GenomeBrowser",
                           refSeqs: refSeqs,
                           trackData: trackInfo,
                           globals: globals,
                           defaultTracks: "DNA,gene,mRNA,noncodingRNA",
                           location: queryParams.loc,
                           tracks: queryParams.tracks,
                           bookmark: bookmarkCallback,
                           dataRoot: "data/"
                       });

        },
        error: function(data,args){
            alert(data);
        }
    });
};


Login.prototype.validate = function(){
    var login = this;
    return login.user_name == "dev";
       
    var args = {"user_name" : login.user_name,
               "passwd" : login.passwd};
    var url = "bin/login.py?" + dojo.objectToQuery( args );
    dojo.xhrPost({
        url: url,
        handleAs: "json",
        load: function(data,args){
            if( data["message"] == "ok" ){
                return true;
            }
            else{
                return false;
            }
        },
        error: function(data,args){
            alert(data);
            return false
        }
    });
};
