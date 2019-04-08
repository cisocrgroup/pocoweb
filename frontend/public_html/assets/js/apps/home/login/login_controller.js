// ===================================
// apps/home/login/login_controller.js
// ===================================

define(["app","apps/home/login/login_view"], function(IPS_App){

IPS_App.module("IPS_App.Login", function(Login,IPS_App,Backbone,Marionette,$,_){
  
  Login.Controller = {
    showLogin: function(){
      var loginFormView = new Login.Form({ asModal:true });
	  IPS_App.dialogRegion.show(loginFormView);
    }
  }
});

return IPS_App.IPS_App.Login.Controller;

});
