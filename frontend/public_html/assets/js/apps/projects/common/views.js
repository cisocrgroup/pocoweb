// ==============================
// apps/projects/common/views.js
// ==============================

define(["app","marionette","backbone.syphon",
        "common/util","common/views"
        ,"tpl!apps/projects/common/templates/projectform.tpl"

        	], function(IPS_App,Marionette,Syphon,Util,CommonViews,projectTpl){


var Views = {};




 Views.ProjectsList = CommonViews.Icon_DataTable.extend({
   events:{
    "click .js-delete-project": "deleteProject",
    "click .js-open-project": "openProject",
    "click .js-start-profiling": "startProfiling"


   },
   initialize: function(){
        this.urlroot="projects",
        this.datatable_options={stateSave:true},
        this.headers = [
          {name: "#"},
          {name: "Title"},
          {name: "Author"},
          {name: "Year"},
          {name: "Language"},
          {name: "Pages"},
        ],
        this.columns = [
          {name:"projectId",id:"projectId",clickrow :true},
          {name:"title",id:"projectId",clickrow :true},
          {name:"author",id:"projectId",clickrow :true},
          {name:"year",id:"projectId",clickrow :true},
          {name:"language",id:"projectId",clickrow :true},
          {name:"pages",id:"projectId",clickrow :true},
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
        },
        startProfiling : function(e){
        var id = $(e.currentTarget).attr('id');
        this.trigger("list:profile",id)

        }

  });

Views.ProjectForm = Marionette.View.extend({
   template: projectTpl,
   className:"",
   selected_file:"",
   events: {
   "click .js-submit-project": "submitClicked",
   "click .js-upload": "chooseFile",
   "click .js-edit-project": "updateClicked"

   },
   initialize: function(){

  },
  updateClicked:function(){
     this.trigger("project:update", Backbone.Syphon.serialize(this));

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
    }

   });






return Views;


});
