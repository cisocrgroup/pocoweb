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
      'click .cordiv' : 'cordiv_clicked',
      'dblclick .cordiv' : 'cordiv_dbclicked'
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
  cordiv_clicked:function(e){
    $(e.currentTarget).find('span').toggleClass('cor_selected');
  },
  cordiv_dbclicked:function(e){
        console.log('dbclick')
      if($(e.currentTarget).hasClass('cordiv')){
        console.log("ASDASDSß")
       $(".custom-popover").remove();

      var checkbox = $('<span class="correction_box"><i class="far fa-square"></i></span>'); 
    //  $(e.currentTarget).find('span').append(checkbox);

      // btn_group.append($('<div class="input-group-prepend"><div class="input-group-text"><input type="checkbox" id="js-select"></div></div>'))
      // btn_group.append($('<input type="text" class="form-control" id="corinput">'))
      // btn_group.append($('<div class="input-group-append" style="background:white;"><button type="button" class="btn btn-outline-secondary dropdown-toggle"></button><button type="button" class="btn btn-outline-secondary"><i class="far fa-arrow-alt-circle-up"></i></button></div>'))

      // var div = $('<div class="custom-popover">')
      // .css({
      //   "left": e.pageX + 'px',
      //   "top": (e.pageY+35) + 'px'
      // })
      //  .append($('<div><i class="fas fa-caret-up custom-popover-arrow"></i></div>'))
      //  .append(btn_group)
      //  .appendTo(document.body);

      //  $('#js-concordance').on('click',function(){
      //   that.trigger("page:concordance_clicked",sel);
      //  });

      
      }
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
 
          console.log(tokendata);
           _.each(tokendata.matches, function(match) {
            var line = match['line'];
            var linetokens = line.tokens;
            var splitImgLine = $('<div class="splitLine"></div>');
            $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").append(splitImgLine);

            var cnt = 0;
            var current_position = 0;
            var img_id = "img_"+line['pageId']+"_"+line['lineId'];
            var line_img = document.getElementById(img_id);

            var scalefactor = line_img.width / line.box.width;
            var prevdiv;
             // linecor = Util.replace_all(linecor,word['cor'],'<span class="badge badge-pill badge-primary">'+word['cor']+'</span>');

              for(var i=0;i<linetokens.length;i++) {

                var token = linetokens[i];

              
                var whitespace_width=0; 
                if (token.ocr.includes(" ")){
                  whitespace_width = token.box.width;
                }

                var box = token['box'];

                // var line_img = document.getElementById(img_id);
                // var boxwidth = box.width + whitespace_length;
                // console.log(line_img);

                // console.log(token);

                // var c = document.createElement("canvas");
                // c.width=boxwidth;
                // c.height=box.height;

                //   var ctx = c.getContext("2d");
                //   ctx.drawImage(line_img, offset, 0, boxwidth, box.height, 0, 0, boxwidth, box.height);

                //     var img = new Image();
                //     img.src = c.toDataURL();
                //     img.setAttribute('id','splitImg_'+line['pageId']+"_"+line['lineId']+"_"+cnt);
                //     img.height = '25';

                    $('#splitImg_'+line['lineId']+"_"+cnt).css('width','auto');

                    
                    var cordiv = $("<div>"+token.cor+"</div>");

                    if(query==token.cor){
                       cordiv = $("<div class='cordiv' contenteditable='true'><span class='badge badge-primary'>"+token.cor.trim()+"</div></span>");
                       //var grp = $ ("<div class='input-group-mb-3'></div>");
                       // grp.append($("<span class='concbtn_left'><i class='far fa-square'></i></span>"));
                       // grp.append($("<span class='cortoken' contenteditable='true'>"+token.cor.trim()+"</span>"));
                       // grp.append($("<span class='concbtn_right'><i class='fas fa-caret-down'></i></span>"));

                       // cordiv.find('span').append(grp);
                      //    
                   //  cordiv = $("<div style='color:green;'>"+token.cor.trim()+"</div>");
                    }

                  

                    // var div = $("<div style='display:inline-block;'></div>").append(img).append(cordiv);
                    var div = $('<div class="tokendiv"></div>').append(cordiv);

                    $('#img_'+line['pageId']+"_"+line['lineId']+"_parent").find('.splitLine').append(div);
                    cnt++;

                       // if (token.ocr.includes(" ")){


                        var whitespace_div_length = token.box.width*scalefactor ;
                         cordiv.css('width',whitespace_div_length);
                       

                        prevdiv = cordiv;
                       //  current_position+=(prev_div_width + whitespace_div_length);
                       // }
                     
                      

               }
           // $("#"+img_id).remove();

     });

    that.$el.modal('show');




});


     




  }

})



return Show;

});

