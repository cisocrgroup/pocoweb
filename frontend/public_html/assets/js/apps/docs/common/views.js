// ==============================
// apps/proposals/common/views.js
// ==============================

define(["app","marionette","backbone.syphon",
        "common/util","common/views"
        ,"tpl!apps/proposals/common/templates/proposalform.tpl"

        	], function(IPS_App,Marionette,Syphon,Util,CommonViews,proposalTpl){


var Views = {};


 Views.ProposalsList = CommonViews.Icon_DataTable.extend({
   initialize: function(){
        this.urlroot="proposalportal",
        this.border_color="red",
        this.datatable_options={stateSave:true},
        this.headers = [
          {name: "Id"},
          {name: "Title"},
          {name: "Status"},
          {name: "ORs"},
          {name: "Reviews"},
          {name: "CO-Is"},
          {name: "Topic"}

        ]

        this.columns = [
        {name:"proposalid",id:"proposal_id"},
        {name:"title",id:"proposal_id"},
        {name:"status",id:"proposal_id"},
        {name:"ors",id:"proposal_id"},
        {name:"reviews",id:"proposal_id"},
        {name:"co_is",id:"proposal_id"},
        {name:"topicacronym",id:"proposal_id"}

        ]

    
        }
   
  });

Views.ProposalsForm = Marionette.View.extend({
   template: proposalTpl,
   className:"",
   events: {
   "click .js-submit-proposal": "submitClicked",
   "click .js-delete": "deleteClicked",
   "click .js-back":   "backClicked",
   "click .js-close":  "closeClicked"

   },
   initialize: function(){
    
  },
  onAttach : function(){

     if(this.options.asModal){

          this.$el.attr("ID","proposal-modal");
          this.$el.addClass("modal fade proposal-modal");
          this.$el.on('shown.bs.modal', function (e) {
           })
       this.$el.modal();

    }

  },

   submitClicked: function(e){
     e.preventDefault();
      var data = Backbone.Syphon.serialize(this);
       var topic = $('#topic_id').find(":selected").text();
        data['topic'] = topic;
     // var checkBox =$('#notify').is(':checked');
     // var checkBoxValue=0;
     // if(checkBox) checkBoxValue=1;
     // data['notify'] = checkBoxValue;

     //   checkBox =$('#verified_checkbox').is(':checked');
     //   checkBoxValue=0;
     // if(checkBox) checkBoxValue=1;
     // data['verified_checkbox'] = checkBoxValue;
    this.trigger("proposal:submit_clicked", data);
   },

   deleteClicked: function(e){
      e.preventDefault();
      var data = Backbone.Syphon.serialize(this);
   },

   backClicked: function(e){
      e.preventDefault();
      this.trigger("form:back");
   },

  closeClicked: function(e){
      e.preventDefault();
         
   },

    serializeData: function(){


          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.asModal = Marionette.getOption(this,"asModal");
          data.text = Marionette.getOption(this,"text");
          data.topics = Marionette.getOption(this,"topics");


        return data;
    
    },

   });


 Views.Reviewers = CommonViews.Icon_DataTable.extend({
   initialize: function(){
        this.urlroot="proposalportal/reviewer",
        this.border_color="red",
        this.headers = [
          {name: "userKey"},
          {name: "Status"},

        ]

        this.columns = [
        {name:"userKey",id:"id"},
        {name:"status",id:"id"},
        ]

        }
  });



return Views;


});
