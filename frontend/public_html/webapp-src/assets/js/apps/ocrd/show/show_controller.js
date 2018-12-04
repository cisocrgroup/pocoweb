// ======================================
// apps/ocrd/show/show_controller.js
// ======================================

define(["app","common/util","common/views","apps/ocrd/show/show_view"], function(App,Util,Views,Show){


 Controller = {

		showOcrd: function(id){
      		$(window).scrollTop(0);

	   		require(["entities/util"], function(UtilEntitites){

	   	      var loadingCircleView = new  Views.LoadingBackdropOpc();
              App.mainLayout.showChildView('backdropRegion',loadingCircleView);

        	
			loadingCircleView.destroy();

		 	//currentProposal.set({"url_id":id}); // pass url_id to view..
			var ocrdShowLayout = new Show.Layout();
			var ocrdShowHeader;
			var ocrdShowInfo;

			// console.log(reviews);
	
			ocrdShowLayout.on("attach",function(){
			  

			  ocrdShowHeader = new Show.Header();
			  ocrdShowInfo = new Show.Info();
			  ocrdShowExampleResponse = new Show.Resp({results:{}})


			  ocrdShowInfo.on("show:training_clicked",function(methods){


			  	var postingTrainingData = UtilEntitites.API.startTraining({methods:methods}); 

  				$.when(postingTrainingData).done(function(response){
  					
  					var resultarray = JSON.parse(response.result);

  					for (key in resultarray) {
						console.log(resultarray[key].evalstring) 

  						resultarray[key].evalstring = resultarray[key].evalstring[0].replace(/(?:\r\n|\r|\n)/g, '<br />');

  					}


  					ocrdShowExampleResponse.options.results = resultarray;
  					ocrdShowExampleResponse.render();

	    		}).fail(function(response){ 

	 			  //     loadingCircleView.destroy();
					  // var errortext = Util.getErrorText(response);    
	      //             var errorView = new Show.Error({errortext:errortext})

	      //             App.mainLayout.showChildView('mainRegion',errorView);

	          }); //  $.when




			  });


  			// ocrdPanel = new Show.FooterPanel();


	          ocrdShowLayout.showChildView('headerRegion',ocrdShowHeader);
	          ocrdShowLayout.showChildView('infoRegion',ocrdShowInfo);
	          ocrdShowLayout.showChildView('respRegion',ocrdShowExampleResponse);



    		}); // on.attach()

          App.mainLayout.showChildView('mainRegion',ocrdShowLayout);



    	}) // require
    	
		}

	}


return Controller;

});
