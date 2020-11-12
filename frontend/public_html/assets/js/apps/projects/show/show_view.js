// ================================
// apps/projects/show/show_view.js
// ================================

define([
  "marionette",
  "app",
  "backbone.syphon",
  "common/views",
  "common/util",
  "apps/projects/common/views",
  "apps/projects/page/show/show_view",
  "apps/projects/concordance/show/show_view",
  "tpl!apps/projects/show/templates/layout.tpl",
  "tpl!apps/projects/show/templates/info.tpl",
  "tpl!apps/projects/show/templates/split.tpl",
  "tpl!apps/projects/show/templates/adaptive.tpl",
  "tpl!apps/projects/show/templates/assign.tpl",
  "tpl!apps/projects/show/templates/packages.tpl"
], function(
  Marionette,
  App,
  BackboneSyphon,
  Views,
  Util,
  CommonViews,
  Page,
  Concordance,
  layoutTpl,
  infoTpl,
  splitTpl,
  adaptiveTpl,
  assignTpl,
  packagesTpl
) {
  var Show = {};

  Show.Layout = Marionette.View.extend({
    template: layoutTpl,
    regions: {
      headerRegion: "#hl-region",
      infoRegion: "#info-region",
      hubRegion: "#hub-region",
      hubRegion2: "#hub2-region",
      packagesRegion: "#packages-region",
      footerRegion: "#footer-region"
    },
      trackJobStatus : function(){ // regulary check if job is finished
      var that = this;
        this.interval = setInterval(function(){ 
           that.trigger("show:checkJobStatus");
          },
          5000);
      },
      stopJobTracking : function(){
         clearInterval(this.interval);
      },
      onDestroy : function(){
         clearInterval(this.interval);
      }
  });

  Show.Header = Views.Header.extend({
    initialize: function() {
      this.title = "projects Prototype Tests";
    }
  });

  Show.Hub = Views.CardHub.extend({});

  Show.Info = Marionette.View.extend({
    template: infoTpl,
    events: {
      "click .js-edit-project": "edit_clicked",
      "click .js-delete-project": "delete_clicked",
      "click .js-add-book": "add_book_clicked"
    },
    serializeData: function() {
      var data = Backbone.Marionette.View.prototype.serializeData.apply(
        this,
        arguments
      );

      return data;
    },

    edit_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:edit_clicked");
    },

    delete_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:delete_clicked");
    },

    add_book_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:add_book_clicked");
    },
    onAttach: function() {
      var table = $("#book_table").DataTable();
      console.log(this)
      let status = this.model.attributes.status;

      if(status['profiled']){
        Util.removeStatusClasses($('.profile_icon'));
        $('.profile_icon').addClass("green").attr('title','Profiling successful');;
      }
      if(status['post-corrected']){
        Util.removeStatusClasses($('.post_cor_icon'));
        $('.post_cor_icon').addClass("green").attr('title','Post Correction successful');
      }
      if(status['extended-lexicon']){
        Util.removeStatusClasses($('.le_icon'));
        $('.le_icon').addClass("green").attr('title','Lexicon Extension successful');

      }

      var jobs = Marionette.getOption(this, "jobs");
      this.setJobSymbol(jobs);

    },
  
    setJobSymbol:function(jobs){
       if(jobs.statusName=="running"){
        if(jobs.jobName=="profiling"){
           Util.removeStatusClasses($('.profile_icon'));
           $('.profile_icon').addClass("yellow").attr('title','Profiling in progress');;
        }
        if(jobs.jobName=="calculating extended lexicon"){
           Util.removeStatusClasses($('.le_icon'));
           $('.le_icon').addClass("yellow").attr('title','Lexcion Extension in progress');;
        }
        if(jobs.jobName=="calculating post-correction"){
           Util.removeStatusClasses($('.post_cor_icon'));
           $('.post_cor_icon').addClass("yellow").attr('title','Post Correction in progress');;
        }
      }

       if(jobs.statusName=="failed"){
        if(jobs.jobName=="profiling"){
           Util.removeStatusClasses($('.profile_icon'));
           $('.profile_icon').addClass("red").attr('title','Profiling failed');;
        }
        if(jobs.jobName=="calculating extended lexicon"){
           Util.removeStatusClasses($('.le_icon'));
           $('.le_icon').addClass("red").attr('title','Lexcion Extension failed');;
        }
        if(jobs.jobName=="calculating post-correction"){
           Util.removeStatusClasses($('.post_cor_icon'));
           $('.post_cor_icon').addClass("red").attr('title','Post Correction failed');;
        }
      }

    if(jobs.statusName=="done"){
        if(jobs.jobName=="profiling"){
           Util.removeStatusClasses($('.profile_icon'));
           $('.profile_icon').addClass("green").attr('title','Profiling successful');;
        }
        if(jobs.jobName=="calculating extended lexicon"){
           Util.removeStatusClasses($('.le_icon'));
           $('.le_icon').addClass("green").attr('title','Post Correction successful');;
        }
        if(jobs.jobName=="calculating post-correction"){
           Util.removeStatusClasses($('.post_cor_icon'));
           $('.post_cor_icon').addClass("green").attr('title','Lexicon Extension successful');;
        }
      }
    }

  });

  Show.Packages = Marionette.View.extend({
    template: packagesTpl,
    events: {
      "click .clickable-row": "row_clicked"
    },
    row_clicked: function(e) {
      e.stopPropagation();
      var url = $(e.currentTarget).attr("data-href");

      if (url == "#") {
        var idx = $(e.currentTarget).attr("data-idx");
        this.trigger("go:list_clicked", { idx: idx });
      } else window.location = url;
    },

    serializeData: function() {
      return {
        packages: Marionette.getOption(this, "packages")
      };
    },

    edit_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:edit_clicked");
    },

    delete_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:delete_clicked");
    },

    add_book_clicked: function(e) {
      e.preventDefault();
      this.trigger("show:add_book_clicked");
    },
    onAttach: function() {
      var table = $("#packages_table").DataTable();
    }
  });

  Show.FooterPanel = Views.FooterPanel.extend({
   manual:true,title:"Back to Projects <i class='fas fa-list card_main_icon' aria-hidden='true'></i>"
  });

  Show.Missingprojects = Views.Error.extend({
    errortext: "Error 404: projects not found"
  });
  Show.ProjectForm = CommonViews.ProjectForm.extend({});
  Show.AreYouSure = Views.AreYouSure.extend({
    triggers: {
      "click .js-yes": "delete:confirm"
    }
  });

  Show.OkDialog = Views.OkDialog.extend({});
  Show.Adaptive = Marionette.View.extend({
    template: adaptiveTpl,
    serializeData: function() {
      return {
        pid: Marionette.getOption(this, "pid"),
        adaptiveTokens: Marionette.getOption(this, "adaptiveTokens")
      };
    },
    onAttach: function() {
      this.$el.addClass("modal fade");
      this.$el.attr("tabindex", "-1");
      this.$el.attr("role", "dialog");
      this.$el.attr("id", "splitModal");
      $("#splitModal").modal();
      var that = this;
    }
  });

  Show.Split = Marionette.View.extend({
    template: splitTpl,
    events: {
      "click .js-confirm": "confirmClicked",
      "click .js-addpackage": "addSplitrow",
      "click .js-close": "closeClicked"
    },
    serializeData: function() {
      return {
        title: Marionette.getOption(this, "title"),
        text: Marionette.getOption(this, "text"),
        id: Marionette.getOption(this, "id"),
        n: Marionette.getOption(this, "n")
      };
    },
    confirmClicked: function() {
      var n = $(".userrows").find(".row").length;
      var random = $("#checkRnd").is(":checked");
      var userIds = [];
      $(".userrows").find(".row").each(function() {
          var selected = $(this)
            .find("select")
            .val();
          userIds.push(Number(selected));
        });
      this.trigger("split:confirmed", {
        userIds: userIds,
        random: random
      });
    },
    addSplitrow: function() {
      var pages = Marionette.getOption(this, "n");
      var length = $(".userrows").find(".row").length;
      //only add max pages rows
      if (length == pages) {
        return;
      }

      var row = $(
        '<div class="form-group"><i class="js-close close-x fa fa-times"></i><div class="row"><div class="col-6"><label id="splitLabel" for="splitPage">Select User</label><select class="form-control"></select></div></div></div>'
      );
      var users = Marionette.getOption(this, "users");

      $(".userrows").prepend($("<hr>"));
      $(".userrows").prepend(row);
      for (i in users) {
        row.find("select")
        .append(
          $(
            '<option value="' +
              users[i].id +
              '">' +
              users[i].name +
              "</option>"
          )
        );
      }
    },
    closeClicked: function(e) {
      $(e.currentTarget)
        .parent()
        .next()
        .remove();

      $(e.currentTarget)
        .parent()
        .remove();
    },

    onAttach: function() {
      this.$el.addClass("modal fade");
      this.$el.attr("tabindex", "-1");
      this.$el.attr("role", "dialog");
      this.$el.attr("id", "splitModal");

      $("#splitModal").modal();
      var that = this;

      // $("#split-n").on('input',function(){
      //   var n = $(this).val();
      //   $('#splitLabel').text("Split into " + n + " pages");
      // })

      //  $('#checkRnd').change(function() {
      //     if($(this).is(":checked")) {
      //       $("#split-n").attr('disabled',true);
      //       $('#splitLabel').text("Split randomly");
      //     }
      //     else{
      //       $("#split-n").attr('disabled',false);
      //       var n = $("#split-n").val();
      //       $('#splitLabel').text("Split into " + n + " pages");
      //    }
      // });
    }
  });
  Show.Assign = Marionette.View.extend({
    template: assignTpl,
    events: {
      "click .js-confirm": "confirmClicked"
    },
    serializeData: function() {
      return {
        users: Marionette.getOption(this, "users"),
        id: Marionette.getOption(this, "id")
      };
    },
    confirmClicked: function() {
      var selected = this.$el.find(".form-control").val();
      this.trigger("assign:confirmed", {
        userId: selected
      });
    },
    onAttach: function() {
      this.$el.addClass("modal fade");
      this.$el.attr("tabindex", "-1");
      this.$el.attr("role", "dialog");
      this.$el.attr("id", "assignModal");
      $("#assignModal").modal();
      var that = this;
    }
  });

  return Show;
});
