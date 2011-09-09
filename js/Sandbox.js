var Sandbox = function(params) {
    dojo.require("dojo.data.ItemFileReadStore"); 
    dojo.require("dijit.Tree");
    dojo.require("dijit.tree.dndSource");
    dojo.require("dijit.tree.TreeStoreModel");
    dojo.require("dojox.data.FileStore");
 
    dojo.addOnLoad( 
        function() { 
            console.log( params.containerID );
            var container = dojo.byId( params.containerID );
            
            var hello_div = document.createElement("div");
            hello_div.id = "hello_div";
            //hello_div.class = " claro ";
            hello_div.innerHTML = "hello";
            container.appendChild( hello_div );

            var fs = new dojox.data.FileStore( 
                        {"id" : "fs",
                         "url" : "../bin/filestore_dojotree.py",
                         "pathAsQueryParam" : "true"}
                     );

            var model = new dijit.tree.ForestStoreModel(
                            {"id" : "model",
                             "store" : fs,
                             "rootId" : "root",
                             "rootLabel" : "root" }
                        );       

            var tree = new dijit.Tree(
                            {"id" : "tree",
                             "model" : model,
                             "dndController" : "dijit.tree.dndSource"}
                       ).placeAt( hello_div ); 
        }
    );

    
 
};
