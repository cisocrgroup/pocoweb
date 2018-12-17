// =============================
// apps/home/login/login_view.js
// =============================

define(["app",
  "tpl!apps/home/login/templates/loginform.tpl",
  "tpl!apps/home/login/templates/loginrole.tpl",
  "backbone.syphon",
  "common/util"
  ],
function(IPS_App,loginformTpl,loginroleTpl,syphon,Util){

IPS_App.module("HomeApp.Login", function(Login,IPS_App,Backbone,Marionette,$,_){


 Login.Form = Marionette.ItemView.extend({
   template: loginformTpl,
   events: {
   "click #login_submit_button": "submitClicked"
  ,"click .close-reveal-modal" : "closeLogin"
   }

  ,closeLogin: function(e){  },

   submitClicked: function(e){

   e.preventDefault();
   var data = Backbone.Syphon.serialize(this);
   this.model.set(data);
   
    //  var preValidate = this.model.isValid(['username', 'password']);
    // var customValidate = this.customValidate(data);
    // if(customValidate)
     this.trigger("form:submit", data);
   },
  customValidate: function(data){

        if(!this.model.isValid('username')) { var error = this.model.preValidate('username', data['username']); Util.showFormErrors("username",error); }
        if(!this.model.isValid('password')) { var error = this.model.preValidate('password', data['password']); Util.showFormErrors("password",error); }

        if(this.model.isValid('username')) { var error = this.model.preValidate('username', data['username']); Util.hideFormErrors("username"); }
        if(this.model.isValid('password')) { var error = this.model.preValidate('password', data['password']); Util.hideFormErrors("password"); }

     if(this.model.isValid(['username', 'password'])){
      return true;
      } 

      return false;
     },     

    serializeData: function(){
      return {
    asModal: Marionette.getOption(this,"asModal"),
    }
  },

   onShow: function(){

     if(this.options.asModal){

          this.$el.attr("ID","login-modal");
          this.$el.addClass("modal fade login-modal");
          this.$el.on('shown.bs.modal', function (e) {

// #ifdef DEVELOPMENT_VERSION
            perform_test("apps/home/login/login_view.js onShow 95");
// #endif DEVELOPMENT_VERSION

           })

          var that = this;
          this.$el.on('hidden.bs.modal', function (e) {
// #ifdef DEVELOPMENT_VERSION
            perform_test("apps/home/login/login_view.js onShow modal hidden 105");
// #endif DEVELOPMENT_VERSION
            that.trigger("user:roles");
           })
             this.$el.modal();
    }
     else {
       var $title = $('#formhl');
     $title.text(this.title);
    }
   } // onShow()
});

Login.RoleSelection = Marionette.ItemView.extend({
   template: loginroleTpl,
   events: {
   "click .btn-primary"        : "roleClicked"
  ,"click .close-reveal-modal" : "closeRoleSelection"
   },

    serializeData: function(){
      return {
    asModal: Marionette.getOption(this,"asModal"),
    model: Marionette.getOption(this,"model"),

    }
  },

   onShow: function(){

     if(this.options.asModal){

          this.$el.attr("ID","login-modal");
          this.$el.addClass("modal fade login-modal");
          this.$el.on('shown.bs.modal', function (e) {

// #ifdef DEVELOPMENT_VERSION
            perform_test("apps/home/login/login_view.js onShow modal shown 147");
// #endif DEVELOPMENT_VERSION

           })

          var that = this;
          this.$el.on('hidden.bs.modal', function (e) {
// #ifdef DEVELOPMENT_VERSION
            perform_test("apps/home/login/login_view.js onShow hidden 156");
// #endif DEVELOPMENT_VERSION
            that.trigger("user:roles");
           })

           this.$el.modal();
    }
// XXXXX ????
     else {
       var $title = $('#formhl');
     $title.text(this.title);
    }
// XXXXX ????
     
   } // onShow()
  ,closeRoleSelection: function(e){  }
  ,roleClicked: function(e){
     e.preventDefault();
     this.trigger("roleselection:selected", e.target.innerHTML);
     $('#login-modal').modal('hide');
   },
  
});


});

return IPS_App.HomeApp.Login.View;

});

