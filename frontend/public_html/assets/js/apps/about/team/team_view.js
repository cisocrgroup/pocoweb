// ============================
// apps/about/team/team_view.js
// ============================

define(["app","common/views"], function(IPS_App){


IPS_App.module("AboutApp.Team", function(Team,IPS_App,
Backbone,Marionette,$,_){

  Team.Layout = IPS_App.Common.Views.Layout.extend({
  });

// IPS_App.AboutApp.Common.Views.Header.extend({
  Team.Header =  IPS_App.Common.Views.Header.extend({
    initialize: function(){
        this.title = "About"
       // IPS_App.AboutApp.Common.Views.Header.prototype.initialize.call(this);

      }
  });

 
 Team.Hub = IPS_App.Common.Views.IconHub.extend({
  initialize: function(){
        this.maxrowlength = 4;
        this.rows = [
        {
         name: "Database",
         icon: "fa fa-database",
           items:[
             {
             name:"About the Project",
             url:"#about/project",
             icon:"fa fa-question",
             subheader:"The projects' research goals",
             loggedIn:false
             },
              {
             name:"Legal Notice",
             url:"#about/legalnotice",
             icon:"fa fa-gavel",
             subheader:"Legal notice and disclaimer",
             loggedIn:false
             },
              {
             name:"Team",
             url:"#about/team",
             icon:"fa fa-users",
             subheader:"EnMAP Ground Segment Team",
             loggedIn:false
             },
           ]
           },
            {
            items:[
             {
             name:"Contact Us",
             url:"#about/contact",
             icon:"fa fa-envelope",
             subheader:"Send an e-mail to the EnMAP Ground Segment Team",
             loggedIn:false
             }
           ]
           }
    


        ]
      }
 })

});

return IPS_App.AboutApp.Team;

});

