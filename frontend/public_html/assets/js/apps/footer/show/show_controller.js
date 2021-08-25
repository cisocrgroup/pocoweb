
define(["marionette","app","apps/footer/show/show_view"], function(Marionette,App,Show){



 var Controller = {

		showFooter: function(){
			var footerView = new Show.Footer();
     		App.mainLayout.showChildView('footerRegion',footerView);
		}

	}


return Controller;

});
