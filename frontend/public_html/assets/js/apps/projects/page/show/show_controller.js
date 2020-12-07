// ======================================
// apps/project/page/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/page/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showPage: function(id,page_id,line_id){

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
           var hidecorrections = App.getHideCorrections(id);
           var ignore_case = App.getIgnoreCase(id);
           var confidence_threshold = App.getConfidenceThreshold(id);

           var breadcrumbs = [
                 {title:"<i class='fas fa-home'></i>",url:"#home"},
                 {title:"Projects",url:"#projects"},
                 {title:project.get("title"),url:"#projects/"+id},
                 {title:"Manual Postcorrection",url:""},
                 {title: page.get('pageId'),url:""},

          ];

			var projectShowLayout = new Show.Layout({breadcrumbs:breadcrumbs});
			var projectShowPage;
      var projectShowSidebar;
			var projectShowFooterPanel;

			projectShowLayout.on("attach",function(){

        var sidebar_height = window.innerHeight-350;

        var fetchingsuspiciouswords = ProjectEntities.API.getSuspiciousWords({pid:id});
        var fetchingerrorpatterns = ProjectEntities.API.getErrorPatterns({pid:id});
        var fetchingcharmap = ProjectEntities.API.getCharmap({pid:id});
           $.when(fetchingsuspiciouswords,fetchingerrorpatterns,fetchingcharmap).done(function(suspicious_words,error_patterns,charmap){

            var suspicious_words_array = [];
            for (word in suspicious_words['counts']) {
               suspicious_words_array.push([word,suspicious_words['counts'][word]]);
            }
            var sp_table = $('.suspicious-words').DataTable({
                 "scrollY": sidebar_height,
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
                  "scrollY": sidebar_height,
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

			   var data = [];
			   for (var key in charmap.charMap) {
				   data.push([key, charmap.charMap[key]]);
			   }
             var char_table = $('.special-characters').DataTable({
                  "scrollY": sidebar_height,
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

            $('#sidebar-container').sticky({zIndex:10});
            $('#sidebar-container').on('sticky-bottom-reached', function() { console.log("Bottom reached"); });

           });


        projectShowPage = new Show.Page({model:page,title:project.get('title'),lineheight:lineheight,linenumbers:linenumbers,hidecorrections:hidecorrections,confidence_threshold:confidence_threshold,pid:id});
	      projectShowSidebar = new Show.Sidebar({model:page,project:project,pagehits:pagehits,hidecorrections:hidecorrections,lineheight:lineheight,linenumbers:linenumbers,ignore_case:ignore_case,confidence_threshold:confidence_threshold,pid:id});
      	projectShowFooterPanel = new Show.FooterPanel({manual:true,title: "Back to Overview <i class='fas fa-book-open'></i>"});

        projectShowSidebar.on("header:show-image-clicked",function(){


                var getUrl = window.location;
                var baseUrl = getUrl .protocol + "//" + getUrl.host + "/" + getUrl.pathname.split('/')[1];
                var url = baseUrl + "/" + page.get('imgFile');

                var fullbg = $('<div class="modal fullscreen_modal fade"></div>');
                var cont = $('  <div class="modal-dialog" role="document"></div>');

                fullbg.append(cont);
                $('body').append(fullbg);
                var close = $('<span class="full_screen_close">&times;</span>');
                      var full_img = $('<img src='+url+' class="full_screen_img">');

                cont.on('click',function(){
                  fullbg.modal('hide');
                });

                fullbg.append(close);
                cont.append(full_img);

                Util.fit_to_screen(full_img[0]);

                fullbg.modal();

                $('.modal-backdrop.show').css('opacity','0.85');
                close.on('click',function(){
                  fullbg.modal('hide');
                });

                fullbg.on('hidden.bs.modal', function (e) {
                  $(this).remove();
                });

        });

        projectShowSidebar.on("header:search-clicked",function(){

            let projectShowSearch = new Show.Search();
                    App.mainLayout.showChildView("dialogRegion",projectShowSearch);
                    projectShowSearch.on("search:confirmed", function(data) {
                    $('#searchModal').modal('hide');

                    $('#searchModal').on('hidden.bs.modal', function () {
                          projectShowPage.trigger("page:concordance_clicked",data.token,"token");
                    });

                 

                  });

        });


       projectShowSidebar.on("sidebar:error-patterns-clicked",function(pid,pat){
          projectShowPage.trigger('page:concordance_clicked',pat,"pattern");
       });

       projectShowSidebar.on("sidebar:error-tokens-clicked",function(pid,pat){
         projectShowPage.trigger('page:concordance_clicked',pat,"token");
       });

       projectShowSidebar.on("sidebar:special-characters-clicked",function(pid,pat){
          Util.copyStringToClipboard(pat);
       });
       projectShowSidebar.on("sidebar:update_page_hits", function(value) {
         App.setPageHits(id, value);
       });
        projectShowSidebar.on("sidebar:update_confidence_highlighting", function(value) {
         App.setConfidenceThreshold(id, value);
       });
        projectShowSidebar.on("sidebar:update_line_height",function(value){
          App.setLineHeight(id,value);
        });

        projectShowSidebar.on("sidebar:update_line_numbers",function(value){
          App.setLineNumbers(id,value);
        });
        projectShowSidebar.on("sidebar:update_ignore_case", function(value) {
          App.setIgnoreCase(id, value);
        });
        projectShowSidebar.on("sidebar:update_hide_corrections", function(value) {
          App.setHideCorrections(id, value);
        });
       projectShowSidebar.on("page:new",function(page_id){
                    line_id=null;

                    var loadingCircleView = new Views.LoadingBackdropOpc();
                    App.mainLayout.showChildView("backdropRegion", loadingCircleView);

                    var fetchinglineheight = App.getLineHeight(id);
                    var fetchinglinenumbers = App.getLineNumbers(id);
                    var fetchinghidecorrections = App.getHideCorrections(id);

                    var fetchingnewpage = ProjectEntities.API.getPage({pid:id, page:page_id});

                     $.when(fetchingnewpage,fetchinglineheight,fetchinglinenumbers,fetchinghidecorrections).done(function(new_page,lineheight,linenumbers,hidecorrections){
                      loadingCircleView.destroy();
                      new_page.attributes.title = project.get('title');
                      projectShowPage.model.set(new_page.toJSON());

                      projectShowSidebar.options.lineheight=lineheight;
                      projectShowSidebar.options.linenumbers=linenumbers;

                      projectShowPage.options.lineheight=lineheight;
                      projectShowPage.options.linenumbers=linenumbers;
                      projectShowPage.options.hidecorrections=hidecorrections;

                      projectShowPage.render();
                         projectShowLayout.showChildView('pageRegion',projectShowPage);

                      projectShowSidebar.model = new_page;
                      $('#pageId').text('Page '+new_page.get('pageId'))
                      $('.js-stepforward > a').attr('title','go to previous page #'+new_page.get('nextPageId'));
                      $('.js-stepbackward > a').attr('title','go to next page #'+new_page.get('prevPageId'));

                      App.navigate("projects/"+id+"/page/"+new_page.get('pageId'));

                      $(".breadcrumbs span").last().text(new_page.get('pageId'));

                  

                  }).fail(function(response){
                        Util.defaultErrorHandling(response,'danger');
                    });  // $when fetchingproject

              });

       projectShowPage.on("page:lines_appended",function(){
                if(page_id=="first") page_id = project.get('pageIds')[0];
                if(page_id=="last") page_id = project.get('pageIds')[project.get('pageIds').length-1];
                if(line_id!=null){
                      var container = $('#page-container');
                      var scrollTo = $('#line-anchor-'+id+"-"+page_id+"-"+line_id);

                    container.animate({
                        scrollTop: scrollTo.offset().top - container.offset().top + container.scrollTop()
                    },2000, function() {
                      scrollTo.parent().fadeOut(500).fadeIn(500);
                    });

                }              

       });

       projectShowSidebar.on("page:correct_line_clicked",function(data,anchor){
              var that = this;
              var projectShowAreYouSure = new Views.AreYouSure({title:"Correct whole page",text:"Mark every line on this page as corrected ?",id:"correct_page_modal"})
              projectShowAreYouSure.on("yesClicked",function(){
                  $('#correct_page_modal').modal('hide');
                  $('.line-text').each(function(){

                    var anchor = $(this).attr('anchor');
                    var ids = Util.getIds(anchor);
                    var text = $('#line-'+anchor).find('.line').text().replace(/\s\s+/g, ' ').trim();
                    that.trigger("page:correct_line",{pid:ids[0],page_id:ids[1],line_id:ids[2],text:text},anchor);

                 });
              });
              App.mainLayout.showChildView("dialogRegion", projectShowAreYouSure);

          
       });

       projectShowPage.on("page:correct_line",function(data,anchor){
         data.text = Util.escapeAsJSON(data.text);
                    var correctingline = ProjectEntities.API.correctLine(data);
                  $.when(correctingline).done(function(result){

                    // page.get('lines')[result.lineId-1] = result; // update line array in page.lines
                    page.attributes.lines[result.lineId-1] = result; // update line array in page.lines

                    var lineanchor = $('#line-'+anchor);
                    lineanchor.addClass('manually_corrected');

                          lineanchor.fadeOut(200,function(){
                          lineanchor.fadeIn(200,function(){
                          lineanchor.find('.line-tokens').empty();
                          Util.addAlignedLine(result);
                          lineanchor.find('.line').hide();
                          lineanchor.find('.line-tokens').show();
                      });
                    });



                  }).fail(function(response){
                      Util.defaultErrorHandling(response,'danger');
                    });  // $when fetchingproject

       })

           projectShowPage.on("page:line_selected",function(selection){
                    var that = this;
                    var gettingCorrectionSuggestions = ProjectEntities.API.getCorrectionSuggestions({q:selection,pid:id});

                  $.when(gettingCorrectionSuggestions).done(function(suggestions){


                    $("#suggestionsDropdown").empty();

                      if(_.isEmpty(suggestions.suggestions)){
                         $('#suggestionsDropdown').append($('<a class="dropdown-item noselect">No suggestions available</a>"'));
                      }

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

                   if(!_.isEmpty(suggestions.suggestions)){
                      $('#suggestionsDropdown .dropdown-item').on('click',function(e){
                       e.stopPropagation();
                      var split = $(this).text().split(" ");
                      Util.replaceSelectedText(split[0].trim());
                      suggestions_btn.dropdown('toggle')
                       // $('.custom-popover').remove();
                     })
                  }



                  }).fail(function(response){
                      Util.defaultErrorHandling(response,'danger');
                    });  // $when gettingCorrectionSuggestions

       })

       projectShowPage.on("page:concordance_clicked",function(selection,searchType){
         var loadingCircleView = new Views.LoadingBackdropOpc();
        App.mainLayout.showChildView("backdropRegion", loadingCircleView);

        var searchingToken = ProjectEntities.API.search({q:selection,pid:id,searchType:searchType,skip:0,max:App.getPageHits(id)});
        var that = this;
         $.when(searchingToken).done(function(tokens){
          loadingCircleView.destroy();
          $('.custom-popover').hide();
            if(tokens.total==0){
                var confirmModal = new Show.OkDialog({
                      asModal: true,
                      title: "Empty results",
                      text: "No matches or uncorrected instances found for token: '" + selection + "'",
                      id: "emptymodal"
                    });
                    App.mainLayout.showChildView("dialogRegion", confirmModal);
                  return;
            }


          var lineheight = App.getLineHeight(id);
          var linenumbers = App.getLineNumbers(id);

           var projectConcView = new Show.Concordance({searchType:searchType,selection:selection,tokendata:tokens,lineheight:lineheight,linenumbers:linenumbers,confidence_threshold:App.getConfidenceThreshold(id)/10,asModal:true});
           $('.custom-popover').remove();

            projectConcView.on("concordance:correct_token",function(data,anchor,done){

              data.token = Util.escapeAsJSON(data.token);
                    var correctingtoken = ProjectEntities.API.correctToken(data);
                  $.when(correctingtoken).done(function(result){

                      done();

                       // update lines in background with corrections

                       var gettingLine = ProjectEntities.API.getLine(data);
                      $.when(gettingLine).done(function(line_result){


                        var lineanchor = $('#line-'+anchor);
                            if(lineanchor.length>0) {


                              if(line_result.isManuallyCorrected) {
                                lineanchor.addClass('manually_corrected');
                              }



                             page.attributes.lines[line_result.lineId-1] = line_result; // update line array in page.lines
                          
                             lineanchor.find('.line').empty().text(line_result['cor']);
                             lineanchor.find('.line-tokens').empty();
                             Util.addAlignedLine(line_result,App.getConfidenceThreshold(id)/10);

                            lineanchor.find('.line').hide();
                            lineanchor.find('.line-tokens').show();

                            }

                        });

                   // update suspicious words and error patterns in background with corrections

          


                  }).fail(function(response){
                      Util.defaultErrorHandling(response,'danger');
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
               var searchingToken = ProjectEntities.API.search({
                 q: selection,
                 pid: id,
                 searchType: searchType,
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
               var searchingToken = ProjectEntities.API.search({
                 q: data.query,
                 pid: data.pid,
                 searchType: searchType,
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

               var fetchingsuspiciouswords = ProjectEntities.API.getSuspiciousWords({pid:id});
               var fetchingerrorpatterns = ProjectEntities.API.getErrorPatterns({pid:id});

                $.when(fetchingsuspiciouswords,fetchingerrorpatterns).done(function(suspicious_words,error_patterns){

                var suspicious_words_array = [];
                for (word in suspicious_words['counts']) {
                   suspicious_words_array.push([word,suspicious_words['counts'][word]]);
                }

                      projectShowSidebar.sp_table.clear();
                      projectShowSidebar.sp_table.rows.add(suspicious_words_array);
                      projectShowSidebar.sp_table.draw();

                  var error_patterns_array = [];
                  for (word in error_patterns['counts']) {
                   error_patterns_array.push([word,error_patterns['counts'][word]]);
                  }

                      projectShowSidebar.ep_table.clear();
                      projectShowSidebar.ep_table.rows.add(error_patterns_array);
                      projectShowSidebar.ep_table.draw();

               });


            })

         projectConcView.on("concordance:jump_to_page",function(data){

                  $('#conc-modal').modal('hide');

                  App.trigger("projects:show_page",data.pid,data.pageId,data.lineId);


               });


             App.mainLayout.showChildView('dialogRegion',projectConcView);



              });

        });



          projectShowFooterPanel.on("go:back",function(){
            App.trigger("projects:show",id);
          });





	          projectShowLayout.showChildView('pageRegion',projectShowPage);
            projectShowLayout.showChildView('sidebarRegion',projectShowSidebar);

	          // projectShowLayout.showChildView('footerRegion',projectShowFooterPanel);


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
