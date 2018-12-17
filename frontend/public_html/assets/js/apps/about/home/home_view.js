// ============================
// apps/about/home/home_view.js
// ============================

define(["app","common/views",
  "tpl!apps/about/home/templates/contacttemplate.tpl"

  ], function(IPS_App,views,contactTpl){


IPS_App.module("AboutApp.Home", function(Home,IPS_App,
Backbone,Marionette,$,_){

  Home.Layout = IPS_App.Common.Views.Layout.extend({
  });

// IPS_App.AboutApp.Common.Views.Header.extend({
  Home.Header =  IPS_App.Common.Views.Header.extend({
    initialize: function(){
        this.title = "About"
       // IPS_App.AboutApp.Common.Views.Header.prototype.initialize.call(this);

      }
  });

 
 Home.Hub = IPS_App.Common.Views.IconHub.extend({
  initialize: function(){
        this.maxrowlength = 4;
        this.rows = [
        {
        
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
           ]
           },
            {
            items:[
             {
             name:"Contact Us",
             selector:"js-contact",
             icon:"fa fa-envelope",
             subheader:"Send an e-mail to the EnMAP Ground Segment Team",
             loggedIn:false
             }
           ]
           }
        ]
      },
    events: {
   'click .js-contact': 'showContactClicked',
   },

   showContactClicked: function(e){
   this.trigger('contact:show');
   },
 })

   Home.Contact = Marionette.ItemView.extend({
   template: contactTpl,

   onShow: function(){
     if(this.options.asModal){
      this.$el.addClass("reveal-modal");
        this.$el.append('<a class="close-reveal-modal">&#215;</a>');
      this.$el.attr("data-reveal","");

          
        $(document).foundation('reflow');

    }

  }
  ,onloadEvent: function() { alert("onload  apps/about/home/home_view.js") }
  ,onloadBodyEvent: function() { alert("onload body apps/about/home/home_view.js") }
  ,renderEvent: function() { alert("render  apps/about/home/home_view.js") }


 });


});

return IPS_App.AboutApp.Home;

});

