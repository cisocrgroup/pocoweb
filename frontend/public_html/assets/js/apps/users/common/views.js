
define(["app",
        "tpl!apps/users/common/templates/userform.tpl",
        "common/util"
	], function(ResearchTool,userformTpl,Util){

ResearchTool.module("UsersApp.Common.Views", function(Views,ResearchTool,Backbone,Marionette,$,_){

 Views.Form = Marionette.ItemView.extend({
	 template: userformTpl,
	 className:"row bg_style",
	 events: {
	 "click .js-submit": "submitClicked",
	 "click .js-delete": "deleteClicked",
	 "click .js-back":   "backClicked",
	 "click .js-close":  "closeClicked"

	 },
	 initialize: function(){
		
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


	 onRender: function(){

	 Backbone.Validation.bind(this, {
		 valid: function(view, attr){
		  // hide errors on the `attr` attribute from the view
 		 Util.hideFormErrors(attr);


		 },
		 invalid: function(view, attr, error){
		 // show errors on the `attr` attribute from the view
		 Util.showFormErrors(attr,error);

		 }
		});
	 },

	  onDomRefresh: function(){
		 if(this.options.asModal){
		  this.$el.removeClass("bg_style");
  		  this.$el.children().removeClass("bg_layer");

		  this.$el.addClass("reveal-modal");
  		  this.$el.append('<a class="close-reveal-modal">&#215;</a>');
		  this.$el.attr("data-reveal","");

          this.$el.foundation('reveal', 'open');
	      $(document).foundation('reflow');

		}
		$("#roles").val(this.model.get('role')); // set current

		    if(this.model.get('verified')==1) {$("#verified_checkbox").prop('checked', true); }
		    else $("#verified_checkbox").prop('checked', false);


	 },

	
	 templateHelpers: function () {
	    return {
	           asModal: this.options.asModal,
   	           role_idx: this.options.role_idx,
   	           roles: this.options.roles,
   	           captcha: this.options.captcha

	     }
  	 }

	 });


});

return ResearchTool.UsersApp.Common.Views;


});
