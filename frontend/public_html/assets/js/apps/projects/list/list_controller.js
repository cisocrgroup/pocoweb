// ======================================
// apps/projects/list/list_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/list/list_view"], function(App,Util,Views,List){


  var Controller = {

 	listProjects: function(){

     		require(["entities/project","entities/util"], function(ProjectEntitites,UtilEntitites){

          // var loadingCircleView = new  Views.LoadingBackdrop();
          // App.mainLayout.showChildView('backdropRegion',loadingCircleView);


     var fetchingprojects = ProjectEntitites.API.getProjects();
     var fetchinglanguages = UtilEntitites.API.getLanguages();

		 var projectsListLayout = new List.Layout();

    	 $.when(fetchingprojects,fetchinglanguages).done(function(projects,languages){
        console.log(projects);
        console.log(languages);

		   // loadingCircleView.destroy();


    		projectsListLayout.on("attach",function(){


      var projectsListHeader = new List.Header();
			var projectsListView = new List.ProjectsList({collection: projects.books,hover:true});
      var projectsListPanel = new List.Panel();
      var projectsListFooterPanel = new List.FooterPanel();

     

          projectsListLayout.showChildView('headerRegion',projectsListHeader);
          projectsListLayout.showChildView('panelRegion',projectsListPanel);
          projectsListLayout.showChildView('infoRegion',projectsListView);
          projectsListLayout.showChildView('footerRegion',projectsListFooterPanel);


          $(window).scrollTop(0);


          projectsListPanel.on("list:create_clicked",function(){


             var projectsListAddProject = new List.ProjectForm({model: new ProjectEntitites.Project, asModal:true,text:"Create a new project",loading_text:"Upload in progress"});



           projectsListAddProject.on("project:submit_clicked",function(data,formdata){
           var uploadingProjectData = ProjectEntitites.API.uploadProjectData(formdata);


                 $.when(uploadingProjectData).done(function(result){

                  console.log(result);
                            $('.loading_background').fadeOut();
                           $('#projects-modal').modal('toggle');
                          $('#selected_file').text("");

                          App.mainmsg.updateContent(result,'success');

                      var fetchingnewprojects = ProjectEntitites.API.getProjects();


                       $.when(fetchingnewprojects).done(function(new_projects){
                          projectsListView.options.collection=new_projects.books;
                          projectsListView.render();
                           projectsListAddProject.model.clear().set(projectsListAddProject.model.defaults);
                          

                       });

                  
                

                }).fail(function(response){
                   $('#projects-modal').modal('hide');
                   App.mainmsg.updateContent(response.responseText,'danger');                       
                                    
          }); // $when fetchingprojects


          });
          App.mainLayout.showChildView('dialogRegion',projectsListAddProject);


          })

         




 		}); // onAttach()


       App.mainLayout.showChildView('mainRegion',projectsListLayout);

		}).fail(function(response){
      
       var mainRegion = App.mainLayout.getRegion('mainRegion');
       mainRegion.empty();

         App.mainmsg.updateContent(response.responseText,'danger');                       
                                    
          }); // $when fetchingprojects

		}); // require
	}
 }

return Controller;

});
