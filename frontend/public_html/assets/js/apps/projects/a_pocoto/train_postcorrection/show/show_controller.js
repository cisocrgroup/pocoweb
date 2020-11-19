// ======================================
// apps/project/a_pocoto/train_postcorrection/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/train_postcorrection/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showTrainPostcorrection: function(id){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     			  var fetchingproject = ProjectEntities.API.getProject({pid:id});
            var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
      $.when(fetchingproject,fetchingjobs).done(function(project,job){

		  loadingCircleView.destroy();
      console.log(project);
      console.log(job)

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowTrain = new Show.Layout(); // dummy view..
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){

    var status = project.get('status');
    if(job.statusName=="running"){
          projectShowLoading = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
          projectShowLayout.showChildView('contentRegion',projectShowLoading);
          projectShowLayout.trackJobStatus();

        }

        else {

          projectShowTrain = new Show.SingleStep({url:"train",color:"blue",step:"Start Training",icon:"fas fa-history",id:"js-start-train",text:"Start the process to re-train the post correction model"});
          projectShowLayout.showChildView('contentRegion',projectShowTrain);

          }

           projectShowTrain.on('singleStep:clicked',function(data){
            if(data.url=="train"){
              this.trigger("show:start_train_clicked")
            }

           });


                  projectShowTrain.on('show:start_train_clicked',function(){

                 var generatingLexicon = ProjectEntities.API.startTrainPostcorrection({pid:id});

                    $.when(generatingLexicon).done(function(result){
                        console.log(result);
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
                                      projectShowLayout.showChildView('contentRegion',loadingView);
                                      projectShowLayout.trackJobStatus();
                                     }

                                  
                            }).fail(function(response){
                               Util.defaultErrorHandling(response,'danger');
                            });
                     }).fail(function(response){
                             Util.defaultErrorHandling(response,'danger');
                     });

        });


        projectShowLayout.on("show:checkJobStatus",function(){
                  var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                   $.when(fetchingjobs).done(function(result){

                       if(result.statusName=="done"){
                        $('.loading_background3').fadeOut(function(){
                         App.trigger("projects:train_postcorrection",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })

                       }

                        else if(result.statusName=="failed"){
                         $('.loading_background3').fadeOut(function(){
                          Util.defaultErrorHandling("Job failed",'danger');
                          App.trigger("projects:train_postcorrection",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })
                       }

                        if(result.statusName!="running"){ // stop job tracking if job is not running
                          projectShowLayout.stopJobTracking();
                        }



                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger');
                   });
             });
       var breadcrumbs = [
                 {title:"<i class='fas fa-home'></i>",url:"#home"},
                 {title:"Projects",url:"#projects"},
                 {title:project.get("title"),url:"#projects/"+id},
                 {title:"Automatic Postcorrection",url:"#projects/"+id+"/a_pocoto"},
                 {title:"Re-train Model",url:""}


          ];
			  projectShowHeader = new Show.Header({title:"Re-train post correction model",icon:"fas fa-server",color:"blue",breadcrumbs:breadcrumbs});
      	projectShowFooterPanel = new Show.FooterPanel({manual:true,title: "Back to A-PoCoTo <i class='fas fa-cogs'></i>"});

               projectShowFooterPanel.on('go:back',function(){
                App.trigger("projects:a_pocoto",id);
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
