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
      var steps = {  

           "empty": {color:"green",step:" Step 1: Profiling",icon:"fas fa-history",id:"js-profile",text:"Start profiling the document."},
           "profiled": {color:"blue",step:" Step 2: Lexicon Extension",icon:"fas fa-history",id:"js-le",text:"Generate the extended lexicon."},

          //  {
          //         "color": "blue",
          //         "icon": "fas fa-history",
          //         "id": "le_start",
          //         "name": "1. Lexicon Extension",
          //         "seq": 1,
          //         "text": "Start generating the extended lexicon.",
          //         "url": "lexicon_extension_start",
          //     }, 
          //      {
          //         "color": "blue",
          //         "icon": "far fa-edit",
          //         "id": "le_inspect",
          //         "name": "2. Lexicon Extension",
          //         "seq": 2,
          //         "text": "Edit and revise the extended lexicon.",
          //         "url": "lexicon_extension_inspect",
          //     },  {
          //         "color": "red",
          //         "icon": "fas fa-history",
          //         "id": "cor_start",
          //         "name": "3. Automatic Postcorrection",
          //         "seq": 3,
          //         "text": "Start the automatic postcorrection process.",
          //         "url": "cor_start",
          // },
          //       {
          //         "color": "red",
          //         "icon": "fas fa-clipboard-list",
          //         "id": "cor_inspect",
          //         "name": "4. Correction Protocol",
          //         "seq": 4,
          //         "text": "Inspect the post correction results.",
          //         "url": "cor_inspect",
          //     }
           
          };

          console.log(steps)
          console.log(project.get('status'));
        // var projectShowInfo = new Show.Info(steps[project.get('status')]);
        var projectShowInfo = new Show.Info(steps['empty']);

        projectShowInfo.on('show:profile_clicked',function(data){
           var profilingproject = ProjectEntities.API.profileProject({pid:id});

             $.when(profilingproject).done(function(result){
                  

                      var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                       $.when(fetchingjobs).done(function(result){
                              console.log(result);
                             }).fail(function(response){
                                   App.mainmsg.updateContent(response.responseText,'danger');                                                 
                             }); 


                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                   }); 
        });
          // if(data.url=="lexicon_extension_start"){
          //    this.trigger("show:start_lexicon_extension",id);
          // }
          // if(data.url=="cor_inspect"){
          //    App.trigger("projects:protocol",id);
          // }
          //  if(data.url=="cor_start"){
          //    this.trigger("show:start_postcorrection",id);
          // }
         
      

         projectShowInfo.on("show:start_lexicon_extension",function(){
          var confirm_lex_ext = new Show.AreYouSure({title:"Start Lexicon Extension",text:"Begin Lexicon Extension Step?",id:"lexModal"})
          App.mainLayout.showChildView('dialogRegion',confirm_lex_ext);

          confirm_lex_ext.on("option:confirm",function(){
             var startinglexextension = ProjectEntities.API.startLexiconExtension({pid:id});
             $('#lexModal').modal('hide');

               $.when(startinglexextension).done(function(result){
                    App.mainmsg.updateContent(result,'Lexicon Extension started');

                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                   }); 
           });

        });

        projectShowInfo.on("show:start_postcorrection",function(){
          var confirm_cor_ext = new Show.AreYouSure({title:"Start Automatic Postcorrection",text:"Begin Automatic Postcorrection Step?",id:"corModal"})
          App.mainLayout.showChildView('dialogRegion',confirm_cor_ext);

            confirm_cor_ext.on("option:confirm",function(){
             var startingpostcorrection = ProjectEntities.API.startPostcorrection({pid:id});
             $('#corModal').modal('hide');

               $.when(startingpostcorrection).done(function(result){
                    App.mainmsg.updateContent(result,'Postcorrection started');

                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                   }); 
           });

        });

     

			  projectShowHeader = new Show.Header({title:"A-PoCoTo",icon:"fas fa-cogs",color:"blue"});
      	projectShowFooterPanel = new Show.FooterPanel();

      

  
  			// projectPanel = new Show.FooterPanel();


	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
            projectShowLayout.showChildView('hubRegion',projectShowInfo);
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
