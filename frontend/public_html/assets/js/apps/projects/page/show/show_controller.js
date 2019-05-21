// ======================================
// apps/project/page/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showPage: function(id,page_id){

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

       projectShowPage.on("page:error-patterns-clicked",function(pid,pat){
          this.trigger('page:concordance_clicked',pat,1);
       });

       projectShowPage.on("page:error-tokens-clicked",function(pid,pat){
        this.trigger('page:concordance_clicked',pat,0);
       });
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

          console.log(data);

                    var correctingline = ProjectEntitites.API.correctLine(data);
                  $.when(correctingline).done(function(result){
                    
                    console.log(result);
                    var lineanchor = $('#line-'+anchor);
                    lineanchor.addClass('line_fully_corrected');

                 

                    lineanchor.fadeOut(200,function(){
                      lineanchor.fadeIn(200,function(){
                          lineanchor.find('.line-tokens').empty();
                          Util.addAlignedLine(result);
                          lineanchor.find('.line').hide();
                          lineanchor.find('.line-tokens').show();
                      });
                    });
                   

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
                    var gettingCorrectionSuggestions = ProjectEntitites.API.getCorrectionSuggestions({q:selection,pid:id});
                    var searchingToken = ProjectEntitites.API.searchToken({q:selection,p:page_id,pid:id,isErrorPattern:0});

                  $.when(searchingToken,gettingCorrectionSuggestions).done(function(tokens,suggestions){
                    
                    // $('#current_selection').popover({
                    //     container: 'body'
                    //   });
                    that.tokendata = tokens;
                    // $('#js-concordance').html('Show concordance of <b>'+ selection+'</b> ('+tokens.nWords+' occurrences)');

                   $('#js-concordance').attr('title','Show concordance of <b>'+ selection+'</b> ('+tokens.nWords+' occurrences)');

                    $("#suggestionsDropdown").empty();


                var suggestions_btn = $('#js-suggestions'); 
                  
                suggestions_btn.addClass('dropdown');

                suggestions_btn.attr('data-toggle','dropdown');
                suggestions_btn.attr('aria-haspopup','true');
                suggestions_btn.attr('aria-expanded','false');
                suggestions_btn.attr('data-flip','false');
                suggestions_btn.attr('data-target','#suggestionsDropdown');


                 var dropdown_content = $('<div></div>');
                 dropdown_content.addClass('dropdown-menu');
                 dropdown_content.attr('id','suggestionsDropdown');
                 dropdown_content.attr('aria-labelledby','js-suggestions');
                 suggestions_btn.append(dropdown_content);


                     for(i=0;i<suggestions.suggestions.length;i++){
                
                     var s = suggestions.suggestions[i];
                     var content = s.suggestion + " (patts: " + s.ocrPatterns.join(',') + ", dist: " +
                      s.distance + ", weight: " + s.weight.toFixed(2) + ")";
                     $('#suggestionsDropdown').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }

                     $('#js-suggestions').click(function(e){
                      e.stopPropagation();
                      $(".dropdown-menu").hide();
                      $('#suggestionsDropdown').toggle();
                     });

                     $('.dropdown-item').on('click',function(e){
                      e.stopPropagation();
                      var split = $(this).text().split(" ");
                      Util.replaceSelectedText(split[0]);
                     })


                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when gettingCorrectionSuggestions
          
       })


       projectShowPage.on("page:concordance_clicked",function(selection,isErrorPattern){
        console.log(selection);
       var gettingCorrectionSuggestions = ProjectEntitites.API.getCorrectionSuggestions({q:selection,pid:id});
       var searchingToken = ProjectEntitites.API.searchToken({q:selection,pid:id,isErrorPattern:isErrorPattern});
       var that = this;
         $.when(searchingToken,gettingCorrectionSuggestions).done(function(tokens,suggestions){
            var tokendata = tokens;
            console.log(suggestions)
            console.log(tokendata)



           var projectConcView = new Show.Concordance({tokendata:tokendata,asModal:true,suggestions:suggestions.suggestions});
           $('.custom-popover').remove();
        
            projectConcView.on("concordance:correct_token",function(data,anchor){

               console.log(anchor);
               console.log(data);

                    var correctingtoken = ProjectEntitites.API.correctToken(data);
                  $.when(correctingtoken).done(function(result){
                    
                    console.log(result);

                       // update lines in background with corrections

                       var gettingLine = ProjectEntitites.API.getLine(data);
                      $.when(gettingLine).done(function(line_result){
                        console.log(line_result);

                        var lineanchor = $('#line-'+anchor);
                            
                            if(lineanchor.length>0) {
                            lineanchor.removeClass('line_fully_corrected');
                            lineanchor.addClass('line_partially_corrected');
                            console.log("lineanchor")
                            console.log(lineanchor);
                             lineanchor.find('.line').empty().text(line_result['cor']);
                             lineanchor.find('.line-tokens').empty();
                             Util.addAlignedLine(line_result);

                            lineanchor.find('.line').hide();
                            lineanchor.find('.line-tokens').show();
                         
                            }
            
                        });

                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject
          
           }) // correct token


             projectConcView.on("concordance:show_suggestions",function(data,div,anchor){

                    var that = this;
                  
                  $('#dropdown-content-conc').remove();


                var suggestions_btn = div;
                  
                suggestions_btn.addClass('dropdown');

                suggestions_btn.attr('data-toggle','dropdown');
                suggestions_btn.attr('aria-haspopup','true');
                suggestions_btn.attr('aria-expanded','false');
                suggestions_btn.attr('id','dropdownMenuConc');
                suggestions_btn.attr('data-flip','false');
                suggestions_btn.attr('data-target','dropdown-content-conc');

                 var dropdown_content = $('<div></div>');
                 dropdown_content.addClass('dropdown-menu');
                 dropdown_content.attr('id','dropdown-content-conc');
                 dropdown_content.attr('aria-labelledby','dropdownMenuConc');
                 suggestions_btn.append(dropdown_content);


                     for(i=0;i<suggestions.suggestions.length;i++){
                    
                     var s = suggestions.suggestions[i];
                     var content = s.suggestion + " (patts: " + s.ocrPatterns.join(',') + ", dist: " +
                      s.distance + ", weight: " + s.weight.toFixed(2) + ")";
                     $('#dropdown-content-conc').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }
                    dropdown_content.toggle();

                     $('#dropdown-content-conc .dropdown-item').on('click',function(e){
                      e.stopPropagation();
                      var split = $(this).text().split(" ");
                      $(this).parent().parent().prev().text(split[0]);
                      $(this).parent().hide();
                     })


          
       }) // conc



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