
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
 
 templateHelpers: function () {
	    return {
	           asModal: this.options.asModal,
   	          id: Marionette.getOption(this,"id")
	     }
  	 },
     onAttach: function(){

			 if(this.options.asModal){
			  		this.$el.addClass("modal fade");
					this.$el.attr("tabindex","-1");
					this.$el.attr("role","dialog");
					this.$el.attr("id",this.id);
			      	$("#"+this.id).modal();
			}
       },

	 submitClicked: function(e){
		 e.preventDefault();
  		var data = Backbone.Syphon.serialize(this);
  		var role = $('#roles').find(":selected").text();
  		data['role'] = role;
  	 var checkBox =$('#notify').is(':checked');
     var checkBoxValue=0;
     if(checkBox) checkBoxValue=1;
     data['notify'] = checkBoxValue;

       checkBox =$('#verified_checkbox').is(':checked');
       checkBoxValue=0;
     if(checkBox) checkBoxValue=1;
     data['verified'] = checkBoxValue;
		this.trigger("form:submit", data);
	 },

	 deleteClicked: function(e){
		  e.preventDefault();
  		var data = Backbone.Syphon.serialize(this);
	 },

	 backClicked: function(e){
		  e.preventDefault();
			this.trigger("form:back");
	 },

 	closeClicked: function(e){
		  e.preventDefault();
          this.$el.foundation('reveal', 'close');
	 },


	
	 });



return Views;


});
