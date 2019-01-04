// ==============================
// apps/projects/common/views.js
// ==============================

define(["app","marionette","backbone.syphon",
        "common/util","common/views"
        ,"tpl!apps/projects/common/templates/projectform.tpl"
        ,"tpl!apps/projects/common/templates/listtemplate.tpl"

        	], function(IPS_App,Marionette,Syphon,Util,CommonViews,projectTpl,listTpl){


var Views = {};




 Views.ProjectsList = CommonViews.Icon_DataTable.extend({
   template: listTpl,
   events:{
    "click .js-delete-project": "deleteProject",
    "click .js-open-project": "openProject"
   },
   initialize: function(){
        this.urlroot="projects",
        this.datatable_options={stateSave:true},
        this.headers = [
          {name: "Title"},
          {name: "Author"},
          {name: "Year"},
          {name: "Language"},
          {name: "Pages"},
          {name: "Book"},
          {name: "Action"},


        ]

        this.columns = [
        {name:"title",id:"projectId",clickrow :false},
        {name:"author",id:"projectId",clickrow :false},
        {name:"year",id:"projectId",clickrow :false},
        {name:"language",id:"projectId",clickrow :false},
        {name:"pages",id:"projectId",clickrow :false},
        {name:"book",id:"projectId",clickrow :false},
        {name:"action",id:"projectId",clickrow :false}


        ]


        },
        deleteProject : function(e){
        var id = $(e.currentTarget).attr('id');
        var parentrow = $(e.currentTarget).parent().parent();
        this.trigger("list:delete",id,parentrow)
        },
        openProject : function(e){
        var id = $(e.currentTarget).attr('id');
        this.trigger("list:open",id)

        }

  });

Views.ProjectForm = Marionette.View.extend({
   template: projectTpl,
   className:"",
   selected_file:"",
   events: {
   "click .js-submit-project": "submitClicked",
   "click .js-upload": "chooseFile",
  

   },
   initialize: function(){

  },
  onAttach : function(){

     if(this.options.asModal){

          this.$el.attr("ID","projects-modal");
          this.$el.addClass("modal fade projects-modal");
          this.$el.on('shown.bs.modal', function (e) {
           })

         $('#projects-modal').modal();

    }
    var that = this
    $('#file-upload').on('change',function(){

      console.log(this.files)

      $('#selected_file').text('Selected file: '+this.files[0].name)
      that.selected_file = this.files[0]

    })

    $("#uploadForm").on('submit',(function(e) {
    e.preventDefault();
     $('.loading_background').fadeIn();
     that.trigger("project:submit_clicked", Backbone.Syphon.serialize(that), this);

    }))

  },

   // submitClicked: function(e){

   //   e.preventDefault();
   //    var data = Backbone.Syphon.serialize(this);

   //      $('.loading_background').fadeIn();

   //      if(Marionette.getOption(this,"edit_project")) {
          
   //         this.trigger("project:update_clicked", data);


   //    }

   //    else if(Marionette.getOption(this,"add_book")) {


   //      var that = this
    
   //  //    Util.getBase64(this.selected_file,function(base64){


   //                var result = {}

   //              result['project'] = data;
   //              result['project']['books'] = [];
   //              result['project']['books'][0] = Backbone.Syphon.serialize(that);



   //              that.trigger("project:addbook_clicked", result);


   //    //  });
   //    }

   //    else {

       

   //      var that = this
    
   //      // Util.getBase64(this.selected_file,function(base64){


   //                var result = {}

   //              result['project'] = data;
   //              result['project']['books'] = [];
   //              result['project']['books'][0] = Backbone.Syphon.serialize(that);

   //             that.trigger("project:submit_clicked", this);


   //      // });

   //    }

   // },

  
 

    serializeData: function(){


          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.asModal = Marionette.getOption(this,"asModal");
          data.text = Marionette.getOption(this,"text");
          data.add_book = Marionette.getOption(this,"add_book");
          data.edit_project = Marionette.getOption(this,"edit_project");
          data.loading_text = Marionette.getOption(this,"loading_text");
          data.languages = Marionette.getOption(this,"languages");

        return data;

    },

   });






return Views;


});
