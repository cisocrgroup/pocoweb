// ======================================
// apps/proposals/list/list_controller.js
// ======================================

define(["app","common/util","common/views","apps/proposals/list/list_view"], function(IPS_App,Util,Views,List){


  var Controller = {

 	listProposals: function(){

     		require(["entities/proposal"], function(ProposalEntitites){

          var loadingCircleView = new  Views.LoadingBackdrop();
          IPS_App.mainLayout.showChildView('backdropRegion',loadingCircleView);

   		
     var fetchingProposals = ProposalEntitites.API.getProposals_List();

		 var proposalsListLayout = new List.Layout();

    	 $.when(fetchingProposals).done(function(proposals){
		   loadingCircleView.destroy();


    		proposalsListLayout.on("attach",function(){

 			
      var proposalsListHeader = new List.Header();
			var proposalsListView = new List.ProposalsList({collection: proposals});
      var porposalsListFooterPanel = new List.FooterPanel();

          proposalsListLayout.showChildView('headerRegion',proposalsListHeader);
          proposalsListLayout.showChildView('contentRegion',proposalsListView);
          proposalsListLayout.showChildView('panelRegion',porposalsListFooterPanel);

          $(window).scrollTop(0);


 		}); // onAttach()


       IPS_App.mainLayout.showChildView('mainRegion',proposalsListLayout);

		}).fail(function(response){ 


 			     // loadingCircleView.destroy();
				  var errortext = Util.getErrorText(response);    
                  var errorView = new List.Error({model: currentUser,errortext:errortext})

                  errorView.on("currentproposal:loggedIn",function(){
					        IPS_App.ProposalsApp.List.Controller.listProposals();
                  });

                  IPS_App.mainLayout.showChildView('mainRegion',errorView);
 
                          
                         
                                        
          }); //  $.when(fetchingAuth).done // $when fetchingProposals

		}); // require
	}
 }

return Controller;

});
