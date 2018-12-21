define(["app","common/views","apps/users/common/views"], function(App){



  Edit.Layout = App.Common.Views.LoginUserLayout.extend({});


  Edit.Header = App.Common.Views.Header.extend({
    initialize: function(){
        this.title = "Edit User: "+this.model.get('username');

        this.breadcrumbs = [
        {name: "Users", url: "#/users"},
        {name: this.model.get('username'), url: "#/users/"+this.model.get('user_id')},
        {name: "Edit",current:"true"}
        ]
      }
  });


  Edit.User = App.UsersApp.Common.Views.Form.extend({});

 Edit.Error = App.Common.Views.LoginError.extend({});



return App.UsersApp.Edit;

});



