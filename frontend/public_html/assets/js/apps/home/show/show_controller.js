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
        var homeHomeMsg = new Home.Message({message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>.',type:'info'});
        App.homeMsg = homeHomeMsg;

        var cards = [
        {
                "color": "green",
                "icon": "fa-align-left",
                "id": "test_btn",
                "name": "Projects",
                "seq": 1,
                "text": "Test pages for OCR projects.",
                "url": "projects:list",
            }, {
                "color": "blue",
                "icon": "fa-book",
                "id": "doc_button",
                "name": "Api-Documentation",
                "seq": 3,
                "text": "Documentation of API-routes.",
                "url": "docs:show",
            }, {
                "color": "purple",
                "icon": "far fa-question-circle",
                "id": "about_btn",
                "name": "About",
                "seq": 4,
                "text": "About this project.",
                "url": "about:home",
        }]


		var homeHomeHub = new Home.Hub({cards:cards,currentRoute:"home"});

        homeHomeHub.on("cardHub:clicked",function(data){
            App.trigger(data.url);
        })       

		homeHomeLayout.on("attach",function(){
                homeHomeLayout.showChildView('msgRegion',homeHomeMsg);
                homeHomeLayout.showChildView('headerRegion',homeHomeHeader);
                homeHomeLayout.showChildView('hubRegion',homeHomeHub);
	
      
 		}); // on:show

      homeHomeMsg.on("msg:login",function(data){
        App.Navbar.trigger("nav:login");
        });

      homeHomeMsg.on("msg:logout",function(data){
        App.Navbar.trigger("nav:logout");
        });


         App.mainLayout.showChildView('mainRegion',homeHomeLayout);


     });

	}
   
 }



return Controller;

});
