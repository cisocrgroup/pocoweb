// ======================================
// apps/project/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProject: function(id){

     		require(["entities/project"], function(ProjectEntities){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     			  var fetchingpage = ProjectEntities.API.getProject({pid:id});

   
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
                  "name": "Order Profile",
                  "seq": 1,
                  "text": "Start profiling the project.",
                  "url": "projects:list",
              }, 
               {
                  "color": "blue",
                  "icon": "far fa-edit",
                  "id": "doc_button",
                  "name": "Adaptive tokens",
                  "seq": 5,
                  "text": "List adaptive tokens.",
                  "url": "docs:show",
              }, {
                  "color": "red",
                  "icon": "fas fa-copy",
                  "id": "about_btn",
                  "name": "Split",
                  "seq": 4,
                  "text": "Split the project.",
                  "url": "about:home",
          },
                {
                  "color": "green",
                  "icon": "far fa-edit",
                  "id": "delete_button",
                  "name": "Edit",
                  "seq": 5,
                  "text": "Edit the project.",
                  "url": "edit",
              },
               {
                  "color": "blue",
                  "icon": "fas fa-download",
                  "id": "test_btn",
                  "name": "Download",
                  "seq": 1,
                  "text": "Save project files to disk.",
                  "url": "projects:list",
              }, 

               {
                  "color": "red",
                  "icon": "far fa-times-circle",
                  "id": "about_btn",
                  "name": "Delete",
                  "seq": 4,
                  "text": "Delete the project.",
                  "url": "delete",
          }
           
          ];

var cards2 = [
          {
                  "color": "blue",
                  "icon": "fas fa-cogs",
                  "id": "test_btn",
                  "name": "PoCoTo-A",
                  "seq": 1,
                  "text": "Fully automatic OCR postcorrection.",
                  "url": "projects:list",
              }, {
                  "color": "green",
                  "icon": "fas fa-file-signature",
                  "id": "users_button",
                  "name": "PoCoTo-A-I",
                  "seq": 2,
                  "text": "Manual interactive postcorrection tools.",
                  "url": "projects:show_page",
              }
          ]

        var projectShowHub = new Show.Hub({columns:3,cards:cards,currentRoute:"home"});
        var projectShowHub2 = new Show.Hub({columns:2,cards:cards2,currentRoute:"home"});

        projectShowHub.on('cardHub:clicked',function(data){
          if(data.url=="delete"){
             this.trigger("show:delete_clicked");
          }
          if(data.url=="edit"){
             this.trigger("show:edit_clicked");
          }
        });

         projectShowHub2.on('cardHub:clicked',function(data){
          if(data.url=="projects:show_page"){
             App.trigger("projects:show_page",id,'first');
          }
        });

			  projectShowHeader = new Show.Header({title:project.get('title'),icon:"fas fa-book-open",color:"green"});
        projectShowInfo = new Show.Info({model:project});
      	projectShowFooterPanel = new Show.FooterPanel();

    	  projectShowHub.on("show:edit_clicked",function(){


			   var projectsShowEditProject = new Show.ProjectForm({model:project
          , asModal:true,text:"Edit Project",edit_project:true,loading_text:"Update in progress"});


           projectsShowEditProject.on("project:update",function(data){
            project.set(data);

            var puttingProject = ProjectEntities.API.updateProject(id,project);


                 $.when(postingProject).done(function(result){
                  $('.loading_background').fadeOut();

                   $('#projects-modal').modal('toggle');

                    // TO DO
                })


          });


          App.mainLayout.showChildView('dialogRegion',projectsShowEditProject);

          });


       projectShowHub.on('show:delete_clicked',function(){

            var confirmModal = new Show.AreYouSure({title:"Are you sure...",text:"...you want to delete project "+project.get('title')+" ?",id:"deleteModal"})
            App.mainLayout.showChildView('dialogRegion',confirmModal)

            confirmModal.on('delete:confirm',function(){
                  var deletingProject = ProjectEntities.API.deleteProject({pid:id});
                  $('#deleteModal').modal("hide");

                 $.when(deletingProject).done(function(result){
                   App.mainmsg.updateContent("Project "+id+" successfully deleted.",'success');              

                     App.trigger('projects:list');

                 }).fail(function(response){ 
                    App.mainmsg.updateContent(response.responseText,'danger');
                  });    
            })

          });



         projectShowHub.on("show:add_book_clicked",function(methods){


		   var projectsShowAddBook = new Show.ProjectForm({model: new ProjectEntitites.Project(), asModal:true,text:"Add a book to the OCR Project",add_book:true,loading_text:"Adding book"});


       projectsShowAddBook.on("project:addbook_clicked",function(data){
		   var addingBook = ProjectEntities.API.addBook(id,data);


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
            projectShowLayout.showChildView('hubRegion',projectShowHub2);
            projectShowLayout.showChildView('hubRegion2',projectShowHub);

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
