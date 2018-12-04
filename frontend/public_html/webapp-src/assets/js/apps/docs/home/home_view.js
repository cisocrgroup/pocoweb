// ================================
// apps/proposals/home/home_view.js
// ================================

define(["app","common/views"], function(IPS_App,Views){

  var Home={};

  Home.Layout = Views.Layout.extend({
  });

    Home.Header = Views.CardHeader.extend({
    initialize: function(){
        this.title = "Proposal Portal",
        this.icon = "fa fa-calendar",
        this.color ="red",
        this.stroke = "#d30014"
      }
  });




Home.Hub = Views.CardHub.extend({
  initialize: function(){

        this.maxrowlength = 3;
        
      }
 });




return Home;

});
