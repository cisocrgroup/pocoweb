// ==============================
// apps/projects/common/views.js
// ==============================

define(["app","marionette","backbone.syphon",
        "common/util","common/views"
        ,"tpl!apps/projects/common/templates/projectform.tpl"

        	], function(IPS_App,Marionette,Syphon,Util,CommonViews,projectTpl){


var Views = {};




 Views.ProjectsList = CommonViews.Icon_DataTable.extend({
   initialize: function(){
        this.urlroot="projects",
        this.border_color="red",
        this.datatable_options={stateSave:true},
        this.headers = [
          {name: "Title"},
          {name: "Author"},
          {name: "Language"},
          {name: "Year"},
          {name: "Pages"},


        ]

        this.columns = [
        {name:"title",id:"projectId"},
        {name:"author",id:"projectId"},
        {name:"language",id:"projectId"},
        {name:"year",id:"projectId"},
        {name:"pages",id:"projectId"},


        ]


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

  },

   submitClicked: function(e){

     e.preventDefault();
      var data = Backbone.Syphon.serialize(this);
      var ocrEngine =  $('#ocrEngine').find(":selected").val();

        $('.loading_background').fadeIn();

        if(Marionette.getOption(this,"edit_project")) {
          
           this.trigger("project:update_clicked", data);


      }

      else if(Marionette.getOption(this,"add_book")) {


        var that = this
    
        Util.getBase64(this.selected_file,function(base64){


                  var result = {}

                result['project'] = data;
                result['project']['books'] = [];
                result['project']['books'][0] = Backbone.Syphon.serialize(that);
                result['project']['books'][0]['ocrEngine'] = ocrEngine


                result['content'] = base64;

                that.trigger("project:addbook_clicked", result);


        });
      }

      else {

       

        var that = this
    
        Util.getBase64(this.selected_file,function(base64){


                  var result = {}

                result['project'] = data;
                result['project']['books'] = [];
                result['project']['books'][0] = Backbone.Syphon.serialize(that);
                result['project']['books'][0]['ocrEngine'] = ocrEngine


                result['content'] = base64;

                that.trigger("project:submit_clicked", result);


        });

      }

   },

  
 

    serializeData: function(){


          var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
          data.asModal = Marionette.getOption(this,"asModal");
          data.text = Marionette.getOption(this,"text");
          data.add_book = Marionette.getOption(this,"add_book");
          data.edit_project = Marionette.getOption(this,"edit_project");
          data.loading_text = Marionette.getOption(this,"loading_text");

        return data;

    },

   });






return Views;


});
