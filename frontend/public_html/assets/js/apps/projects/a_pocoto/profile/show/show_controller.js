// ======================================
// apps/project/a_pocoto/profile/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/profile/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProfile: function(id){

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
			var projectShowProfile = new Show.Layout(); // dummy view..
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
          projectShowProfile = new Show.SingleStep({url:"profile",color:"green",step:"Order Profile",icon:"fas fa-history",id:"js-start-profile",text:"Re-profile the project."});
          projectShowLayout.showChildView('contentRegion',projectShowProfile);
        }

           projectShowProfile.on('singleStep:clicked',function(data){
            if(data.url=="profile"){
              this.trigger("show:start_profile_clicked")
            }

           });


                  projectShowProfile.on('show:start_profile_clicked',function(){


              //          projectsShowHub2.on("projects:profile", function(id) {
              //   var profilingproject = ProjectEntities.API.profileProject({
              //     pid: id,
              //     tokens: []
              //   });

              //   $.when(profilingproject)
              //     .done(function(result) {
              //       console.log(result)
              //       var confirmModal = new Show.OkDialog({
              //         asModal: true,
              //         title: "Profiling started",
              //         text: "Profile for project '" + project.get("title") + "' ordered.",
              //         id: "profileModal"
              //       });
              //       App.mainLayout.showChildView("dialogRegion", confirmModal);
              //       App.mainmsg.updateContent("Profiling started",'info',true,result.request_url);

              //          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
              //         $.when(fetchingjobs).done(function(jobs) {
              //           projectShowLayout.trackJobStatus();

              //         });

              //     })
              //     .fail(function(response) {
              //       Util.defaultErrorHandling(response, "danger");
              //     });
              // });




                  var profilingproject = ProjectEntities.API.profileProject({
                  pid: id,
                  tokens: []
                });;

                    $.when(profilingproject).done(function(result){
                        console.log(result);
                       App.mainmsg.updateContent("Profiling started",'info',true,result.request_url);

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
                         App.trigger("projects:profile",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })

                       }

                        else if(result.statusName=="failed"){
                         $('.loading_background3').fadeOut(function(){
                          Util.defaultErrorHandling("Job failed",'danger');
                          App.trigger("projects:profile",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })
                       }

                        if(result.statusName!="running"){ // stop job tracking if job is not running
                          projectShowLayout.stopJobTracking();
                        }



                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger',true,response.request_url);
                   });
             });
         var breadcrumbs = [
                 {title:"<i class='fas fa-home'></i>",url:"#home"},
                 {title:"Projects",url:"#projects"},
                 {title:project.get("title"),url:"#projects/"+id},
                 {title:"Automatic Postcorrection",url:"#projects/"+id+"/a_pocoto"},
                 {title:"Profile",url:""}


          ];
			  projectShowHeader = new Show.Header({title:"Profile",icon:"fas fa-poll",color:"green",breadcrumbs:breadcrumbs});
      	projectShowFooterPanel = new Show.FooterPanel({manual:true,title: "Back to A-PoCoTo <i class='fas fa-cogs'></i>"});

	           projectShowLayout.showChildView('headerRegion',projectShowHeader);
	           projectShowLayout.showChildView('panelRegion',projectShowFooterPanel);

                projectShowFooterPanel.on('go:back',function(){
                App.trigger("projects:a_pocoto",id);
               });



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
