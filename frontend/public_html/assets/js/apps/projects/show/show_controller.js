// ======================================
// apps/project/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showProject: function(id,page_id){
      		$(window).scrollTop(0);

     		require(["entities/project"], function(ProjectEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
              App.mainLayout.showChildView('backdropRegion',loadingCircleView);

   			  var fetchingpage = ProjectEntitites.API.getPage({pid:id, page:page_id});

   
        	 $.when(fetchingpage).done(function(page){

		     	loadingCircleView.destroy();
            console.log(page);

		 	//currentProposal.set({"url_id":id}); // pass url_id to view..
			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);
	
			projectShowLayout.on("attach",function(){
			  

        // ** to do: get junks from server
        var fetchingallcorrections = ProjectEntitites.API.getAllCorrectionSuggestions({pid:id, page:page_id});
           $.when(fetchingallcorrections).done(function(allsuggestions){
             projectShowPage.setErrorDropdowns(allsuggestions,id);
           });



			  // projectShowHeader = new Show.Header({title:"Project: "+project.get('title')});
        projectShowPage = new Show.Page({model:page});
			  projectShowInfo = new Show.Info({});
      	projectShowFooterPanel = new Show.FooterPanel();

       projectShowPage.on("page:new",function(page_id){
                    var fetchingnewpage = ProjectEntitites.API.getPage({pid:id, page:page_id});
                  $.when(fetchingnewpage).done(function(new_page){
                      projectShowPage.model=new_page
                        projectShowPage.render();    
                     App.navigate("projects/"+id+"/page/"+page_id);
         
                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject
          
       })

    
       projectShowPage.on("page:correct_line",function(data,anchor){

                    var correctingline = ProjectEntitites.API.correctLine(data);
                  $.when(correctingline).done(function(result){
                    
                    $('#line-text-'+anchor).css('background','#d4edda');
                   /*** TO DO 

                                 var fully = res.isFullyCorrected;
                  var partial = res.isPartiallyCorrected;
                  var input = document.getElementById(anchor);
                  if (input !== null) {
                    input.value = res.cor;
                    pcw.setCorrectionStatus(input, fully, partial);
                  }
                  var text = document.getElementById('line-text-' + anchor);
                  if (text !== null) {
                    pcw.setCorrectionStatus(text, fully, partial);
                    text.replaceChild(
                        document.createTextNode(res.cor),
                        text.childNodes[0]);
                    var aapi = Object.create(pcw.Api);
                    aapi.sid = pcw.getSid();
                    aapi.setupForGetSuspiciousWords(ids[0], ids[1], ids[2]);
                    aapi.run(pcw.markSuspiciousWordsInLine);

                   ***/


                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject
          
       })

           projectShowPage.on("page:line_selected",function(selection){
                    var that = this;
                    var searchingToken = ProjectEntitites.API.searchToken({q:selection,p:page_id,pid:id});
                    var gettingCorrectionSuggestions = ProjectEntitites.API.getCorrectionSuggestions({q:selection,pid:id});

                  $.when(searchingToken,gettingCorrectionSuggestions).done(function(token,suggestions){
                   that.editor.extensions[0].button.innerHTML = 'Show concordance of <b>'+ selection+'</b> ('+token.nWords+' occurrences)';
                    
                    that.tokendata = token;

                    $("#dropdown-content").empty();
                     for(i=0;i<suggestions.suggestions.length;i++){
                
                     var s = suggestions.suggestions[i];
                     var content = s.suggestion + " (patts: " + s.ocrPatterns.join(',') + ", dist: " +
                      s.distance + ", weight: " + s.weight.toFixed(2) + ")";
                     $('#dropdown-content').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }

                     $('.dropdown-item').on('click',function(){
                      var split = $(this).text().split(" ");
                      Util.replaceSelectedText(split[0]);
                     })


                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject
          
       })


       projectShowPage.on("page:concordance_clicked",function(selection){

       var gettingCorrectionSuggestions = ProjectEntitites.API.getCorrectionSuggestions({q:this.saved_selection,pid:id});
       var that = this;
         $.when(gettingCorrectionSuggestions).done(function(suggestions){
            var tokendata = that.tokendata;
            console.log(suggestions)
            console.log(tokendata)

           var projectConcView = new Show.Concordance({tokendata:tokendata,asModal:true,suggestions:suggestions.suggestions});
    

           projectConcView.on("conc:destroy:editor",function(){
            projectShowPage.render();
           })

             App.mainLayout.showChildView('dialogRegion',projectConcView);

            
                   
              });

        });
   


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


	          // projectShowLayout.showChildView('headerRegion',projectShowHeader);
	          projectShowLayout.showChildView('infoRegion',projectShowPage);
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