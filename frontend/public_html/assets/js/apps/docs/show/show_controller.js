// ======================================
// apps/docs/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/docs/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showDocs: function(doc_type){
      		$(window).scrollTop(0);

	   		require(["entities/util"], function(UtilEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
              App.mainLayout.showChildView('backdropRegion',loadingCircleView);

              var title = "";
              var fetchingDocs = "";
              var icon = "";
	  		  if(doc_type=="api_doc"){
			 	title = "API - Documentation";
			 	icon = "fas fa-route"
    	     	fetchingDocs = UtilEntitites.API.getDocumentation(doc_type);
			  }
			  else if(doc_type=="manual"){
			 	title = "User Manual";
    	     	fetchingDocs = UtilEntitites.API.getDocumentation(doc_type);
			 	icon = "fas fa-book-open"

			  }
		 	//currentdocs.set({"url_id":id}); // pass url_id to view..


	    	$.when(fetchingDocs).done(function(data){



			var docsShowLayout = new Show.Layout();
			var docsShowHeader;
     		var docsShowPanel;
			var docsShowInfo;


			docsShowLayout.on("attach",function(){

		

			 var breadcrumbs = [
	             {title:"<i class='fas fa-home'></i>",url:"#home"},
	             {title:"Documentation",url:"/#documentation"},
	             {title:title,url:""}
       		 ];

			  docsShowHeader = new Show.Header({title:title,breadcrumbs:breadcrumbs,color:"purple",icon:icon});
			  docsShowInfo = new Show.Info({data:data});
  			  docsPanel = new Show.FooterPanel({color:"blue",title: "Back to Documentation <i class='fas fa-book'></i>",manual:true});

  			  docsShowInfo.on("doc:append",function(){
	         		loadingCircleView.destroy();
	         });

	          docsShowLayout.showChildView('headerRegion',docsShowHeader);
	          docsShowLayout.showChildView('infoRegion',docsShowInfo);
	          docsShowLayout.showChildView('panelRegion',docsPanel);	  		
		

			   docsPanel.on("go:back",function(){
			  	 App.trigger("docs:home");
			  });


    		}); // on.attach()

  			
          App.mainLayout.showChildView('mainRegion',docsShowLayout);

           });

    	}) // require
    	
		}

	}


return Controller;

});
