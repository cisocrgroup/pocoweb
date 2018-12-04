// ======================================
// apps/docs/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/docs/show/show_view"], function(IPS_App,Util,Views,Show){


 Controller = {

		showDocs: function(id){
      		$(window).scrollTop(0);

	   		require(["entities/docs"], function(DocsEntitites){

	   	      // var loadingCircleView = new  Views.LoadingBackdrop();
           //    IPS_App.mainLayout.showChildView('backdropRegion',loadingCircleView);
              // var currentUser = IPS_App.getCurrentUser();


			// loadingCircleView.destroy();

		 	//currentdocs.set({"url_id":id}); // pass url_id to view..

     		var fetchingDocs = DocsEntitites.API.getJson();

	    	$.when(fetchingDocs).done(function(data){

	    	console.log(data)


			var docsShowLayout = new Show.Layout();
			var docsShowHeader;
     		var docsShowPanel;
			var docsShowInfo;


			docsShowLayout.on("attach",function(){
			  

			  docsShowHeader = new Show.Header({});
			  docsShowInfo = new Show.Info();
  			  docsPanel = new Show.FooterPanel({color:"blue"});

	          docsShowLayout.showChildView('headerRegion',docsShowHeader);
	          docsShowLayout.showChildView('infoRegion',docsShowInfo);

	          docsShowLayout.showChildView('panelRegion',docsPanel);

	        


			   docsPanel.on("go:back",function(){
			  	 IPS_App.trigger("docs:list");
			  });


    		}); // on.attach()

          IPS_App.mainLayout.showChildView('mainRegion',docsShowLayout);

           });

    	}) // require
    	
		}

	}


return Controller;

});
