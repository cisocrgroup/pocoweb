// ======================================
// apps/projects/a_pocoto/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showAPoCoTo: function(id){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

	   	    var loadingCircleView = new  Views.LoadingBackdropOpc();
          App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     		  var fetchingproject = ProjectEntities.API.getProject({pid:id});

      $.when(fetchingproject).done(function(project){

		  loadingCircleView.destroy();
      console.log(project);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
      // var steps = {

      //      "empty": {color:"green",step:" Step 1: Profiling",icon:"fas fa-history",id:"js-profile",text:"Start profiling the document."},
      //      "profiled": {color:"blue",step:" Step 2: Lexicon Extension",icon:"fas fa-history",id:"js-start-le",text:"Generate the extended lexicon."},

      //     };

             var cards = [
        {
                "color": "green",
                "icon": "fa-list",
                "id": "le_btn",
                "name": "Lexicon Extension",
                "seq": 1,
                "text": "Generate and modify the extended lexicon.",
                "url": "lexicon_extension"
            }, {
                "color": "red",
                "icon": "fa-play",
                "id": "post_cor_btn",
                "name": "Postcorrection",
                "seq": 2,
                "text": "Run the automated post correction process.",
                "url": "postcorrection"
            },
            {
                "color": "blue",
                "icon": "fa-server",
                "id": "post_train_btn",
                "name": "Train model",
                "seq": 2,
                "text": "Re-train the post correction model.",
                "url": "train_postcorrection"
            }
            ]


    var projectsShowHub = new Show.Hub({cards:cards,currentRoute:"a_pocoto"});

     projectsShowHub.on('cardHub:clicked',function(data){

          if(data.url=="postcorrection"){
           App.trigger("projects:postcorrection",id);
          }
          if(data.url=="lexicon_extension"){
             App.trigger("projects:lexicon_extension",id);
          }
          if(data.url=="train_postcorrection"){
             App.trigger("projects:train_postcorrection",id);
          }
        });
        var status = project.get('status');

			  projectShowHeader = new Show.Header({title:"Automatic Postcorrection",icon:"fas fa-cogs",color:"blue"});
      	projectShowFooterPanel = new Show.FooterPanel();




  			// projectPanel = new Show.FooterPanel();


	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
            projectShowLayout.showChildView('hubRegion',projectsShowHub);
	          projectShowLayout.showChildView('footerRegion',projectShowFooterPanel);


    		}); // on.attach()

          App.mainLayout.showChildView('mainRegion',projectShowLayout);

          }).fail(function(response){
               loadingCircleView.destroy();
                Util.defaultErrorHandling(response,'danger');

          });  // $when fetchingproject


    	}) // require

		}

	}


return Controller;

});
