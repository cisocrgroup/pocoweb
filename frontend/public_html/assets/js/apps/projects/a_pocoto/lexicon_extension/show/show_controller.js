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
            var fetchingjobs = ProjectEntities.API.getJobs({pid:id});



   
      $.when(fetchingproject,fetchingjobs).done(function(project,job){

		  loadingCircleView.destroy();
      console.log(project);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowLex = new Show.Layout(); // dummy view..
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
    
    var status = project.get('status');
    if(job.statusName=="running"){
          projectShowLoading = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
          projectShowLayout.showChildView('contentRegion',projectShowLoading);
          projectShowLayout.trackJobStatus();

        }

        else {

          if (!status['extended-lexicon']){
          projectShowLex = new Show.SingleStep({url:"le",color:"blue",step:"Lexicon Extension",icon:"fas fa-history",id:"js-start-le",text:"Generate the extended lexicon."});
          projectShowLayout.showChildView('contentRegion',projectShowLex);

          }
          else {

              var fetchingle = ProjectEntities.API.getLexiconExtension({pid:id});
               $.when(fetchingle).done(function(le){
                       projectShowLex = new Show.LexiconExtension({le});
                       projectShowLayout.showChildView('contentRegion',projectShowLex);

                        projectShowLex.on("show:word_clicked",function(word){
            
                        var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:true});

                        $.when(searchingToken).done(function(tokens){

                        var projectConcView = new Show.Concordance({selection:word,tokendata:tokens,asModal:true,le:true});
                        App.mainLayout.showChildView('dialogRegion',projectConcView);

                       });

                    });


               // profile with lexicon extensions
             projectShowLex.on("show:start_le_profile_clicked",function(extensions){
                          var profilingleproject = ProjectEntities.API.profileWithExtensions({pid:project.get('projectId'),bid:project.get('bookId'),extensions:extensions});

                              $.when(profilingleproject).done(function(result){

                                    var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                                     $.when(fetchingjobs).done(function(job){

                                            if(job.statusName=="running"){
                                                var profileloading = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
                                                projectShowLayout.showChildView('contentRegion',profileloading);
                                                projectShowLayout.trackJobStatus();
                                              }

                                      }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                                      }); 
                               }).fail(function(response){
                                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                               });           

                      });

                  //re-run el process
                  projectShowLex.on("show:le_redo_clicked",function(){
                     var generatingLexicon = ProjectEntities.API.startLexiconExtension({pid:id});

                    $.when(generatingLexicon).done(function(result){
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " is generating, please wait."});
                                      projectShowLayout.showChildView('contentRegion',loadingView);
                                      projectShowLayout.trackJobStatus();
                                     }
                            }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                            }); 
                     }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                     });  

                  });



            });
           }
          }

           projectShowLex.on('singleStep:clicked',function(data){
            if(data.url=="le"){
              this.trigger("show:start_le_clicked")
            }
         
           });


                  projectShowLex.on('show:start_le_clicked',function(){

                 var generatingLexicon = ProjectEntities.API.startLexiconExtension({pid:id});

                    $.when(generatingLexicon).done(function(result){
                        console.log(result);
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " is generating, please wait."});
                                      projectShowLayout.showChildView('contentRegion',loadingView);
                                      projectShowLayout.trackJobStatus();
                                     }
                            }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                            }); 
                     }).fail(function(response){
                               App.mainmsg.updateContent(response.responseText,'danger');                                                 
                     });  
                  
        });


        projectShowLayout.on("show:checkJobStatus",function(){
                  var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                   $.when(fetchingjobs).done(function(result){
                                      
                       if(result.statusName=="done"){
                        $('.loading_background3').fadeOut(function(){
                         App.trigger("projects:lexicon_extension",id); //reload a_pocoto
                         clearInterval(projectShowLayout.interval); // clear interval when job done
                        })
                      
                       }

                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                   }); 
             });
                 
			  projectShowHeader = new Show.Header({title:"Lexicon Extension",icon:"far fa-edit",color:"blue"});
      	projectShowFooterPanel = new Show.FooterPanel();
    
	           projectShowLayout.showChildView('headerRegion',projectShowHeader);
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
