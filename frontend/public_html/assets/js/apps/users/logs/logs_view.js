define(["app","marionette",
        "tpl!apps/users/logs/templates/logs.tpl"], function(App,Marionette,logsTpl){


var Logs = {}

   
  Logs.List = Marionette.View.extend({
   template: logsTpl,
   events: {
    "click .js-delete-msg": "delete_msg"
   },


    serializeData: function(){
      return {
        headers: Marionette.getOption(this,"headers"),
        columns: Marionette.getOption(this,"columns"),
        items: Marionette.getOption(this,"collection"),
        title: Marionette.getOption(this,"title"),
        asModal: Marionette.getOption(this,"asModal"),
        table_id: Marionette.getOption(this,"table_id")

      }
    },

   initialize: function(){
        this.urlroot="users"
        // this.actioncolumn = true

        this.headers = [
          {name: "Message"},
          {name: "Date"},
          {name: "TimeStamp"},
          {name: "Type"},
          {name: "Url"}

        ]

        this.columns = [
        {name:"msg",id:"id",clickrow :false},
        {name:"date",id:"id",clickrow :false},
        {name:"type",id:"id",clickrow :false},
        {name:"url",id:"id",clickrow :false}


        ]


        },


      onDomRefresh: function(){

       var table_id = Marionette.getOption(this,"table_id");

        var that = this;
        var table = $("#"+table_id).DataTable({

           "columnDefs": [
                       {
                         "targets": [0],
                          "orderable": false
                      },
                        {
                         "targets": [1],
                          "orderable": false
                      },
                      {
                          "targets": [2],
                          "visible": false,
                          "searchable": false,
                          "orderable": false
                      },
                        {
                         "targets": [3],
                          "orderable": false
                      },
                    
                  ]


        });

        table.order( [ 2, 'desc' ] ).draw();

            if(this.options.asModal){

                 this.$el.attr("ID","logs-modal");
                 this.$el.addClass("modal fade logs-modal");
                 this.$el.on('shown.bs.modal', function (e) {

                })

                var that = this;
                 this.$el.modal();
               }



      },


       delete_msg : function(e){
        var id = $(e.currentTarget).attr('id');
        var parentrow = $(e.currentTarget).parent().parent();
        this.trigger("msg:delete",id,parentrow);
     }


  });


return Logs;

});
