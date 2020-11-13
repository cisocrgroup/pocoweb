// ======================================
// apps/docs/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/docs/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showDocs: function(id){
      		$(window).scrollTop(0);

	   		require(["entities/util"], function(UtilEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
              App.mainLayout.showChildView('backdropRegion',loadingCircleView);


		 	//currentdocs.set({"url_id":id}); // pass url_id to view..

     		var fetchingDocs = UtilEntitites.API.getDocumentation();

	    	$.when(fetchingDocs).done(function(data){



			var docsShowLayout = new Show.Layout();
			var docsShowHeader;
     		var docsShowPanel;
			var docsShowInfo;


			docsShowLayout.on("attach",function(){
			  

			 var breadcrumbs = [
             {title:"<i class='fas fa-home'></i>",url:"/"},
             {title:"Documentation",url:""}
       		 ];

			  docsShowHeader = new Show.Header({breadcrumbs:breadcrumbs,color:"purple",icon:"fas fa-book"});
			  docsShowInfo = new Show.Info({data:data});
  			  docsPanel = new Show.FooterPanel({color:"blue",title: "Back to Home <i class='fas fa-home'></i>",manual:true});

  			  docsShowInfo.on("doc:append",function(){
	         		loadingCircleView.destroy();
	         });

	          docsShowLayout.showChildView('headerRegion',docsShowHeader);
	          docsShowLayout.showChildView('infoRegion',docsShowInfo);
	          docsShowLayout.showChildView('panelRegion',docsPanel);


	  		
		

			   docsPanel.on("go:back",function(){
			  	 App.trigger("home:portal");
			  });


    		}); // on.attach()

  			
          App.mainLayout.showChildView('mainRegion',docsShowLayout);

           });

    	}) // require
    	
		}

	}


return Controller;

});
