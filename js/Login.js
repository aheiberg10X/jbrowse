var Login = function(containerID){
    var login = this;
    login.containerID = containerID;

    dojo.addOnLoad( function(){
        dojo.addClass(document.body, "tundra");
        dojo.require("dojo.cookie");
        dojo.require("dijit.form.ValidationTextBox");
        dojo.require("dijit.form.Button");
        dojo.require("dijit.Dialog");

        //TODO:
        //why no style?
        //http://jorgealbaladejo.com/2011/04/10/workaround-to-dynamically-load-css-resources/
 
        //check if login info stored in cookie
        var user_name_cookie = dojo.cookie("user_name");
        var passwd_cookie = dojo.cookie("passwd");
        //var user_info = dojo.fromJson( derp );
        
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
                var def = login.validate( login.user_name, 
                                            login.passwd,
                                            false );
                def.then(
                    function(valid){
                        if( valid ){
                            login.login_dialog.hide();
                            //dojo.toJson( {"":,"":}
                            //var user_info =  "user_name:"+%s+";passwd:"%s"
                            //(login.user_name, login.passwd );
                            dojo.cookie("user_name",
                                        login.user_name,
                                        {expires: 3});
                            dojo.cookie("passwd",login.passwd,{expires:3});

                            login.showBrowser(); //alert("show browser");
                        }
                        else{
                            alert("try again");
                        }
                    },
                    function(err){
                        var a = 5;
                    }
                );
             }
            }).placeAt( login_div ) ;

        var register_button = new dijit.form.Button(
            {id: "register_button", 
             label: "Register",
             style: "align-text: left; margin-top: 30px;",
             onClick: function(){
                login.user_name = user_name_textbox.value;
                login.passwd = passwd_textbox.value;
                var def = login.validate( login.user_name, 
                                            login.passwd,
                                            true );
                def.then(
                    function(valid){
                        if( valid ){
                            login.login_dialog.hide();
                            //dojo.toJson( {"":,"":}
                            //var user_info =  "user_name:"+%s+";passwd:"%s"
                            //(login.user_name, login.passwd );
                            dojo.cookie("user_name",
                                        login.user_name,
                                        {expires: 3});
                            dojo.cookie("passwd",login.passwd,{expires:3});

                            login.showBrowser(); //alert("show browser");
                        }
                        else{
                            alert("try again");
                        }
                    },
                    function(err){
                        var a = 5;
                    }
                );

             }
            }).placeAt( login_div ) ;


        login.login_dialog = new dijit.Dialog({
                        id : "login_dialog",
                        title: "Login",
                       // style: "width: 70%;",
                        content: login_div
        });

        if( ! user_name_cookie || ! passwd_cookie ){
            login.login_dialog.show();
        }
        else{
           login.user_name = user_name_cookie;
           login.passwd = passwd_cookie;
           var def = login.validate( login.user_name, login.passwd, false );
           def.then(
               function(valid){
                   if( valid ){
                       login.showBrowser();
                   }
                   else{
                       alert("bad cookie");
                       login.login_dialog.show();
                   }
               },
               function(err){
                   var a = 5;
               }
           );
        }
    });
}


Login.prototype.showBrowser = function(){
   var login = this;

   if( login.brwsr_created ) {
       login.brwsr.container.style.display = "block";
       login.brwsr.refreshUser();
   }
   else {
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


        var args = {task: "getTrackData",
                    user_name: login.user_name};
        var url = "bin/track_data.py?" + dojo.objectToQuery(args);
        dojo.xhrGet({
            url: url,
            handleAs: "json",
            load: function(data,args){
                trackInfo = data;
                var b = new Browser({
                               containerID: login.containerID,
                               refSeqs: refSeqs,
                               trackData: trackInfo,
                               globals: globals,
                               defaultTracks: "DNA,gene,mRNA,noncodingRNA",
                               location: queryParams.loc,
                               tracks: queryParams.tracks,
                               bookmark: bookmarkCallback,
                               dataRoot: "data/"
                           });
                b.login_dialog = login.login_dialog;
                login.brwsr_created = true;
                login.brwsr = b;

            },
            error: function(data,args){
                alert(data);
            }
        });
   }
};

Login.prototype.validate = function( user_name, passwd, is_registering ){
    var login = this;
    //TODO:
    //remove this
    //return login.user_name == "dev";
       
    var args = {"user_name" : user_name,
                "passwd" : passwd,
                "is_registering" : is_registering };
    var url = "bin/login.py?" + dojo.objectToQuery( args );
    var def = dojo.xhrGet({
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
    return def;
    //def.then(
    //function(res){
    //var a = 4;
    //},
    //function(err){
    //}
    //);
};
