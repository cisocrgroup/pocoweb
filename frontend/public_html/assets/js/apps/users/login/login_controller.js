
define(["app","apps/users/login/login_view"], function(App){

  
  var Controller = {

    showLogin: function(){

      var loginFormView = new Login.Form({
				asModal:true
	  });
	  App.dialogRegion.show(loginFormView);

    }

}

return App.UsersApp.Login.Controller;


});