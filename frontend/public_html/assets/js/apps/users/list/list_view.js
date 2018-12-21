define(["app","common/views"], function(App){


var List = {}

  List.Layout = App.Common.Views.LoginUserLayout.extend({    
  });

  
    List.Header = App.Common.Views.Header.extend({
    initialize: function(){
        this.title = "Browse: Users"

        this.breadcrumbs = [
        {name: "Users", url: "#/users"},
        {name: "Browse", url: "#/users",current:"true"},

        ]
      }
  });


  List.UsersList = App.Common.Views.DataTable.extend({
   initialize: function(){
        this.urlroot="users"

        this.headers = [
          {name: "Username"},
          {name: "Role"},
          {name: "Email"},
          {name: "Verified"}
        ]

        this.columns = [
        {name:"username",id:"user_id"},
        {name:"role",id:"user_id"},
        {name:"email",id:"user_id"},
        {name:"verified",id:"user_id"},

        ]

    
        }
   
  });


 List.Error = App.Common.Views.LoginError.extend({});




return List;

});
