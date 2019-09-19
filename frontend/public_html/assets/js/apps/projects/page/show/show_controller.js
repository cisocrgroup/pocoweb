// ======================================
// apps/project/page/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/page/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showPage: function(id,page_id){


     		require(["entities/project"], function(ProjectEntities){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
    			  var fetchingpage = ProjectEntities.API.getPage({pid:id, page:page_id});
            var fetchingproject = ProjectEntities.API.getProject({pid:id});

        	 $.when(fetchingpage,fetchingproject).done(function(page,project){

		     	loadingCircleView.destroy();
            console.log(page);
           var lineheight = App.getLineHeight(id);
           var pagehits = App.getPageHits(id);
           var linenumbers = App.getLineNumbers(id);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowPage;
      var projectShowSidebar;
			var projectShowFooterPanel;

			projectShowLayout.on("attach",function(){


        var fetchingsuspiciouswords = ProjectEntities.API.getSuspiciousWords({pid:id});
        var fetchingerrorpatterns = ProjectEntities.API.getErrorPatterns({pid:id});
        var fetchingcharmap = ProjectEntities.API.getCharmap({pid:id});
           $.when(fetchingsuspiciouswords,fetchingerrorpatterns,fetchingcharmap).done(function(suspicious_words,error_patterns,charmap){

            var suspicious_words_array = [];
            for (word in suspicious_words['counts']) {
               suspicious_words_array.push([word,suspicious_words['counts'][word]]);
            }
            var tab_content_height = $('.tab-content').height();
            var sp_table = $('.suspicious-words').DataTable({
                 "scrollY": '556px',
                  "data":suspicious_words_array,
                  "info":false,
                  "paging": false,
                  "lengthChange": false,
                  "order": [[ 1, "desc" ]]
                });

            projectShowSidebar.sp_table = sp_table;


              $('#suspicious-words_filter input').on('keyup click', function () {
                sp_table.search($(this).val()).draw();
              });
              var rows = $('#suspicious-words_filter').next().find('.row');
              rows[0].remove();
            $('#suspicious-words-container > .loading_background2').fadeOut();

            $('#suspicious-words-container').find('.dataTables_wrapper').removeClass('.container-fluid');

              var error_patterns_array = [];
              for (word in error_patterns['counts']) {
               error_patterns_array.push([word,error_patterns['counts'][word]]);
            }

             var ep_table = $('.error-patterns').DataTable({
                  "scrollY": '556px',
                  "data":error_patterns_array,
                  "info":false,
                  "paging": false,
                  "lengthChange": false,
                  "order": [[ 1, "desc" ]]

                });

            projectShowSidebar.ep_table = ep_table;


              $('#error-patterns_filter input').on('keyup click', function () {
                ep_table.search($(this).val()).draw();
              });

              var rows = $('#error-patterns_filter').next().find('.row');
              rows[0].remove();
             $('#error-patterns-container > .loading_background2').fadeOut();

               console.log(charmap);
			   var data = [];
			   for (var key in charmap.charMap) {
				   data.push([key, charmap.charMap[key]]);
			   }
             var char_table = $('.special-characters').DataTable({
                  "scrollY": '556px',
                  "data": data,//[],//[["a",10],["b",10],["c",10]],
                  "info":false,
                  "paging": false,
                  "lengthChange": false,
                  "order": [[ 0, "asc" ]]

             });

            projectShowSidebar.char_table = char_table;


              $('#special-characters_filter input').on('keyup click', function () {
                char_table.search($(this).val()).draw();
              });

              var rows = $('#special-characters_filter').next().find('.row');
              rows[0].remove();
             $('#special-characters-container > .loading_background2').fadeOut();


           });



			  projectShowHeader = new Show.Header({title:project.get('title')});
        console.log(lineheight)
        projectShowPage = new Show.Page({model:page,lineheight:lineheight,linenumbers:linenumbers});
	    projectShowSidebar = new Show.Sidebar({model:page,project:project,pagehits:pagehits,lineheight:lineheight,linenumbers:linenumbers});
      	projectShowFooterPanel = new Show.FooterPanel();

       projectShowSidebar.on("sidebar:error-patterns-clicked",function(pid,pat){
          projectShowPage.trigger('page:concordance_clicked',pat,1);
       });

       projectShowSidebar.on("sidebar:error-tokens-clicked",function(pid,pat){
         projectShowPage.trigger('page:concordance_clicked',pat,0);
       });

       projectShowSidebar.on("sidebar:special-characters-clicked",function(pid,pat){
         console.log(pat)
          Util.copyStringToClipboard(pat);


       });
       projectShowSidebar.on("sidebar:update_page_hits", function(value) {
         App.setPageHits(id, value);
       });
        projectShowSidebar.on("sidebar:update_line_height",function(value){
          App.setLineHeight(id,value);
        });

        projectShowSidebar.on("sidebar:update_line_numbers",function(value){
          App.setLineNumbers(id,value);
        });


       projectShowSidebar.on("page:new",function(page_id){
        
                    var fetchinglineheight = App.getLineHeight(id);
                    var fetchinglinenumbers = App.getLineNumbers(id);
                    var fetchingnewpage = ProjectEntities.API.getPage({pid:id, page:page_id});
                  $.when(fetchingnewpage,fetchinglineheight,fetchinglinenumbers).done(function(new_page,lineheight,linenumbers){
                     new_page.attributes.title = project.get('title');
                      projectShowPage.model=new_page;

                      projectShowSidebar.options.lineheight=lineheight;
                      projectShowSidebar.options.linenumbers=linenumbers;


                      projectShowPage.render();
                      projectShowSidebar.model = new_page;
                      $('#pageId').text('Page '+new_page.get('pageId'))
                      $('.js-stepforward > a').attr('title','go to previous page #'+new_page.get('nextPageId'));
                      $('.js-stepbackward > a').attr('title','go to next page #'+new_page.get('prevPageId'));

                     App.navigate("projects/"+id+"/page/"+new_page.get('pageId'));

                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject

              });


       projectShowPage.on("page:correct_line",function(data,anchor){
         data.text = Util.escapeAsJSON(data.text);
         console.log(data);
                    var correctingline = ProjectEntities.API.correctLine(data);
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



                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when fetchingproject

       })

           projectShowPage.on("page:line_selected",function(selection){
                    var that = this;
                    var gettingCorrectionSuggestions = ProjectEntities.API.getCorrectionSuggestions({q:selection,pid:id});
                    // var searchingToken = ProjectEntities.API.searchToken({q:selection,p:page_id,pid:id,isErrorPattern:0});

                  $.when(gettingCorrectionSuggestions).done(function(suggestions){


                    // $('#js-concordance').attr('title','Show concordance of <b>'+ selection+'</b> ('+tokens.nWords+' occurrences)');

                    $("#suggestionsDropdown").empty();

                     var suggestions_btn = $('#js-suggestions');

                     $(suggestions_btn).dropdown();

                     for(key in suggestions.suggestions){
                       for (var i=0;i<suggestions.suggestions[key].length;i++){
                        // to do: datatable instead ?
          						   var s = suggestions.suggestions[key][i];
          						   var content = Util.formatSuggestion(s);
                     $('#suggestionsDropdown').append($('<a class="dropdown-item noselect">'+content+"</a>"));
                     }
                   }


                      $('#suggestionsDropdown .dropdown-item').on('click',function(e){
                       e.stopPropagation();
                      var split = $(this).text().split(" ");
                      Util.replaceSelectedText(split[0].trim());
                     })




                  }).fail(function(response){
                     App.mainmsg.updateContent(response.responseText,'danger');
                    });  // $when gettingCorrectionSuggestions

       })


       projectShowPage.on("page:concordance_clicked",function(selection,isErrorPattern){
        console.log(isErrorPattern);
        console.log(selection);
        var searchingToken = ProjectEntities.API.searchToken({q:selection,pid:id,isErrorPattern:isErrorPattern,skip:0,max:App.getPageHits(id)});
        var that = this;
         $.when(searchingToken).done(function(tokens){
          console.log(tokens)
            if(tokens.total==0){
                  return;
            }

          var lineheight = App.getLineHeight(id);

           var projectConcView = new Show.Concordance({isErrorPattern:isErrorPattern,selection:selection,tokendata:tokens,lineheight:lineheight,asModal:true});
           $('.custom-popover').remove();

            projectConcView.on("concordance:correct_token",function(data,anchor,done){
               console.log(anchor);
               console.log(data);
              data.token = Util.escapeAsJSON(data.token);
               console.log(data);
                    var correctingtoken = ProjectEntities.API.correctToken(data);
                  $.when(correctingtoken).done(function(result){

                    console.log(result);

               

                      done();
                       // update lines in background with corrections

                       var gettingLine = ProjectEntities.API.getLine(data);
                      $.when(gettingLine).done(function(line_result){
                        console.log(line_result);

                        var lineanchor = $('#line-'+anchor);
                            if(lineanchor.length>0) {
                                lineanchor.removeClass('line_fully_corrected');
                                lineanchor.removeClass('line_partially_corrected');

                              if(line_result.isFullyCorrected) {
                                lineanchor.addClass('line_fully_corrected');
                              }
                              else if(line_result.isPartiallyCorrected){
                                lineanchor.addClass('line_partially_corrected');
                              }
                           
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

             // build suggestions drop down
             projectConcView.on("concordance:show_suggestions",function(data){

              var gettingCorrectionSuggestions = ProjectEntities.API.getCorrectionSuggestions({q:data.token,pid:id});
               $.when(gettingCorrectionSuggestions).done(function(suggestions){
                  projectConcView.setSuggestionsDropdown(data.dropdowndiv,suggestions.suggestions);
               });
            })

             projectConcView.on("concordance:pagination",function(page_nr){
               var max = App.getPageHits(id);
               var searchingToken = ProjectEntities.API.searchToken({
                 q: selection,
                 pid: id,
                 isErrorPattern: isErrorPattern,
                 skip: (page_nr-1)*max,
                 max: max
               });
               var that = this;
               $.when(searchingToken).done(function(tokens){
                 that.options.tokendata = tokens;
                 that.setImages(max);
                 that.setContent(false);
               });
            })

              projectConcView.on("concordance:update_after_correction",function(data){
               var max = App.getPageHits(id);
               var searchingToken = ProjectEntities.API.searchToken({
                 q: data.query,
                 pid: data.pid,
                 isErrorPattern: isErrorPattern,
                 skip:0,
                 max: max
               });
               var that = this;
               $.when(searchingToken).done(function(tokens){

                if(tokens.total==0){
                  $('#conc-modal').modal('hide');
                }
                else{
                 that.options.tokendata = tokens;
                 that.render();
                 $(".js-global-correction-suggestion").val(data.current_input);
                 that.setContent(false);
               }
               });
            })

         projectConcView.on("concordance:jump_to_page",function(data){
           
                  $('#conc-modal').modal('hide');

                   projectShowSidebar.trigger("page:new",data.pageId);


                  setTimeout(function() {
                  var lineanchor = document.getElementById('line-anchor-'+data.pid+"-"+data.pageId+"-"+data.lineId);
                  lineanchor.scrollIntoView();
                  }, 500);

          
               });
            

             App.mainLayout.showChildView('dialogRegion',projectConcView);



              });

        });



          projectShowFooterPanel.on("go:back",function(){
            App.trigger("projects:show",id);
          });





	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
	          projectShowLayout.showChildView('pageRegion',projectShowPage);
            projectShowLayout.showChildView('sidebarRegion',projectShowSidebar);

	          projectShowLayout.showChildView('footerRegion',projectShowFooterPanel);


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
