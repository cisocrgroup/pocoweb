// ======================================
// apps/project/a_pocoto/lexicon_extension/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/lexicon_extension/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showLexiconExtension: function(id){

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
    



     
			  projectShowHeader = new Show.Header({title:"Lexicon Extension",icon:"fas fa-clipboard-list",color:"blue"});
        projectShowInfo = new Show.Info({model:project});
      	projectShowFooterPanel = new Show.FooterPanel();

        projectShowInfo.on("show:word_clicked",function(word){
            

            var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:true});

            $.when(searchingToken).done(function(tokens){
            var tokendata = tokens['matches'][word]

            var projectConcView = new Show.Concordance({selection:word,tokendata:tokendata,asModal:true,suggestions:undefined});
            App.mainLayout.showChildView('dialogRegion',projectConcView);

           });

        });

  
	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
	          projectShowLayout.showChildView('contentRegion',projectShowInfo);
	          projectShowLayout.showChildView('panelRegion',projectShowFooterPanel);


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
