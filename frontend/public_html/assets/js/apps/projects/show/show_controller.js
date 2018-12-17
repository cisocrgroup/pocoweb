// ======================================
// apps/project/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProject: function(id){
      		$(window).scrollTop(0);

     		require(["entities/project"], function(ProjectEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
              App.mainLayout.showChildView('backdropRegion',loadingCircleView);

   			  var fetchingproject = ProjectEntitites.API.getProject(id);


        	 $.when(fetchingproject).done(function(project){

			loadingCircleView.destroy();

		 	//currentProposal.set({"url_id":id}); // pass url_id to view..
			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);
	
			projectShowLayout.on("attach",function(){
			  

			  projectShowHeader = new Show.Header({title:"OCR Project: "+project.get('title')});
			  projectShowInfo = new Show.Info({model:project});
      		  projectShowFooterPanel = new Show.FooterPanel();
      		  console.log(project)

			  projectShowInfo.on("show:edit_clicked",function(methods){


			   var projectsShowEditProject = new Show.ProjectForm({model:project
          , asModal:true,text:"Edit OCR Project",edit_project:true,loading_text:"Update in progress"});


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

			   var projectsShowDeleteProject = new Show.DeleteProjectForm({asModal:true,text:"Remove this Project?",title:"Delete OCR Project"});


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


 			     // loadingCircleView.destroy();
				  // var errortext = Util.getErrorText(response);
      //             var errorView = new List.Error({model: currentUser,errortext:errortext})

      //             errorView.on("currentProject:loggedIn",function(){
					 //        IPS_App.projectsApp.List.Controller.listprojects();
      //             });

      //             IPS_App.mainLayout.showChildView('mainRegion',errorView);




          });  // $when fetchingproject


    	}) // require
    	
		}

	}


return Controller;

});
