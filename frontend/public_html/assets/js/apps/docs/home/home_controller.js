// =================================
// apps/docs/home/home_controller.js
// =================================

define(["app","common/util","apps/docs/home/home_view","apps/header/show/show_view"], function(App,Util,Home,Header){


 var Controller = {

 	showHome: function(){
         $(window).scrollTop(0);

       require(["entities/docs"], function(UserEntities){

        var breadcrumbs = [
             {title:"<i class='fas fa-home'></i>",url:"/#home"},
             {title:"Docmentation",url:""}
         ];

		var docsHomeLayout = new Home.Layout();
		var docsHomeHeader = new Home.Header({breadcrumbs:breadcrumbs});



        var cards = [
        {
                "color": "purple",
                "icon": "fas fa-book-open",
                "id": "show_user_btn",
                "name": "User Manual",
                "seq": 1,
                "text": "Hints on usage and installation.",
                "url": "docs:manual"
            },
          {
                "color": "purple",
                "icon": "fa-route",
                "id": "show_user_btn",
                "name": "Api Specification",
                "seq": 2,
                "text": "Description of backend API routes.",
                "url": "docs:api"
            }
        ]

         


		var docsHomeHub = new Home.Hub({cards:cards,currentRoute:"docs"});
        var docsFooterPanel = new Home.FooterPanel({title: "Back to Home <i class='fas fa-home'></i>",manual:true});


        docsFooterPanel.on("go:back",function(){
         App.trigger("home:portal");
        });

        docsHomeHub.on("cardHub:clicked",function(data){
            App.trigger(data.url);
        })

		docsHomeLayout.on("attach",function(){
                docsHomeLayout.showChildView('headerRegion',docsHomeHeader);
                docsHomeLayout.showChildView('contentRegion',docsHomeHub);
	            docsHomeLayout.showChildView('panelRegion',docsFooterPanel);


 		}); // on:show


         App.mainLayout.showChildView('mainRegion',docsHomeLayout);


     });

	}

 }



return Controller;

});
