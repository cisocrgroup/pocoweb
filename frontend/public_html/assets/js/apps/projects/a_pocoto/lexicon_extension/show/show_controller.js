// ======================================
// apps/project/a_pocoto/lexicon_extension/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/lexicon_extension/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showLexiconExtension: function(id){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
            App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     			  var fetchingproject = ProjectEntities.API.getProject({pid:id});
            var fetchingjobs = ProjectEntities.API.getJobs({pid:id});




      $.when(fetchingproject,fetchingjobs).done(function(project,job){

		  loadingCircleView.destroy();
      console.log(project);
			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowLex = new Show.Layout(); // dummy view..
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){

    var status = project.get('status');
    if(job.statusName=="running"){
          projectShowLoading = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
          projectShowLayout.showChildView('contentRegion',projectShowLoading);
          projectShowLayout.trackJobStatus();

        }

        else {

          if (!status['extended-lexicon']){
          projectShowLex = new Show.SingleStep({url:"le",color:"blue",step:"Lexicon Extension",icon:"fas fa-history",id:"js-start-le",text:"Generate the extended lexicon."});
          projectShowLayout.showChildView('contentRegion',projectShowLex);

          }
          else {

              var fetchingle = ProjectEntities.API.getLexiconExtension({pid:id});
               $.when(fetchingle).done(function(le){
                       projectShowLex = new Show.LexiconExtension({le});

                       projectShowLex.on("show:move_token",function(yes,no){
                      
                         var updatingLE = ProjectEntities.API.putLexiconExtension({
                             pid:project.get('projectId'),
                             bid:project.get('bookId'),
                             yes:yes,
                             no: no
                           });

                             $.when(updatingLE).done(function(le){
                              console.log(le);

                                    // update extension and unknown tables
                                   var extensions_array = [];
                                    for (word in le['yes']) {
                                       extensions_array.push([word,le['yes'][word]+'<div class="table_chevron unknown" style="display:inline-block; float: right; margin-right: 2px;" title="Move to unknown"> <span style="float: right;"><i class="fas fa-chevron-right"></i></span></div>']);
                                    }
                                 
                                      projectShowLex.extensions_table.clear();
                                      projectShowLex.extensions_table.rows.add(extensions_array);
                                      projectShowLex.extensions_table.draw();

                                    var unknown_array = [];
                                    for (word in le['no']) {
                                       unknown_array.push(['<div class="table_chevron" style="display:inline-block;" title="Move to extensions"> <span ><i class="fas fa-chevron-left"></i></span></div>'+word,le['no'][word]]);
                                    }
                                 
                                      projectShowLex.unknown_table.clear();
                                      projectShowLex.unknown_table.rows.add(unknown_array);
                                      projectShowLex.unknown_table.draw();

                             });

                       });

                        projectShowLex.on("show:word_clicked",function(word){

                        var searchingToken = ProjectEntities.API.search({
                          q:word,pid:id,
                          searchType:"token",
                          skip:0,
                          max:App.getPageHits(id)
                        });

                        $.when(searchingToken).done(function(tokens){
                        var lineheight = App.getLineHeight(id);
                        var linenumbers = App.getLineNumbers(id);
                        var confidence_threshold = App.getConfidenceThreshold(id)/10;
                            if(tokens.total==0){
                                var confirmModal = new Show.OkDialog({
                                      asModal: true,
                                      title: "Empty results",
                                      text: "No matches found for token: '" + word + "'",
                                      id: "emptymodal"
                                    });
                                    App.mainLayout.showChildView("dialogRegion", confirmModal);
                                  return;
                            }

                        var projectConcView = new Show.Concordance({selection:word,tokendata:tokens,asModal:true,le:true,lineheight:lineheight,linenumbers:linenumbers,confidence_threshold:confidence_threshold});

                         projectConcView.on("concordance:pagination",function(page_nr){
                                 var max = 9;
                                 var searchingToken = ProjectEntities.API.search({
                                   q: word,
                                   pid: id,
                                   searchType: "token",
                                   skip: (page_nr-1)*max,
                                   max: max
                                 });
                                 var that = this;
                                 $.when(searchingToken).done(function(tokens){
                                   that.options.tokendata = tokens;
                                   that.setImages(max);
                                   that.setContent(false);
                                 });
                           });


                          projectConcView.on("concordance:jump_to_page",function(data){

                              $('#conc-modal').modal('hide');
                              App.trigger("projects:show_page",data.pid,data.pageId,data.lineId);

                           });

                        App.mainLayout.showChildView('dialogRegion',projectConcView);

                       }).fail(function(response){
                              Util.defaultErrorHandling(response,'danger');
                     });

                    });


               // profile with lexicon extensions
             projectShowLex.on("show:start_le_profile_clicked",function(data){
               console.log(data);
               extensions = [];
               for (key in data.yes) {
                 extensions.push(key);
               }
               console.log(extensions);
               var profilingleproject = ProjectEntities.API.profileProject({
                 pid:project.get('projectId'),
                 bid:project.get('bookId'),
                 tokens:extensions
               });
               var updatingLE = ProjectEntities.API.putLexiconExtension({
                 pid:project.get('projectId'),
                 bid:project.get('bookId'),
                 yes: data.yes,
                 no: data.no
               });
               $.when(updatingLE).fail(function(response) {
               Util.defaultErrorHandling(response,'danger');
               });
                              $.when(profilingleproject).done(function(result){

                                    var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                                     $.when(fetchingjobs).done(function(job){

                                            if(job.statusName=="running"){
                                                var profileloading = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
                                                projectShowLayout.showChildView('contentRegion',profileloading);
                                                projectShowLayout.trackJobStatus();
                                              }

                                      }).fail(function(response){
                                      Util.defaultErrorHandling(response,'danger');
                                      });
                               }).fail(function(response){
                                     Util.defaultErrorHandling(response,'danger');
                               });

                      });

                  //re-run el process
                  projectShowLex.on("show:le_redo_clicked",function(){
                     var generatingLexicon = ProjectEntities.API.startLexiconExtension({pid:id});

                    $.when(generatingLexicon).done(function(result){
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
                                      projectShowLayout.showChildView('contentRegion',loadingView);
                                      projectShowLayout.trackJobStatus();
                                     }
                            
                            }).fail(function(response){
                                Util.defaultErrorHandling(response,'danger');
                              });
                     }).fail(function(response){
                            Util.defaultErrorHandling(response,'danger');
                     });

                  });
                  console.log(projectShowLex);
                  projectShowLayout.showChildView('contentRegion',projectShowLex);


            });
           }
          }

           projectShowLex.on('singleStep:clicked',function(data){
            if(data.url=="le"){
              this.trigger("show:start_le_clicked")
            }

           });


                  projectShowLex.on('show:start_le_clicked',function(){

                 var generatingLexicon = ProjectEntities.API.startLexiconExtension({pid:id});

                    $.when(generatingLexicon).done(function(result){
                        console.log(result);
                          var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

                           $.when(fetchingjobs).done(function(job){

                                  if(job.statusName=="running"){
                                      var loadingView = new Views.LoadingView({title:"Job running",message:job.jobName+ " , please wait."});
                                      projectShowLayout.showChildView('contentRegion',loadingView);
                                      projectShowLayout.trackJobStatus();
                                     }

                                  
                            }).fail(function(response){
                               Util.defaultErrorHandling(response,'danger');
                            });
                     }).fail(function(response){
                             Util.defaultErrorHandling(response,'danger');
                     });

        });


        projectShowLayout.on("show:checkJobStatus",function(){
                  var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                   $.when(fetchingjobs).done(function(result){

                       if(result.statusName=="done"){
                        $('.loading_background3').fadeOut(function(){
                         App.trigger("projects:lexicon_extension",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })

                       }

                        else if(result.statusName=="failed"){
                         $('.loading_background3').fadeOut(function(){
                          Util.defaultErrorHandling("Job failed",'danger');
                          App.trigger("projects:lexicon_extension",id); //reload a_pocoto
                          projectShowLayout.stopJobTracking(); // clear interval when job done
                        })
                       }

                        if(result.statusName!="running"){ // stop job tracking if job is not running
                          projectShowLayout.stopJobTracking();
                        }



                   }).fail(function(response){
                         App.mainmsg.updateContent(response.responseText,'danger',true,response.request_url);
                   });
             });
         var breadcrumbs = [
                 {title:"<i class='fas fa-home'></i>",url:"#home"},
                 {title:"Projects",url:"#projects"},
                 {title:project.get("title"),url:"#projects/"+id},
                 {title:"Automatic Postcorrection",url:"#projects/"+id+"/a_pocoto"},
                 {title:"Lexicon Extension",url:""}


          ];
			  projectShowHeader = new Show.Header({title:"Lexicon Extension",icon:"far fa-edit",color:"blue",breadcrumbs:breadcrumbs});
      	projectShowFooterPanel = new Show.FooterPanel({manual:true,title: "Back to A-PoCoTo <i class='fas fa-cogs'></i>"});

	           projectShowLayout.showChildView('headerRegion',projectShowHeader);
	           projectShowLayout.showChildView('panelRegion',projectShowFooterPanel);

                projectShowFooterPanel.on('go:back',function(){
                App.trigger("projects:a_pocoto",id);
               });



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
