// ======================================
// apps/proposals/home/home_controller.js
// ======================================

define(["app","common/util","common/views","apps/proposals/home/home_view"], function(IPS_App,Util,Views,Home){


 Controller = {

 	showHome: function(){


   		require(["entities/user","entities/util"], function(UserEntities,UtilEntities){
      $(window).scrollTop(0);

       var currentUser = IPS_App.getCurrentUser();
       var fetchingHubData = UtilEntities.API.getInterfaceData({userKey:currentUser.get('userKey'),role:[currentUser.get('userRole')],frontendRoute:IPS_App.getCurrentRoute()});

       var loadingCircleView = new  Views.LoadingBackdrop();
       IPS_App.mainLayout.showChildView('backdropRegion',loadingCircleView);

  

      $.when(currentUser,fetchingHubData).done(function(currentUser,cardHubData){

       loadingCircleView.destroy();

		var proposalsHomeLayout = new Home.Layout();
	 	var	proposalsHomeHeader = new Home.Header();

	 	console.log(cardHubData);

		var	proposalsHomeHub = new Home.Hub({cards:Util.getSortedCards(cardHubData.cards),columns:3,back_btn:true});


			proposalsHomeLayout.on("attach",function(){
			

          proposalsHomeLayout.showChildView('headerRegion',proposalsHomeHeader);
          proposalsHomeLayout.showChildView('contentRegion',proposalsHomeHub);

    		});

			proposalsHomeHub.on("cardHub:clicked",function(data){
			if(data.rolename!=""){
				currentUser.set('userRole',data.rolename);
			}

			IPS_App.trigger(data.url,currentUser.get('userKey'));


		})
			
		    proposalsHomeHub.on("go:back",function(){
			  	 IPS_App.trigger("home:portal");
			  });	

		
    IPS_App.mainLayout.showChildView('mainRegion',proposalsHomeLayout);

    });

		}); // require

	}
 }


return Controller;

});
