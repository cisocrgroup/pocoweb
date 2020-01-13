
define(["app","common/util","common/views","apps/users/common/views"], function(App,Util,Views,UserViews){

var New = {}
 
 New.Header = Views.Header.extend({
    initialize: function(){
        this.title = "Create a new user account"
        this.icon ="fas fa-user-plus"
        this.color ="red"
      }
  });


 New.Layout = Views.Layout.extend({
 });

  New.Form = UserViews.Form.extend({
  });

  New.FooterPanel = Views.FooterPanel.extend({
    });


return New;

});
