// =================================
// apps/home/show/home_controller.js
// =================================

define(["app","common/util","apps/home/show/show_view","apps/header/show/show_view","apps/users/login/login_view"], function(App,Util,Home,Header,Login){


 var Controller = {

 	showHome: function(){
         $(window).scrollTop(0);

       require(["entities/users"], function(UserEntities){

		var homeHomeLayout = new Home.Layout();
		var homeHomeHeader = new Home.Header();

        var cards = [
        {
                "color": "green",
                "icon": "fa-list",
                "id": "test_btn",
                "name": "Projects",
                "seq": 1,
                "text": "Your current OCR - projects.",
                "url": "projects:list"
            }, {
                "color": "purple",
                "icon": "fa-book",
                "id": "doc_button",
                "name": "Documentation",
                "seq": 5,
                "text": "Documentation of PoCoWeb.",
                "url": "docs:home"
            },
        // {
        //         "color": "blue",
        //         "icon": "far fa-question-circle",
        //         "id": "about_btn",
        //         "name": "About",
        //         "seq": 4,
        //         "text": "About this project.",
        //         "url": "about:home"
        // }
        ];
         let user = App.getCurrentUser();
         if (user != null && user.admin) {
           cards.push({
                "color": "red",
                "icon": "fa-users-cog",
                "id": "users_button",
                "name": "User Management",
                "seq": 2,
                "text": "Manage user accounts.",
                "url": "users:home"
           });
         }

		var homeHomeHub = new Home.Hub({cards:cards,currentRoute:"home"});

        homeHomeHub.on("cardHub:clicked",function(data){
            App.trigger(data.url);
        })

		homeHomeLayout.on("attach",function(){
                homeHomeLayout.showChildView('headerRegion',homeHomeHeader);
                homeHomeLayout.showChildView('hubRegion',homeHomeHub);


 		}); // on:show

      

         App.mainLayout.showChildView('mainRegion',homeHomeLayout);


     });

	}

 }



return Controller;

});
