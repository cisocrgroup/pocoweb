// ================================
// apps/concordance/show/show_view.js
// ================================

define(["marionette","app","medium","imagesLoaded","backbone.syphon","common/views","common/util",
        "tpl!apps/projects/concordance/show/templates/concordance.tpl",


  ], function(Marionette,App,MediumEditor,ImagesLoaded,BackboneSyphon,Views,Util,concordanceTpl){


    var Show = {};

  Show.Concordance = Marionette.View.extend({
  template:concordanceTpl,
  editor:"",
events:{
      'click .js-stepbackward' : 'backward_clicked',
      'click .js-stepforward' : 'forward_clicked',
      'click .js-firstconcordance' : 'firstconcordance_clicked',
      'click .js-lastconcordance' : 'lastconcordance_clicked',
      'click .js-correct' : 'correct_clicked',
      'click .line-text' : 'line_clicked',
      'mouseup .line-text' : 'line_selected',

      },


      serializeData: function(){
      var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
      console.log(data);
      var asModal = Marionette.getOption(this,"asModal");
          data.tokendata =  Marionette.getOption(this,"tokendata");
          data.suggestions =  Marionette.getOption(this,"suggestions");
          data.Util = Util;
          data.asModal = asModal;

        return data;
      },



       backward_clicked:function(e){
        e.preventDefault();
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        console.log(data)
        this.trigger("concordance:new",data.prevPageId);
      },

       forward_clicked:function(e){
        var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new",data.nextPageId);
      },

       firstconcordance_clicked:function(e){
         var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new","first");
      },
       lastconcordance_clicked:function(e){
             var data = Backbone.Marionette.View.prototype.serializeData.apply(this, arguments);
        e.preventDefault();
        this.trigger("concordance:new","last");
      },
      correct_clicked:function(e){
       
        var anchor = $(e.currentTarget).attr('anchor');
          var ids = Util.getIds(anchor);
          var text = $('#line-text-'+anchor).text();
          this.trigger("concordance:correct_line",{pid:ids[0],concordance_id:ids[1],line_id:ids[2],text:text},anchor)

      },
      line_clicked:function(e){
        e.preventDefault();
        $('.correct-btn').hide();
        $('.line-text').css('border-bottom','1px solid transparent');
        $('.line-text').css('border-left','1px solid transparent');
        $('.line-text').css('border-top','1px solid transparent');
        $('.line-text').css('border-top-left-radius','0rem');
        $('.line-text').css('border-bottom-left-radius','0rem');

  
        $(e.currentTarget).css('border-left','1px solid #ced4da');
        $(e.currentTarget).css('border-bottom','1px solid #ced4da');
        $(e.currentTarget).css('border-top','1px solid #ced4da');
        $(e.currentTarget).css('border-top-left-radius','.25rem');
        $(e.currentTarget).css('border-bottom-left-radius','.25rem');

        $(e.currentTarget).next().find('.correct-btn').show();
        
      },
      line_selected:function(e){
        var selection = window.getSelection().toString();
        this.trigger("concordance:line_selected",selection)
      },
     

    onAttach : function(){
      var that = this;
       if(this.options.asModal){

          this.$el.attr("id","conc-modal");
          this.$el.addClass("modal fade conc-modal");
        
          $('#conc-modal').on('show.bs.modal', function () {
                that.trigger("conc:destroy:editor")
            })
            $('#conc-modal').on('hidden.bs.modal', function () {
            })

             $('#conc-modal').on('shown.bs.modal', function () {
            })
           
        
       }

$('#conc-modal').imagesLoaded( function() {

  $('#conc-modal').css('opacity','1').show();

          var tokendata =  Marionette.getOption(that,"tokendata");
          var query = tokendata.query;
 

           _.each(tokendata.matches, function(match) {
            var line = match['line'];
            var linetokens = line.tokens;
            var splitImgLine = $('<div class="splitLine"></div>');
            $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").append(splitImgLine);

            var cnt = 0;
            var offset = 0;
            var img_id = "img_"+line['pageId']+"_"+line['lineId'];

             // linecor = Util.replace_all(linecor,word['cor'],'<span class="badge badge-pill badge-primary">'+word['cor']+'</span>');

              _.each(linetokens, function(token) {

                // var gettingSplitImages = ProjectEntitites.API.getSplitImages({word:word});
                //          $.when(gettingSplitImages).done(function(images){

                //           line['leftImg'] = images.leftImg;
                //           line['rightImg'] = images.rightImg;
                //           line['middleImg'] = images.middleImg;
                //          projectConcView.render();
                //          });

                 var box = token['box'];

                var line_img = document.getElementById(img_id);

                // console.log(line_img);

                var c = document.createElement("canvas");
                c.width=box.width;
                c.height=box.height;

                  var ctx = c.getContext("2d");
                  ctx.drawImage(line_img, offset, 0, box.width, box.height, 0, 0, box.width, box.height);

                    var img = new Image();
                    img.src = c.toDataURL();
                    img.setAttribute('id','splitImg_'+line['pageId']+"_"+line['lineId']+"_"+cnt);
                    img.height = '25';

                    $('#splitImg_'+line['lineId']+"_"+cnt).css('width','auto');

                    var cordiv = $("<div>"+token.cor.trim()+"</div>");
                    if(query==token.cor){
                       cordiv = $("<div><span class='badge badge-primary'>"+token.cor.trim()+"</span></div>");
                   //  cordiv = $("<div style='color:green;'>"+token.cor.trim()+"</div>");
                    }


                    var div = $("<div style='display:inline-block;'></div>").append(img).append(cordiv);

                    $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").find('.splitLine').append(div);
                    cnt++;
                    offset +=box.width;
              });
           $("#"+img_id).remove();

     });

    that.$el.modal('show');




});


     




  }

})



return Show;

});

