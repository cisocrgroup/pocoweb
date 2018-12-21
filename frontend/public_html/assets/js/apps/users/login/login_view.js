
define(["app","marionette",
  "tpl!apps/users/login/templates/loginform.tpl",
   "backbone.syphon",
   "common/util"
  ],
function(App,Marionette,loginformTpl,syphon,Util){

  var Login = {};


 Login.Form = Marionette.View.extend({
   template: loginformTpl,
   events: {
   "click .js-loginsubmit": "submitClicked"

   },

   submitClicked: function(e){
   e.preventDefault();
   var data = Backbone.Syphon.serialize(this);
  
   
    //  var preValidate = this.model.isValid(['username', 'password']);
    // console.log(preValidate);
    this.trigger("login:submit", data);
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

   // onRender: function(){
   //   Backbone.Validation.bind(this, {
   //   valid: function(view, attr){
   //    // hide errors on the `attr` attribute from the view
   //    hideFormErrors(attr);

   //   },
   //   invalid: function(view, attr, error){
   //   // show errors on the `attr` attribute from the view
   //     showFormErrors(attr,error);
   //   }
   //  });
  
   // },

   // onShow: function(){
   //   if(this.options.asModal){



   //  }
   //   else {
   //     var $title = $('#formhl');
   //   $title.text(this.title);
   //  }
   // }
  
});



return Login;


});

