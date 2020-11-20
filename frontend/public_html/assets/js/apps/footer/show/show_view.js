// =============================
// apps/footer/show/show_view.js
// =============================

define(["marionette","app","common/views",
        "tpl!apps/footer/show/templates/footer.tpl"
 ], function(Marionette,IPS_App,views,footerTpl){

 	var Show={};

	Show.Footer = Marionette.View.extend({
		template: footerTpl,

		onAttach: function(){

			$('.js-up').click(function(e){
				e.preventDefault();
				window.scrollTo(0,0);
			});
				
		}
	  
		});

return Show;

});

