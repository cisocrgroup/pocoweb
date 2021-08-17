// =============================
// apps/footer/show/show_view.js
// =============================

define(["marionette","app","common/views",
        "tpl!apps/footer/show/templates/footer.tpl"
 ], function(Marionette,App,views,footerTpl){

 	var Show={};

	Show.Footer = Marionette.View.extend({
		template: footerTpl,

		onAttach: function(){

			$('.js-top').click(function(e){
				e.preventDefault();
				window.scrollTo(0,0);
			});

		   $(window).scroll(function () {
	          var scroll = $(window).scrollTop();

	          if (scroll >= 300) {
	              $(".top-button").addClass("visible");
	          } else {
	              $(".top-button").removeClass("visible");
	          }
	      });
  

				
		}
	  
		});

return Show;

});

