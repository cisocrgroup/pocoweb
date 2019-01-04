
define(["app","marionette",
        "tpl!apps/users/common/templates/userform.tpl",
        "common/util"
	], function(App,Marionette,userformTpl,Util){

var Views = {}

 Views.Form = Marionette.View.extend({
	 template: userformTpl,
	 events: {
	 "click .js-submit": "submitClicked",
	 "click .js-delete": "deleteClicked",
	 "click .js-back":   "backClicked",
	 "click .js-close":  "closeClicked"

	 },
	 initialize: function(){
		
 	},

   	 serializeData: function(){

         var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.asModal = Marionette.getOption(this,"asModal");
          data.modaltitle = Marionette.getOption(this,"modaltitle");
          data.admincheck = Marionette.getOption(this,"admincheck");
          data.id = Marionette.getOption(this,"id");

        return data;
    
    },
     onAttach: function(){

			 if(Marionette.getOption(this,"asModal")){
			  		this.$el.addClass("modal fade");
					this.$el.attr("tabindex","-1");
					this.$el.attr("role","dialog");
					this.$el.attr("id",this.id);
			      	$("#"+this.id).modal();
			}
       },

	 submitClicked: function(e){
		 e.preventDefault();
  	    var data = {}
      data['name'] = $("input[name=name]").val();
      data['email'] = $("input[name=email]").val();
      data['institute'] = $("input[name=institute").val();
      data['password'] = $("input[name=password]").val();
      data['new_password'] = $("input[name=new_password]").val();
	  if($('#admin_check').length>0) data['admin'] = $('#admin_check').val();
		this.trigger("form:submit", data);
	 }



	
	 });



return Views;


});
