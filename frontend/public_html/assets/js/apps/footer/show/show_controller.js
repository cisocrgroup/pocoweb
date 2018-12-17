
define(["marionette","app","apps/footer/show/show_view"], function(Marionette,IPS_App,Show){



 var Controller = {

		showFooter: function(){
			var footerView = new Show.Footer();
     		IPS_App.mainLayout.showChildView('footerRegion',footerView);
		}

	}


return Controller;

});
