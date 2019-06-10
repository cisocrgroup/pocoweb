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
            var fetchingle = ProjectEntities.API.getLexiconExtension({pid:id});
            var fetchinguser = UserEntities.API.loginCheck();
                            
   
      $.when(fetchingproject,fetchinglanguages,fetchingle,fetchinguser).done(function(project,languages,le,user){

		  loadingCircleView.destroy();
      console.log(project);
      console.log(le);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
    



     
			  projectShowHeader = new Show.Header({title:"Lexicon Extension",icon:"fas fa-clipboard-list",color:"blue"});
        projectShowInfo = new Show.Info({le:le});
      	projectShowFooterPanel = new Show.FooterPanel();
        console.log(projectShowInfo)
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
