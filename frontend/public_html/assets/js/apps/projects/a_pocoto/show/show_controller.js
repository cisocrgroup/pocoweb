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
            var fetchinglanguages = UtilEntities.API.getLanguages();
            var fetchingprojects = ProjectEntities.API.getProjects();
            var fetchinguser = UserEntities.API.loginCheck();

   
      $.when(fetchingproject,fetchinglanguages,fetchingprojects,fetchinguser).done(function(project,languages,projects,user){

		  loadingCircleView.destroy();
      console.log(project);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
      var cards = [         
           {
                  "color": "blue",
                  "icon": "fas fa-history",
                  "id": "le_start",
                  "name": "1. Lexicon Extension",
                  "seq": 1,
                  "text": "Start generating the extended lexicon.",
                  "url": "lexicon_extension_start",
              }, 
               {
                  "color": "blue",
                  "icon": "far fa-edit",
                  "id": "le_inspect",
                  "name": "2. Lexicon Extension",
                  "seq": 2,
                  "text": "Edit and revise the extended lexicon.",
                  "url": "lexicon_extension_inspect",
              },  {
                  "color": "red",
                  "icon": "fas fa-history",
                  "id": "cor_start",
                  "name": "3. Automatic Postcorrection",
                  "seq": 3,
                  "text": "Start the automatic postcorrection process.",
                  "url": "postcorrection_start",
          },
                {
                  "color": "red",
                  "icon": "fas fa-clipboard-list",
                  "id": "cor_inspect",
                  "name": "4. Correction Protocoll",
                  "seq": 4,
                  "text": "Inspect the post correction results.",
                  "url": "edit",
              }
           
          ];


        var projectShowHub = new Show.Hub({columns:2,cards:cards,currentRoute:"home"});

        projectShowHub.on('cardHub:clicked',function(data){
          if(data.url=="lexicon_extension_inspect"){
             App.trigger("projects:lexicon_extension",id);
          }
          if(data.url=="postcorrection_inspect"){
             App.trigger("projects:postcorrection",id);
          }
        
        });

     

			  projectShowHeader = new Show.Header({title:"A-PoCoTo",icon:"fas fa-cogs",color:"blue"});
      	projectShowFooterPanel = new Show.FooterPanel();

      
      
  
  			// projectPanel = new Show.FooterPanel();


	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
            projectShowLayout.showChildView('hubRegion',projectShowHub);
	          projectShowLayout.showChildView('footerRegion',projectShowFooterPanel);


    		}); // on.attach()

          App.mainLayout.showChildView('mainRegion',projectShowLayout);

          }).fail(function(response){
               loadingCircleView.destroy();
                App.mainmsg.updateContent(response.responseText,'danger');
          });  // $when fetchingproject


    	}) // require
    	
		}

	}


return Controller;

});
