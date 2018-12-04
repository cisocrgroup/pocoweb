// =================================
// apps/home/show/home_controller.js
// =================================

define(["app","common/util","apps/home/show/show_view","apps/header/show/show_view"], function(App,Util,Home,Header){


 var Controller = {

 	showHome: function(){
         $(window).scrollTop(0);

            
		var homeHomeLayout = new Home.Layout();
		var homeHomeHeader = new Home.Header();


        var cards = [
        {
                "color": "green",
                "icon": "fa-align-left",
                "id": "test_btn",
                "name": "OCR Projects",
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

                homeHomeLayout.showChildView('headerRegion',homeHomeHeader);
                homeHomeLayout.showChildView('hubRegion',homeHomeHub);
	
      
 		}); // on:show



         App.mainLayout.showChildView('mainRegion',homeHomeLayout);




	}
 }

return Controller;

});
