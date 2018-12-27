define(["app","common/views"], function(App,Views){


var List = {}

  List.Layout = Views.Layout.extend({    
  });

  
 List.Header = Views.Header.extend({
    initialize: function(){
        this.title = "Users"
        this.icon ="fas fa-users"
        this.color ="red"
      }
  });

  List.UsersList = Views.Icon_DataTable.extend({
   initialize: function(){
        this.urlroot="users"

        this.headers = [
          {name: "#"},
          {name: "Name"},
          {name: "Email"},
          {name: "Institute"},
          {name: "Admin"}

        ]

        this.columns = [
        {name:"id",id:"id"},
        {name:"name",id:"id"},
        {name:"email",id:"id"},
        {name:"institute",id:"id"},
        {name:"admin",id:"id"},

        ]

    
        }
   
  });



  List.FooterPanel = Views.FooterPanel.extend({
    });


return List;

});
