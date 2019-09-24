// ======================================
// apps/project/show/show_controller.js
// ======================================

define([
  "app",
  "common/util",
  "common/views",
  "apps/projects/show/show_view"
], function(App, Util, Views, Show) {
  Controller = {
    showProject: function(id) {
      require(["entities/project", "entities/util", "entities/users"], function(
        ProjectEntities,
        UtilEntities,
        UserEntities
      ) {
        var loadingCircleView = new Views.LoadingBackdropOpc();
        App.mainLayout.showChildView("backdropRegion", loadingCircleView);
        var fetchingproject = ProjectEntities.API.getProject({ pid: id });
        var fetchinglanguages = UtilEntities.API.getLanguages();
        var fetchingprojects = ProjectEntities.API.getProjects();
        var fetchinguser = UserEntities.API.loginCheck();
        var fetchingjobs = ProjectEntities.API.getJobs({pid:id});

        $.when(
          fetchingproject,
          fetchinglanguages,
          fetchingprojects,
          fetchinguser,
          fetchingjobs
        )
        .done(function(project, languages, projects, user,jobs) {



          let isProject = project.attributes.bookId == project.attributes.projectId;
          let projectId = project.get('bookId');
          let isAdmin = user.admin;

            loadingCircleView.destroy();
            console.log(project);
            var projectShowLayout = new Show.Layout();
            var projectShowHeader;
            var projectShowInfo;
            var projectShowFooterPanel;
            // console.log(reviews);

            // only show packages of this project
            console.log(projects);

           if(jobs.statusName=="running"){ // start job tracking if job is running
            projectShowLayout.trackJobStatus();
            }

            var packages = [];
            for (var i = 0; i < projects.books.length; i++) {
              var book = projects.books[i];
              if (
                user.admin &&
                book.bookId != book.projectId &&
                book.bookId == id
              ) {
                packages.push(book);
              }
            }


            projectShowLayout.on("attach", function() {
              if (isProject) { // project view
              var cards = [
                {
                  color: "blue",
                  icon: "far fa-edit",
                  id: "adaptive_btn",
                  name: "Adaptive tokens",
                  seq: 5,
                  text: "List adaptive tokens.",
                  url: "adaptive"
                },
                {
                  color: "red",
                  icon: "fas fa-columns",
                  id: "about_btn",
                  name: "Split",
                  seq: 4,
                  text: "Split the project.",
                  url: "split"
                },
                {
                  color: "green",
                  icon: "far fa-edit",
                  id: "delete_button",
                  name: "Edit",
                  seq: 5,
                  text: "Edit the project.",
                  url: "edit"
                },
                {
                  color: "blue",
                  icon: "fas fa-download",
                  id: "test_btn",
                  name: "Download",
                  seq: 1,
                  text: "Save project files to disk.",
                  url: "download"
                },

                {
                  color: "red",
                  icon: "far fa-times-circle",
                  id: "about_btn",
                  name: "Delete",
                  seq: 4,
                  text: "Delete the project.",
                  url: "delete"
                },
                {
                  color: "blue",
                  icon: "fas fa-chevron-circle-left",
                  id: "takeback_btn",
                  name: "Reclaim packages",
                  seq: 5,
                  text: "Reclaim all packages.",
                  url: "takeback"
                }

              ];
                } else if (isAdmin) { // admin: package view
                  var cards = [
                {
                  color: "blue",
                  icon: "far fa-edit",
                  id: "adaptive_btn",
                  name: "Adaptive tokens",
                  seq: 5,
                  text: "List adaptive tokens.",
                  url: "adaptive"
                },
                {
                  color: "red",
                  icon: "far fa-times-circle",
                  id: "about_btn",
                  name: "Delete",
                  seq: 4,
                  text: "Delete the package.",
                  url: "delete"
                },
                {
                  color: "blue",
                  icon: "fas fa-user-tag",
                  id: "assign_btn",
                  name: "Assign",
                  seq: 5,
                  text: "Assign package to a user.",
                  url: "assign"
                }
              ];
                  } else { // normal user: package view
                  var cards = [
                {
                  color: "blue",
                  icon: "far fa-edit",
                  id: "adaptive_btn",
                  name: "Adaptive tokens",
                  seq: 5,
                  text: "List adaptive tokens.",
                  url: "adaptive"
                },
                {
                  color: "red",
                  icon: "far fa-times-circle",
                  id: "assign_btn",
                  name: "Reassign",
                  seq: 4,
                  text: "Reassign the package back to its owner.",
                  url: "reassign"
                }
              ];
                  }
              var cards2 = [
                {
                  color: "green",
                  icon: "fas fa-history",
                  id: "test_btn",
                  name: "Order Profile",
                  seq: 1,
                  text: "Start profiling the project.",
                  url: "profile"
                },
                {
                  color: "blue",
                  icon: "fas fa-cogs",
                  id: "test_btn",
                  name: "Automatic Postcorrection",
                  seq: 2,
                  text: "Fully automatic OCR postcorrection.",
                  url: "projects:a_pocoto"
                },
                {
                  color: "green",
                  icon: "fas fa-file-signature",
                  id: "users_button",
                  name: "Manual Postcorrection",
                  seq: 3,
                  text: "Manual interactive postcorrection tools.",
                  url: "projects:show_page"
                }
              ];

              var projectShowHub = new Show.Hub({
                columns: 3,
                cards: cards,
                currentRoute: "home"
              });
              var projectShowHub2 = new Show.Hub({
                columns: 3,
                cards: cards2,
                currentRoute: "home"
              });

              projectShowHub.on("cardHub:clicked", function(data) {
                if (data.url == "delete") {
                  this.trigger("show:delete_clicked");
                }
                if (data.url == "edit") {
                  this.trigger("show:edit_clicked");
                }
                if (data.url == "adaptive") {
                  this.trigger("show:adaptive");
                }
                if (data.url == "split") {
                  this.trigger("show:split");
                }
                if (data.url == "download") {
                  this.trigger("show:download");
                }
                if (data.url == "reassign") {
                  this.trigger("show:reassign");
                }
                if (data.url == "takeback") {
                  this.trigger("show:takeback");
                }
                if (data.url == "assign") {
                  this.trigger("show:assign");
                }
              });

              projectShowHub2.on("cardHub:clicked", function(data) {
                if (data.url == "projects:show_page") {
                  App.trigger("projects:show_page", id, "first");
                }
                if (data.url == "projects:a_pocoto") {
                  App.trigger("projects:a_pocoto", id);
                }
                if (data.url == "profile") {
                  this.trigger("show:profile");
                }
              });
              let icon = isProject ? 'fas fa-book-open' : 'fas fa-box-open';
              projectShowHeader = new Show.Header({
                title: project.get("title"),
                icon: icon,
                color: "green"
              });
              projectShowInfo = new Show.Info({ model: project, jobs:jobs });
              projectShowFooterPanel = new Show.FooterPanel({manual:true});

              projectShowFooterPanel.on('go:back',function(){
                App.trigger("projects:list");
              })

              var projectShowPackages = new Show.Packages({
                packages: packages
              });

              projectShowHub2.on("show:profile", function() {
                var profilingproject = ProjectEntities.API.profileProject({
                  pid: id,
                  tokens: []
                });

                $.when(profilingproject)
                  .done(function(result) {
                    console.log(result)
                    var confirmModal = new Show.OkDialog({
                      asModal: true,
                      title: "Profiling started",
                      text: "Profile for " + project.get("title") + " ordered.",
                      id: "profileModal"
                    });
                    App.mainLayout.showChildView("dialogRegion", confirmModal);

                       var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                      $.when(fetchingjobs).done(function(jobs) {
                        projectShowInfo.setJobSymbol(jobs);
                        projectShowLayout.trackJobStatus();

                      });

                  })
                  .fail(function(response) {
                    Util.defaultErrorHandling(response, "danger");
                  });
              });

              projectShowHub.on("show:adaptive", function() {
                let fetchingAdaptiveTokens = ProjectEntities.API.getAdaptiveTokens(
                  { pid: id }
                );
                $.when(fetchingAdaptiveTokens)
                  .done(function(tokens) {
                    if (tokens.adaptiveTokens==null){
                      return;
                    }
                    let projectShowAdaptiveTokens = new Show.Adaptive({
                      asModal: true,
                      pid: tokens.projectId,
                      adaptiveTokens: tokens.adaptiveTokens
                    });


                      projectShowAdaptiveTokens.on('attach',function(){
                      var ad_table = $('.adaptive-tokens').DataTable({
                           "scrollY": '500px',
                            "info":false,
                            "paging": false,
                            "lengthChange": false,
                          });

                    });
                    App.mainLayout.showChildView(
                      "dialogRegion",
                      projectShowAdaptiveTokens
                    );
                  })
                  .fail(function(response) {
                    Util.defaultErrorHandling(response, "danger");
                  });
              }),

              projectShowHub.on("show:reassign", function() {
                let fetchingReassign = ProjectEntities.API.assignPackageBack({pid: id});
                $.when(fetchingReassign)
                .done(function(ignore) {
                  App.trigger('projects:list');
                  App.mainmsg.updateContent(
                    "Successfully reassigned package " + id, 'success');
                })
                .fail(function(response) {
                  Util.defaultErrorHandling(response, "danger");
                });
              }),

              projectShowHub.on("show:takeback", function() {
                let fetchingTakeBack = ProjectEntities.API.takeBackPackages({pid: id});
                $.when(fetchingTakeBack)
                .done(function(ignore) {
                  App.trigger('projects:show', id);
                  App.mainmsg.updateContent(
                    "Successfully reclaimed all packages", 'success');
                })
                .fail(function(response) {
                  Util.defaultErrorHandling(response, "danger");
                });
              }),

              projectShowHub.on("show:assign", function() {
                let fetchingUsers = UserEntities.API.getUsers();
                $.when(fetchingUsers).done(function(users) {
                  let projectShowAssign = new Show.Assign({id: id, users: users});
                    App.mainLayout.showChildView(
                      "dialogRegion",
                      projectShowAssign
                    );
                  projectShowAssign.on("assign:confirmed", function(data) {
                    console.log(data);
                    let assigning = ProjectEntities.API.assignPackageTo({
                      pid: id,
                      uid: data.userId
                    });
                    $.when(assigning).done(function(ignored) {
                      $('#assignModal').modal('hide');
                      App.trigger('projects:show', projectId);
                    }).fail(function(response) {
                      Util.defaultErrorHandling(response, "danger");
                    });
                  });
                }).fail(function(response) {
                  Util.defaultErrorHandling(response, "danger");
                });
              }),

                projectShowHub.on("show:split", function() {
                  var fetchingusers = UserEntities.API.getUsers();

                  $.when(fetchingusers).done(function(users) {
                    let currentUser = App.getCurrentUser();
                    users.users.forEach(function(user, i){
                      if (user.id == currentUser.id) {
                        let tmp = users.users[0];
                        users.users[0] = user;
                        users.users[i] = tmp;
                      }
                    });
                    var projectsShowSplitProject = new Show.Split({
                      users: users.users,
                      model: project,
                      asModal: true,
                      text: "Split Project",
                      n: project.get("pages")
                    });
                    App.mainLayout.showChildView(
                      "dialogRegion",
                      projectsShowSplitProject
                    );
                    projectsShowSplitProject.on("split:confirmed", function(
                      data
                    ) {
                      console.log(data);
                      data["pid"] = id;
                      var splitingproject = ProjectEntities.API.splitProject(
                        data
                      );

                      $.when(splitingproject)
                        .done(function(result) {
                          $("#splitModal").modal("hide");
                          //   App.mainmsg.updateContent(result,'success');
                          var assign_data = { pairs: [] };
                          _.each(result.books, function(book, index) {
                            assign_data["pairs"].push({
                              uid: data.ids[index],
                              pid: book.projectId
                            });
                          });

                          var assigningprojects = ProjectEntities.API.assignPackages(
                            assign_data
                          );
                          $.when(assigningprojects).done(function(
                            assign_result
                          ) {
                            // show message and update table
                            for (var i = 0; i < result.length; i++) {
                              var string = "";
                              string +=
                                '<tr class="clickable-row" data-href="#projects/"' +
                                result[i]["pid"] +
                                "><td>";
                              string += "<td>" + result[i]["title"] + "</td>";
                              string +=
                                "<td>" + result[i]["language"] + "</td>";
                              string +=
                                "<td>" + result[i]["pages"] + "</td></tr>";

                              $("#book_table")
                                .find("tbody")
                                .append($(string));
                            }
                          }).fail(function(response) {
                         $("#splitModal").modal("hide");
                          Util.defaultErrorHandling(response, "danger");
                          });
                        })
                        .fail(function(response) {
                         $("#splitModal").modal("hide");
                          Util.defaultErrorHandling(response, "danger");
                        });
                    });
                  });
                });

              projectShowHub.on("show:download", function() {
                var downloadinggproject = ProjectEntities.API.downloadProject({
                  pid: id
                });

                $.when(downloadinggproject)
                  .done(function(result) {
                    console.log(result);
                    window.open("/" + result.archive);
                  })
                  .fail(function(response) {
                    Util.defaultErrorHandling(response, "danger");
                  });
              });

              projectShowHub.on("show:edit_clicked", function() {
                var projectsShowEditProject = new Show.ProjectForm({
                  model: project,
                  asModal: true,
                  text: "Edit Project",
                  edit_project: true,
                  loading_text: "Update in progress",
                  languages: languages.languages
                });

                projectsShowEditProject.on("project:update", function(data) {
                  project.set(data);
                  data.histPatterns = Util.escapeAsJSON(data.histPatterns);
                  console.log(data); //data.histPatterns
                  var puttingProject = ProjectEntities.API.updateProject({
                    pid: id,
                    projectdata: data
                  });

                  $.when(puttingProject).done(function(result) {
                    $(".loading_background").fadeOut();

                    $("#projects-modal").modal("toggle");
                    projectShowHeader.options.title = data.title;
                    projectShowHeader.render();
                    projectShowInfo.render();
                    App.mainmsg.updateContent(
                      "Successfully updated project " + id, 'success');
                  });
                });

                App.mainLayout.showChildView(
                  "dialogRegion",
                  projectsShowEditProject
                );
              });

              projectShowHub.on("show:delete_clicked", function() {
                var confirmModal = new Show.AreYouSure({
                  title: "Are you sure...",
                  text:
                    "...you want to delete project " +
                    project.get("title") +
                    " ?",
                  id: "deleteModal"
                });
                App.mainLayout.showChildView("dialogRegion", confirmModal);

                confirmModal.on("delete:confirm", function() {
                  var deletingProject = ProjectEntities.API.deleteProject({
                    pid: id
                  });
                  $("#deleteModal").modal("hide");

                  $.when(deletingProject)
                    .done(function(result) {
                      App.mainmsg.updateContent(
                        "Successfully deleted project " + id, 'success');
                      App.trigger("projects:list");
                    })
                    .fail(function(response) {
                      Util.defaultErrorHandling(response, "danger");
                    });
                });
              });

              
            projectShowLayout.on("show:checkJobStatus",function(){
                      var fetchingjobs = ProjectEntities.API.getJobs({pid:id});
                       $.when(fetchingjobs).done(function(jobs){
                                           
                            projectShowInfo.setJobSymbol(jobs);
                            console.log(jobs);
                           if(jobs.statusName!="running"){ // stop job tracking if job is not running
                            projectShowLayout.stopJobTracking();
                            }

                       }).fail(function(response){
                             App.mainmsg.updateContent(response.responseText,'danger');                                                 
                       }); 
                 });
                 

              // projectPanel = new Show.FooterPanel();

              projectShowLayout.showChildView(
                "headerRegion",
                projectShowHeader
              );
              projectShowLayout.showChildView("infoRegion", projectShowInfo);
              projectShowLayout.showChildView("hubRegion", projectShowHub2);
              projectShowLayout.showChildView("hubRegion2", projectShowHub);
              projectShowLayout.showChildView(
                "packagesRegion",
                projectShowPackages
              );

              projectShowLayout.showChildView(
                "footerRegion",
                projectShowFooterPanel
              );
            }); // on.attach()

            App.mainLayout.showChildView("mainRegion", projectShowLayout);
          })
          .fail(function(response) {
            loadingCircleView.destroy();
            Util.defaultErrorHandling(response, "danger");
          }); // $when fetchingproject
      }); // require
    }
  };

  return Controller;
});
