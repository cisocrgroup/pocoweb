// ======================================
// apps/projects/list/list_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/list/list_view"], function(App,Util,Views,List){


  var Controller = {

 	listProjects: function(){

     		require(["entities/project"], function(ProjectEntitites){

          var loadingCircleView = new  Views.LoadingBackdrop();
          App.mainLayout.showChildView('backdropRegion',loadingCircleView);


     var fetchingprojects = ProjectEntitites.API.getProjects();

		 var projectsListLayout = new List.Layout();

    	 $.when(fetchingprojects).done(function(projects){
		   loadingCircleView.destroy();


    		projectsListLayout.on("attach",function(){


      var projectsListHeader = new List.Header();
			var projectsListView = new List.ProjectsList({collection: projects.projects,hover:true});
      var projectsListPanel = new List.Panel();
      var projectsListFooterPanel = new List.FooterPanel();

     

          projectsListLayout.showChildView('headerRegion',projectsListHeader);
          projectsListLayout.showChildView('panelRegion',projectsListPanel);
          projectsListLayout.showChildView('infoRegion',projectsListView);
          projectsListLayout.showChildView('footerRegion',projectsListFooterPanel);



          $(window).scrollTop(0);


          projectsListPanel.on("list:create_clicked",function(){


             var projectsListAddProject = new List.ProjectForm({model: new ProjectEntitites.Project, asModal:true,text:"Create a new OCR Project",loading_text:"Upload in progress"});


           projectsListAddProject.on("project:submit_clicked",function(data){
           var postingProject = ProjectEntitites.API.createProject(data);


                 $.when(postingProject).done(function(result){
                  $('.loading_background').fadeOut();

                   $('#projects-modal').modal('toggle');
                   console.log(result)
                   App.trigger("projects:show",result.projectId)

                   projectsListAddProject.model.clear().set(projectsListAddProject.model.defaults);
                   $('#selected_file').text("");
                   // projectsListAddProject.render()

                })


          });

          App.mainLayout.showChildView('dialogRegion',projectsListAddProject);


          })

         



 		}); // onAttach()


       App.mainLayout.showChildView('mainRegion',projectsListLayout);

		}).fail(function(response){


 			     // loadingCircleView.destroy();
				  var errortext = Util.getErrorText(response);
                  var errorView = new List.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentProject:loggedIn",function(){
					        App.projectsApp.List.Controller.listprojects();
                  });

                  App.mainLayout.showChildView('mainRegion',errorView);




          }); //  $.when(fetchingAuth).done // $when fetchingprojects

		}); // require
	}
 }

return Controller;

});
