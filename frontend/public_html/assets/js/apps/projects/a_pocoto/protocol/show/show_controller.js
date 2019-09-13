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
            var fetchingprotocol = ProjectEntities.API.getProtocol({pid:id});
            var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                            
   
      $.when(fetchingproject,fetchingprotocol,fetchingjobs).done(function(project,pr,job){

		  loadingCircleView.destroy();
      console.log(project);
      console.log(pr);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowProtocol = new Show.Layout() // dummy view;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
    
     
			  projectShowHeader = new Show.Header({title:"Postcorrection",icon:"fas fa-play",color:"red"});
      	projectShowFooterPanel = new Show.FooterPanel();


      var status = project.get('status');
      if(job.statusName=="running"){
          projectShowLoading = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
          projectShowLayout.showChildView('contentRegion',projectShowLoading);
          projectShowLayout.trackJobStatus();
        }

        else {

          if (!status['post-corrected']){
          projectShowProtocol = new Show.SingleStep({url:"pr",color:"red",step:"Postcorrection",icon:"fas fa-play",id:"js-start-pc",text:"Start automated postcorrection"});
          projectShowLayout.showChildView('contentRegion',projectShowProtocol);
          
          }
          else {

               var fetchingprotocol = ProjectEntities.API.getProtocol({pid:id});

                         $.when(fetchingprotocol).done(function(pr){
                              projectShowProtocol = new Show.Protocol({pr});
                              projectShowLayout.showChildView('contentRegion',projectShowProtocol);

        
                      projectShowProtocol.on("show:word_clicked",function(word){
                      



                      var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:false,skip:0,max:App.getPageHits(id)});

                      $.when(searchingToken).done(function(tokens,suggestions){
                      var lineheight = App.getLineHeight(id);


                      var projectConcView = new Show.Concordance({selection:word,tokendata:tokens,asModal:true,lineheight:lineheight});

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

                    projectConcView.on("concordance:show_suggestions",function(data){

                              var gettingCorrectionSuggestions = ProjectEntities.API.getCorrectionSuggestions({q:data.token,pid:id});
                               $.when(gettingCorrectionSuggestions).done(function(suggestions){
                                  projectConcView.setSuggestionsDropdown(data.dropdowndiv,suggestions.suggestions);

                               });

                            });

                    projectConcView.on("concordance:pagination",function(page_nr){
                                 var max = 9;
                                 var searchingToken = ProjectEntities.API.searchToken({
                                   q: word,
                                   pid: id,
                                   isErrorPattern: false,
                                   skip: (page_nr-1)*max,
                                   max: max
                                 });
                                 var that = this;
                                 $.when(searchingToken).done(function(tokens){
                                   that.options.tokendata = tokens;
                                   that.setImages(max);
                                   that.setContent(false);
                                 });
                      });

                      App.mainLayout.showChildView('dialogRegion',projectConcView);

                     });

                  });



                  //re-run pr process
                  projectShowProtocol.on("show:pr_redo_clicked",function(){

                     var runningPostcorrection = ProjectEntities.API.startPostcorrection({bid:project.get('bookId'),pid:id,extensions:["a"]});

                    $.when(runningPostcorrection).done(function(result){
                        console.log(result);
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
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

        projectShowProtocol.on('singleStep:clicked',function(data){
            if(data.url=="pr"){
              this.trigger("show:start_pr_clicked")
            }
        });

        projectShowProtocol.on('show:start_pr_clicked',function(){
         var runningPostcorrection = ProjectEntities.API.startPostcorrection({bid:project.get('bookId'),pid:id,extensions:["a"]});

            $.when(runningPostcorrection).done(function(result){
                console.log(result);
                  var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                   $.when(fetchingjobs).done(function(job){

                          if(job.statusName=="running"){
                              var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
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
                         App.trigger("projects:postcorrection",id); //reload a_pocoto
                         clearInterval(projectShowLayout.interval); // clear interval when job done
                        })
                      
                       }

                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');                                                 
                   }); 
             });
                 
	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
	          projectShowLayout.showChildView('panelRegion',projectShowFooterPanel);


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
