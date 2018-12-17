// ================================
// apps/ocrd/show/show_view.js
// ================================

define(["marionette","app","backbone.syphon","common/views",
        "tpl!apps/ocrd/show/templates/layout.tpl",
        "tpl!apps/ocrd/show/templates/info.tpl",
        "tpl!apps/ocrd/show/templates/resp.tpl"


  ], function(Marionette,App,BackboneSyphon,Views,layoutTpl,infoTpl,respTpl){


    var Show = {};

  Show.Layout = Marionette.View.extend({
    template:layoutTpl,
    regions:{
       headerRegion: "#hl-region"
      ,infoRegion: "#info-region"
      ,respRegion: "#resp-region"
      ,footerlRegion: "#footer-region"
    }

  });


  Show.Header = Views.Header.extend({
    initialize: function(){
        this.title = "OCRD Prototype Tests"
      }
  });



  Show.Info = Marionette.View.extend({
      template: infoTpl,
      events:{
      'click .js-train' : 'training_clicked'
      },
     

      training_clicked:function(e){
        e.preventDefault();
        methods = [];

          $('.method_select').children().each(function(){

              if($(this).is(':checked')){
                methods.push("{type:"+$(this).val()+"}")

              }
        
        })

      var that = this;
         $('.loading_background').fadeIn(function(){
          that.trigger("show:training_clicked",methods);
          })
        

      },
     onAttach: function(){
      
      },          

  });

  Show.Resp = Marionette.View.extend({
      template: respTpl,


      formSubmitted:function(e){
        e.preventDefault();
        var data = Backbone.Syphon.serialize(this);

        this.trigger("show:formSubmitted",data);

      },
     onDomRefresh: function(){
      
      results = Marionette.getOption(this,"results")
      console.log(results)
         if(results.length==0){
          $('.loading_background').fadeOut(function(){
              });
          }
          else {
            $("#tabs").tab();
            $('.loading_background').hide();
          }

      },  
      serializeData: function(){
       return {
       results: Marionette.getOption(this,"results")
       }
      }
        

  });


 
	Show.Missingocrd = Views.Error.extend({errortext:"Error 404: ocrd not found"});


return Show;

});

