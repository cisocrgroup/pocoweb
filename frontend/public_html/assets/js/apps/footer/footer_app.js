
define(["marionette","app"], function(Marionette,IPS_App){

	var FooterApp={};

	FooterApp.API = {
		showFooter: function(){
			require(["apps/footer/show/show_controller"], function(ShowController){
       				ShowController.showFooter();
				});
		}
	};




 return FooterApp; 	

});
