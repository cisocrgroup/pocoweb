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
            var fetchingjobs = ProjectEntities.API.getJobs({pid:id});



   
      $.when(fetchingproject,fetchinglanguages,fetchingprojects,fetchinguser,fetchingjobs).done(function(project,languages,projects,user,job){

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
           "profiled": {color:"blue",step:" Step 2: Lexicon Extension",icon:"fas fa-history",id:"js-start-le",text:"Generate the extended lexicon."},
           
          };

        var status = project.get('status');
         // status="empty";
        var projectShowInfo = new Show.Info(steps["empty"]);

        console.log(job);

        if(job.statusName=="running"){
          projectShowInfo = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
          projectShowLayout.trackJobStatus();
        }

        else {

          if (status=="empty"||status=="profiled"){
          projectShowInfo = new Show.Info(steps[status]);
          }
          else if (status=="extended-lexicon"){

              var fetchingle = ProjectEntities.API.getLexiconExtension({pid:id});
               $.when(fetchingle).done(function(le){
                    var projectShowLex = new Show.LexiconExtension({le});

                        projectShowLex.on("show:word_clicked",function(word){
            
                        var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:true});

                        $.when(searchingToken).done(function(tokens){

                        var projectConcView = new Show.Concordance({selection:word,tokendata:tokens,asModal:true,le:true});
                        App.mainLayout.showChildView('dialogRegion',projectConcView);

                       });

                    });

                      projectShowLex.on("show:postcorrect_clicked",function(extensions){
                      
                          var postcorrectingproject = ProjectEntities.API.startPostcorrection({pid:project.get('projectId'),bid:project.get('bookId'),extensions:extensions});

                              $.when(postcorrectingproject).done(function(result){

                                    var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                                     $.when(fetchingjobs).done(function(job){

                                            if(job.statusName=="running"){
                                                var profileloading = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
                                                projectShowLayout.showChildView('hubRegion',profileloading);
                                                projectShowLayout.trackJobStatus();
                                              }

                                      }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                                      }); 
                               }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                               });           

                      });

                    projectShowLayout.showChildView('hubRegion',projectShowLex);
              });
          }
          else if (status=="post-corrected"){
             var fetchingprotocol = ProjectEntities.API.getProtocol({pid:id});
                         $.when(fetchingprotocol).done(function(pr){
                              var projectShowProtocol = new Show.Protocol({pr});
                              projectShowLayout.showChildView('hubRegion',projectShowProtocol);
                        });
          }

        }


        projectShowInfo.on('show:profile_clicked',function(data){
         
                
                            var profilingproject = ProjectEntities.API.profileProject({pid:id});

                              $.when(profilingproject).done(function(result){

                                    var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                                     $.when(fetchingjobs).done(function(job){

                                            if(job.statusName=="running"){
                                                var profileloading = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
                                                projectShowLayout.showChildView('hubRegion',profileloading);
                                                projectShowLayout.trackJobStatus();

                                              }
                                      }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                                      }); 
                               }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                               });           
                  
        });


        projectShowInfo.on('show:start_le_clicked',function(){

                 var generatingLexicon = ProjectEntities.API.startLexiconExtension({pid:id});

                    $.when(generatingLexicon).done(function(result){
                        console.log(result);
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var profileloading = new Views.LoadingView({title:"Job running",message:job.jobName+ " is generating, please wait."});
                                      projectShowLayout.showChildView('hubRegion',profileloading);
                                      projectShowLayout.trackJobStatus();
                                     }
                            }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                            }); 
                     }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                     });  
                  
        });
      

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

         projectShowLayout.on("show:checkJobStatus",function(){
              var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
               $.when(fetchingjobs).done(function(result){
                                  
                   if(result.statusName=="done"){
                    $('.loading_background3').fadeOut(function(){
                     App.trigger("projects:a_pocoto",id); //reload a_pocoto
                     clearInterval(projectShowLayout.interval); // clear interval when job done
                    })
                  
                   }

               }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');                                                 
               }); 
         });

        // projectShowInfo.on("show:start_postcorrection",function(){
        //   var confirm_cor_ext = new Show.AreYouSure({title:"Start Automatic Postcorrection",text:"Begin Automatic Postcorrection Step?",id:"corModal"})
        //   App.mainLayout.showChildView('dialogRegion',confirm_cor_ext);

        //     confirm_cor_ext.on("option:confirm",function(){
        //      var startingpostcorrection = ProjectEntities.API.startPostcorrection({pid:id});
        //      $('#corModal').modal('hide');

        //        $.when(startingpostcorrection).done(function(result){
        //             App.mainmsg.updateContent(result,'Postcorrection started');

        //            }).fail(function(response){
        //                  App.mainmsg.updateContent(response.responseText,'danger');                                                 
        //            }); 
        //    });

        // });

     

			  projectShowHeader = new Show.Header({title:"A-PoCoTo",icon:"fas fa-cogs",color:"blue"});
      	projectShowFooterPanel = new Show.FooterPanel();

      

  
  			// projectPanel = new Show.FooterPanel();


	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
        if(status=="empty"||status=="profiled") projectShowLayout.showChildView('hubRegion',projectShowInfo);
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
