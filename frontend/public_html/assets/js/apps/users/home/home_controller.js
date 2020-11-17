// =================================
// apps/users/home/home_controller.js
// =================================

define(["app","common/util","apps/users/home/home_view","apps/header/show/show_view","apps/users/login/login_view"], function(App,Util,Home,Header,Login){


 var Controller = {

 	showHome: function(){
         $(window).scrollTop(0);

       require(["entities/users"], function(UserEntities){

        var breadcrumbs = [
             {title:"<i class='fas fa-home'></i>",url:"/"},
             {title:"User Management",url:""}
         ];

		var usersHomeLayout = new Home.Layout();
		var usersHomeHeader = new Home.Header({breadcrumbs:breadcrumbs});



        var cards = [
        {
                "color": "red",
                "icon": "fa-user-circle",
                "id": "show_user_btn",
                "name": "My Account",
                "seq": 1,
                "text": "Show your personal user account.",
                "url": "users:account"
            }, 
        ]

        let user = App.getCurrentUser();
         if (user != null && user.admin) {
           cards.push({
                "color": "red",
                "icon": "fa-users",
                "id": "list_user_btn",
                "name": "List Users",
                "seq": 2,
                "text": "List of all registered users.",
                "url": "users:list"
            });
             cards.push({
                "color": "red",
                "icon": "fa-user-plus",
                "id": "add_user_btn",
                "name": "Create User",
                "seq": 3,
                "text": "Create a new user account.",
                "url": "users:new"
            });
         }


		var usersHomeHub = new Home.Hub({cards:cards,currentRoute:"users"});
        var usersFooterPanel = new Home.FooterPanel({title: "Back to Home <i class='fas fa-home'></i>",manual:true});


        usersFooterPanel.on("go:back",function(){
         App.trigger("home:portal");
        });

        usersHomeHub.on("cardHub:clicked",function(data){
            App.trigger(data.url);
        })

		usersHomeLayout.on("attach",function(){
                usersHomeLayout.showChildView('headerRegion',usersHomeHeader);
                usersHomeLayout.showChildView('contentRegion',usersHomeHub);
	            usersHomeLayout.showChildView('panelRegion',usersFooterPanel);


 		}); // on:show


         App.mainLayout.showChildView('mainRegion',usersHomeLayout);


     });

	}

 }



return Controller;

});
