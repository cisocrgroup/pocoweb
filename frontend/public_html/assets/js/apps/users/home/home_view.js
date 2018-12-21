
define(["app","common/views"], function(ResearchTool){

ResearchTool.module("UsersApp.Home", function(Home,ResearchTool,
Backbone,Marionette,$,_){

  Home.Layout = ResearchTool.Common.Views.LoginUserLayout.extend({
  });

    Home.Header = ResearchTool.Common.Views.Header.extend({
    initialize: function(){
        this.title = "Users"

        this.breadcrumbs = [
        {name: "Users", url: "#/users",current:"true"},
        ]
      }
  });


  Home.Error = ResearchTool.Common.Views.LoginError.extend({});

//  Home.Hub = ResearchTool.Common.Views.Hub.extend({
//   initialize: function(){

//         this.rows = [
//         {
//          name: "Database",
//          icon:"fa fa-database",
//          items:[
//            {name:"Browse",url:"#users/browse",icon:"fa fa-list-alt",loggedIn:true},
//            {name:"Query",url:"#users/query",icon:"fa fa-question",loggedIn:true},
//            {name:"Create New Account",url:"#users/new",icon:"fa fa-user-plus",loggedIn:true},
//          ]
//         },
//         {
//        name: "Statistics",
//        icon:"fa fa-bar-chart",
//        items:[
//          {name:"Charts",url:"#users/statistics/charts",icon:"fa fa-pie-chart",loggedIn:true},
//          {name:"Query",url:"#users/statistics/query",icon:"fa fa-question",loggedIn:true},
//        ]
//       },


//         ]
//       }
//  })

// });

Home.Hub = ResearchTool.Common.Views.IconHub.extend({
  initialize: function(){
        this.maxrowlength=3,
        this.rows = [
        {
         needsLogin:true,
         name: "Database",
           items:[
              {
             name:"My Account",
             url:"#users/"+this.model.get('user_id'),
             icon:"fa fa-user",
             subheader:"Information on my account",
             loggedIn:false
             },
             {
             name:"Browse",
             url:"#users/browse",
             icon:"fa fa-list-alt",
             subheader:"Browse the users database",
             loggedIn:false
             },
             //  {
             // name:"Create New Account",
             // url:"#users/new",
             // icon:"fa fa-user-plus",
             // subheader:"Add a new user",
             // loggedIn:true
             // },
           ]
           },
           //  {
           //  needsLogin:true,
           // name: "Statistics",
           //  items:[
           //   {
           //   name:"Charts",
           //   url:"#users/statistics/charts",
           //   icon:"fa fa-pie-chart",
           //   subheader:"Interactive charts concerning the users database",
           //   loggedIn:false
           //   },
           //    {
           //   name:"Query",
           //   url:"#users/statistics/query",
           //   icon:"fa fa-bar-chart",
           //   subheader:"Query result as a chart",
           //   loggedIn:false
           //   },
    
           // ]
           // }
    


        ]
      }
 })

});


return ResearchTool.UsersApp.Home;

});

