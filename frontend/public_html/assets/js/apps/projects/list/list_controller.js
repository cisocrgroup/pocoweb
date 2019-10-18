// ======================================
// apps/projects/list/list_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/list/list_view"], function(App,Util,Views,List){


  var Controller = {

 	listProjects: function(){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

          // var loadingCircleView = new  Views.LoadingBackdrop();
          // App.mainLayout.showChildView('backdropRegion',loadingCircleView);


     var fetchingprojects = ProjectEntities.API.getProjects();
     var fetchinglanguages = UtilEntities.API.getLanguages();
     var fetchinguser = UserEntities.API.loginCheck();

		 var projectsListLayout = new List.Layout();

    	 $.when(fetchingprojects,fetchinglanguages,fetchinguser).done(function(projects,languages,user){
        console.log(projects);
        console.log(languages.languages);
        console.log(user)



        // only show projects not packages
        // remember: book = project and project = package :(
        if(projects.books){
         for(var i=0;i<projects.books.length;i++){
          var book = projects.books[i];
          if(book.isBook){
             book['icon'] = '<i class="green fas fa-book-open"></i>';
          }
          else {
            book['icon'] = '<i class="green fas fa-box-open"></i>';
          }
         }
       }

      

    	   projectsListLayout.on("attach",function(){


      var projectsListHeader = new List.Header();
			var projectsListView = new List.ProjectsList({collection: projects.books,hover:true});
      var projectsListPanel = new List.Panel({user: App.getCurrentUser()});
      var projectsListFooterPanel = new List.FooterPanel();

          projectsListLayout.showChildView('headerRegion',projectsListHeader);
          projectsListLayout.showChildView('panelRegion',projectsListPanel);
          projectsListLayout.showChildView('infoRegion',projectsListView);
          projectsListLayout.showChildView('footerRegion',projectsListFooterPanel);



             projectsListView.on('list:profile',function(id){
               var profilingproject = ProjectEntities.API.profileProject({pid:id});

                       $.when(profilingproject).done(function(result){
                            App.mainmsg.updateContent(result,'success');

                       }).fail(function(response){

                             App.mainmsg.updateContent(response.responseText,'danger');

                            });
             });


               projectsListView.on('list:open',function(id){
                App.trigger('projects:show',id);
              });


          projectsListPanel.on("list:create_clicked",function(){


             var projectsListAddProject = new List.ProjectForm({
               model: new ProjectEntities.Project,
               languages:languages.languages,
               asModal:true,
               text:"Create a new project",
               loading_text:"Upload in progress"});

           projectsListAddProject.on("project:submit_clicked",function(data,formdata){
             var uploadingProjectData = ProjectEntities.API.uploadProjectData(formdata);
                 $.when(uploadingProjectData).done(function(result){
                   console.log(result);
                   $('.loading_background').fadeOut();
                   $('#projects-modal').modal('toggle');
                   $('#selected_file').text("");
                   App.mainmsg.updateContent(
                     "Project " + result.projectId + " successfully created (" +
                       result.pages + " pages).",
                     "success"
                   );
                   App.trigger("projects:list");
                }).fail(function(response){
                   $('#projects-modal').modal('hide');
                   App.mainmsg.updateContent(response.responseText,'danger');

                }); // $when uploadingProject




          });
          App.mainLayout.showChildView('dialogRegion',projectsListAddProject);


          })



          projectsListFooterPanel.on("go:back",function(){
            App.trigger("home:portal");
          });




 		}); // onAttach()


       App.mainLayout.showChildView('mainRegion',projectsListLayout);

		}).fail(function(response){

          Util.defaultErrorHandling(response,'danger');

          }); // $when fetchingprojects

		}); // require
	}
 }

return Controller;

});
