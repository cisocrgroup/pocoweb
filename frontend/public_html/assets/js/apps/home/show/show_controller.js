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
        var homeHomeMsg = new Home.Message({message:'Welcome to PoCoWeb. Please <a href="#" class="js-login">login</a>',type:'info'});

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
                "icon": "fa-question-circle-o",
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
        var headerLogin = new Login.Form();
         App.mainLayout.showChildView('dialogRegion',headerLogin);
         $('#loginModal').modal();

                 headerLogin.on("login:submit",function(data){
                 $('#loginModal').modal('hide');

                var loggingInUser = UserEntities.API.login(data);


                             $.when(loggingInUser).done(function(result){

                              if(App.getCurrentRoute()=="home"){
                             
                              var newMsg = new Home.Message({message:result,type:'success'});

                              App.mainLayout._regions.mainRegion.currentView.showChildView('msgRegion',newMsg);
                              }

                              console.log(result)

                                // TO DO
                            })

                 console.log(data)
                });

        });




         App.mainLayout.showChildView('mainRegion',homeHomeLayout);


     });

	}
   
 }



return Controller;

});
