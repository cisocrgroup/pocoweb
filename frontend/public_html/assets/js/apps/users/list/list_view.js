define(["app","marionette","common/views","apps/users/common/views",
        "tpl!apps/users/list/templates/layout.tpl",
        "tpl!apps/users/list/templates/panel.tpl"], function(App,Marionette,Views,UserViews,layoutTpl,panelTpl){


var List = {}

   List.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
      headerRegion: "#hl-region",
      panelRegion: "#panel-region",
      infoRegion: "#info-region",
      footerRegion: "#footer-region"

    }

  });

 List.Header = Views.Header.extend({
    initialize: function(){
        this.title = "Users"
        this.icon ="fas fa-users"
        this.color ="red"
      }
  });

  List.UsersList = Views.Icon_DataTable.extend({
   events: {
    "click .js-delete-user": "delete_user"
   },
   initialize: function(){
        this.urlroot="users"
        this.actioncolumn = true

        this.headers = [
          {name: "#"},
          {name: "Name"},
          {name: "Email"},
          {name: "Institute"},
          {name: "Admin"},
          {name: "Action"}

        ]

        this.columns = [
        {name:"id",id:"id",clickrow :false},
        {name:"name",id:"id",clickrow :false},
        {name:"email",id:"id",clickrow :false},
        {name:"institute",id:"id",clickrow :false},
        {name:"admin",id:"id",clickrow :false},
        {name:"action",id:"id",clickrow :false}

        ]


        },
       delete_user : function(e){
        var id = $(e.currentTarget).attr('id');
        var parentrow = $(e.currentTarget).parent().parent();
        this.trigger("user:delete",id,parentrow);
     }


  });

    List.Panel = Marionette.View.extend({
      template: panelTpl,
       triggers:{
        "click button.js-create":   "user:create"
      }

  });

  List.Form = UserViews.Form.extend({

  });



  List.AreYouSure = Views.AreYouSure.extend({
      triggers:{
     "click .js-yes":"delete:confirm"
    }
  })

  List.FooterPanel = Views.FooterPanel.extend({
    });


return List;

});
