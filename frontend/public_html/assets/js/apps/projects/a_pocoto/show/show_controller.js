// ======================================
// apps/projects/a_pocoto/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/projects/a_pocoto/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showAPoCoTo: function(id){

     		require(["entities/project","entities/util","entities/users"], function(ProjectEntities,UtilEntities,UserEntities){

	   	    var loadingCircleView = new  Views.LoadingBackdropOpc();
          App.mainLayout.showChildView('backdropRegion',loadingCircleView);
     		  var fetchingproject = ProjectEntities.API.getProject({pid:id});

      $.when(fetchingproject).done(function(project){

		  loadingCircleView.destroy();
      console.log(project);

			var projectShowLayout = new Show.Layout();
			var projectShowHeader;
			var projectShowInfo;
			var projectShowFooterPanel;
			// console.log(reviews);

			projectShowLayout.on("attach",function(){
      // var steps = {

      //      "empty": {color:"green",step:" Step 1: Profiling",icon:"fas fa-history",id:"js-profile",text:"Start profiling the document."},
      //      "profiled": {color:"blue",step:" Step 2: Lexicon Extension",icon:"fas fa-history",id:"js-start-le",text:"Generate the extended lexicon."},

      //     };

             var cards = [
       {
                "color": "red",
                "icon": "fa-play",
                "id": "post_cor_btn",
                "name": "Postcorrection",
                "seq": 3,
                "text": "Run the automated post correction process.",
                "url": "postcorrection"
            },
              {
                "color": "green",
                "icon": "fa-list",
                "id": "le_btn",
                "name": "Lexicon Extension",
                "seq": 2,
                "text": "Generate and modify the extended lexicon.",
                "url": "lexicon_extension"
            }
            ]


        var cards2 = [
              {
                  color: "green",
                  icon: "fas fa-history",
                  id: "test_btn",
                  name: "Order Profile",
                  seq: 1,
                  text: "Re-profile the project.",
                  url: "profile"
            },    
     
             {
                "color": "blue",
                "icon": "fa-server",
                "id": "post_train_btn",
                "name": "Train model",
                "seq": 4,
                "text": "Re-train the post correction model.",
                "url": "train_postcorrection"
            }
              ];

          
         

    var projectsShowHub = new Show.Hub({cards:cards,currentRoute:"a_pocoto"});
    var projectsShowHub2 = new Show.Hub({
                columns: 2,
                cards: cards2,
                currentRoute: "a_pocoto"
              });

    projectsShowHub2.on('cardHub:clicked',function(data){

          if(data.url=="profile"){
             this.trigger("projects:profile",id);
          }

          if(data.url=="train_postcorrection"){
             App.trigger("projects:train_postcorrection",id);
          }
    });

   
     projectsShowHub.on('cardHub:clicked',function(data){

          if(data.url=="postcorrection"){
           App.trigger("projects:postcorrection",id);
          }
         if(data.url=="lexicon_extension"){
             App.trigger("projects:lexicon_extension",id);
          }
         
        });
          var status = project.get('status');
          var breadcrumbs = [
                 {title:"<i class='fas fa-home'></i>",url:"#home"},
                 {title:"Projects",url:"#projects"},
                 {title:project.get("title"),url:"#projects/"+id},
                 {title:"Automatic Postcorrection",url:""}

          ];
			  projectShowHeader = new Show.Header({title:"Automatic Postcorrection",icon:"fas fa-cogs",color:"blue",breadcrumbs:breadcrumbs});
      	projectShowFooterPanel = new Show.FooterPanel({manual:true,title: "Back to Overview <i class='fas fa-book-open'></i>"
    });

         projectShowFooterPanel.on('go:back',function(){
                App.trigger("projects:show",id);
          })

             projectsShowHub2.on("projects:profile", function(id) {
                var profilingproject = ProjectEntities.API.profileProject({
                  pid: id,
                  tokens: []
                });

                $.when(profilingproject)
                  .done(function(result) {
                    console.log(result)
                    var confirmModal = new Show.OkDialog({
                      asModal: true,
                      title: "Profiling started",
                      text: "Profile for project '" + project.get("title") + "' ordered.",
                      id: "profileModal"
                    });
                    App.mainLayout.showChildView("dialogRegion", confirmModal);
                    App.mainmsg.updateContent("Profiling started",'info',true,result.request_url);

                       var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                      $.when(fetchingjobs).done(function(jobs) {
                        projectShowLayout.trackJobStatus();

                      });

                  })
                  .fail(function(response) {
                    Util.defaultErrorHandling(response, "danger");
                  });
              });


  			// projectPanel = new Show.FooterPanel();

	          projectShowLayout.showChildView('headerRegion',projectShowHeader);
            projectShowLayout.showChildView("hubRegion", projectsShowHub);
            projectShowLayout.showChildView("hubRegion2", projectsShowHub2);
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
