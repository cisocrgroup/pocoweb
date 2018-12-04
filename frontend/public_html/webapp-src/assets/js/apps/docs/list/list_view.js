// ================================
// apps/proposals/list/list_view.js
// ================================

define(["marionette","app","common/views","apps/proposals/common/views"], function(Marionette,IPS_App,CommonViews,Views){

  var List ={};

  List.Layout = CommonViews.Layout.extend({    
  });

  
    List.Header = CommonViews.Header.extend({
    initialize: function(){
        this.title = "Proposal Survey"
        this.icon ="fa fa-th-list"
        this.color ="red"
      }
  });


  List.ProposalsList = Views.ProposalsList.extend({
   
  });

  List.FooterPanel = CommonViews.FooterPanel.extend({
    });



return List;

});
