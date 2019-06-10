// ======================================
// apps/project/a_pocoto/protocol/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/protocol/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProtocol: function(id){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     			  var fetchingproject = ProjectEntities.API.getProject({pid:id});
            var fetchinglanguages = UtilEntities.API.getLanguages();
            var fetchingprotocol = ProjectEntities.API.getProtocol({pid:id});
            var fetchinguser = UserEntities.API.loginCheck();
                            
   
      $.when(fetchingproject,fetchinglanguages,fetchingprotocol,fetchinguser).done(function(project,languages,pr,user){

		  loadingCircleView.destroy();
      console.log(project);
      console.log(pr);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
    
     
			  projectShowHeader = new Show.Header({title:"Correction Protocol",icon:"fas fa-clipboard-list",color:"red"});
        projectShowInfo = new Show.Info({pr:pr});
      	projectShowFooterPanel = new Show.FooterPanel();
        console.log(projectShowInfo)


        projectShowInfo.on("show:word_clicked",function(word){
            
            var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:true});
            var gettingCorrectionSuggestions = ProjectEntities.API.getCorrectionSuggestions({q:word,pid:id});

            $.when(searchingToken,gettingCorrectionSuggestions).done(function(tokens,suggestions){
            var tokendata = tokens['matches'][word];
            console.log(suggestions);

            var projectConcView = new Show.Concordance({selection:word,tokendata:tokendata,asModal:true,suggestions:suggestions.suggestions});

            projectConcView.on("concordance:correct_token",function(data,anchor){

               console.log(anchor);
               console.log(data);

                 var correctingtoken = ProjectEntities.API.correctToken(data);
                  $.when(correctingtoken).done(function(result){
                    
                    console.log(result);


                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject
          
           }) // correct token




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
