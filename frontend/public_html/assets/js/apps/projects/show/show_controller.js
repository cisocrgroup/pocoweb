// ======================================
// apps/project/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProject: function(id){

     		require(["entities/project"], function(ProjectEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     			  var fetchingpage = ProjectEntitites.API.getPage({pid:id, page:1});

   
      $.when(fetchingpage).done(function(project){

		     	loadingCircleView.destroy();
      console.log(project);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);
	
			projectShowLayout.on("attach",function(){
      var cards = [
          {
                  "color": "green",
                  "icon": "fas fa-history",
                  "id": "test_btn",
                  "name": "Automatic Postcorrection",
                  "seq": 1,
                  "text": "Start automatic postcorrection process.",
                  "url": "projects:list",
              }, {
                  "color": "green",
                  "icon": "fas fa-file-signature",
                  "id": "users_button",
                  "name": "Interactive Postcorrection",
                  "seq": 3,
                  "text": "Manually correct pages.",
                  "url": "projects:show_page",
              },
               {
                  "color": "green",
                  "icon": "far fa-edit",
                  "id": "doc_button",
                  "name": "Edit",
                  "seq": 5,
                  "text": "Edit the project.",
                  "url": "docs:show",
              }, {
                  "color": "green",
                  "icon": "far fa-times-circle",
                  "id": "about_btn",
                  "name": "Delete",
                  "seq": 4,
                  "text": "Delete the project.",
                  "url": "about:home",
          }]


        var projectShowInfo = new Show.Hub({cards:cards,currentRoute:"home"});

        projectShowInfo.on('cardHub:clicked',function(data){
          if(data.url=="projects:show_page"){
             App.trigger("projects:show_page",id,1);
          }
        });

			  projectShowHeader = new Show.Header({title:"Project "+project.get('projectId'),icon:"fas fa-book-open",color:"green"});
      	projectShowFooterPanel = new Show.FooterPanel();

    	  projectShowInfo.on("show:edit_clicked",function(methods){


			   var projectsShowEditProject = new Show.ProjectForm({model:project
          , asModal:true,text:"Edit Project",edit_project:true,loading_text:"Update in progress"});


           projectsShowEditProject.on("project:update_clicked",function(data){
            project.set(data)

            var puttingProject = ProjectEntitites.API.updateProject(id,project);


                 $.when(postingProject).done(function(result){
                  $('.loading_background').fadeOut();

                   $('#projects-modal').modal('toggle');

                    // TO DO
                })


          });


          App.mainLayout.showChildView('dialogRegion',projectsShowEditProject);

          });


            projectShowInfo.on("show:delete_clicked",function(methods){

			   var projectsShowDeleteProject = new Show.DeleteProjectForm({asModal:true,text:"Remove this Project?",title:"Delete Project"});


        	   projectsShowDeleteProject.on("project:delete_clicked",function(){
               var deletingProject = ProjectEntitites.API.deleteProject(id);


                 $.when(deletingProject).done(function(result){
                  $('.loading_background').fadeOut();

                   $('#confirm-modal').modal('toggle');


                   	App.trigger("projects:list");   


                   projectsShowDeleteProject.model.clear().set(projectsListDeleteProject.model.defaults);
                   $('#selected_file').text("");
                   // projectsListAddProject.render()

                })


          });

        

          App.mainLayout.showChildView('dialogRegion',projectsShowDeleteProject);



		  });


         projectShowInfo.on("show:add_book_clicked",function(methods){


		   var projectsShowAddBook = new Show.ProjectForm({model: new ProjectEntitites.Project(), asModal:true,text:"Add a book to the OCR Project",add_book:true,loading_text:"Adding book"});


       projectsShowAddBook.on("project:addbook_clicked",function(data){
		   var addingBook = ProjectEntitites.API.addBook(id,data);


		         $.when(addingBook).done(function(result){
		          $('.loading_background').fadeOut();

		           $('#projects-modal').modal('toggle');


		           projectsShowAddBook.model.clear().set(projectsListEditProject.model.defaults);
		           $('#selected_file').text("");
		           // projectsListAddProject.render()

		        })


      });


          App.mainLayout.showChildView('dialogRegion',projectsShowAddBook);

          });


  			// projectPanel = new Show.FooterPanel();


	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
	          projectShowLayout.showChildView('infoRegion',projectShowInfo);
	          projectShowLayout.showChildView('footerRegion',projectShowFooterPanel);


    		}); // on.attach()

          App.mainLayout.showChildView('mainRegion',projectShowLayout);

          }).fail(function(response){
                App.mainmsg.updateContent(response.responseText,'danger');
          });  // $when fetchingproject


    	}) // require
    	
		}

	}


return Controller;

});
