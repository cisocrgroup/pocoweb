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
                  "icon": "fas fa-clipboard-list",
                  "id": "test_btn",
                  "name": "Lexicon Extension",
                  "seq": 1,
                  "text": "Edit and revise the lexicon.",
                  "url": "lexicon_extension",
              }, 
               {
                  "color": "blue",
                  "icon": "far fa-edit",
                  "id": "doc_button",
                  "name": "Adaptive tokens",
                  "seq": 5,
                  "text": "List adaptive tokens.",
                  "url": "docs:show",
              }, {
                  "color": "red",
                  "icon": "fas fa-columns",
                  "id": "about_btn",
                  "name": "Split",
                  "seq": 4,
                  "text": "Split the project.",
                  "url": "split",
          },
                {
                  "color": "green",
                  "icon": "far fa-edit",
                  "id": "delete_button",
                  "name": "Edit",
                  "seq": 5,
                  "text": "Edit the project.",
                  "url": "edit",
              },
               {
                  "color": "blue",
                  "icon": "fas fa-download",
                  "id": "test_btn",
                  "name": "Download",
                  "seq": 1,
                  "text": "Save project files to disk.",
                  "url": "download",
              }, 

               {
                  "color": "red",
                  "icon": "far fa-times-circle",
                  "id": "about_btn",
                  "name": "Delete",
                  "seq": 4,
                  "text": "Delete the project.",
                  "url": "delete",
          }
           
          ];


        var projectShowHub = new Show.Hub({columns:3,cards:cards,currentRoute:"home"});

        projectShowHub.on('cardHub:clicked',function(data){
          if(data.url=="lexicon_extension"){
             App.trigger("projects:lexicon_extension",id);
          }
          // if(data.url=="edit"){
          //    this.trigger("show:edit_clicked");
          // }
          // if(data.url=="profile"){
          //    this.trigger("show:profile");
          // }
          //   if(data.url=="split"){
          //    this.trigger("show:split");
          // }
          //   if(data.url=="download"){
          //    this.trigger("show:download");
          // }
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
