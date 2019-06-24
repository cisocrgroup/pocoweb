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
    
     
			  projectShowHeader = new Show.Header({title:"Correction Protocol",icon:"fas fa-clipboard-list",color:"red"});
      	projectShowFooterPanel = new Show.FooterPanel();


            var status = project.get('status');
    status="post-corrected";
      if(job.statusName=="running"){
          projectShowProtocol = new Views.LoadingView({title:"Job running",message:job.jobName+ " is running, please wait."});
          projectShowLayout.trackJobStatus();
        }

        else {

          if (status=="empty"||status=="profiled"){
          projectShowProtocol = new Show.SingleStep({url:"le",color:"blue",step:"Postcorrection",icon:"fas fa-history",id:"js-start-pc",text:"Start automated postcorrection"});
          }
          else if (status=="post-corrected"){

               var fetchingprotocol = ProjectEntities.API.getProtocol({pid:id});
                         $.when(fetchingprotocol).done(function(pr){
                              projectShowProtocol = new Show.Protocol({pr});
                              projectShowLayout.showChildView('contentRegion',projectShowProtocol);


        
                      projectShowProtocol.on("show:word_clicked",function(word){
                      
                      var searchingToken = ProjectEntities.API.searchToken({q:word,pid:id,isErrorPattern:true});

                      $.when(searchingToken).done(function(tokens,suggestions){
     

                      var projectConcView = new Show.Concordance({selection:word,tokendata:tokens,asModal:true});

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


              });
           }
          }


    

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
	          projectShowLayout.showChildView('contentRegion',projectShowProtocol);
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
