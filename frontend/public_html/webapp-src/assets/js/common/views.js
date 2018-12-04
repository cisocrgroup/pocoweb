// ===============
// common/views.js
// ===============

define(["marionette","app","spin","spin.jquery","common/util","datatables",
"tpl!common/templates/listtemplate.tpl",
"tpl!common/templates/loadingtemplate.tpl",
"tpl!common/templates/loadingtemplateopc.tpl",
"tpl!common/templates/headertemplate.tpl",
"tpl!common/templates/cardheadertemplate.tpl",

"tpl!common/templates/cardhubtemplate.tpl",

"tpl!common/templates/layouttemplate.tpl",
"tpl!common/templates/errortemplate.tpl",
"tpl!common/templates/emptytemplate.tpl",
"tpl!common/templates/areyousure.tpl",
"tpl!common/templates/okdialog.tpl",
"tpl!common/templates/buttongroup.tpl",
"tpl!common/templates/bginfo.tpl",
"tpl!common/templates/footerpaneltemplate.tpl",
"tpl!common/templates/confirm.tpl",

	], function(Marionette,IPS_App,Spinner,SpinnerJQuery,Util,dtb,listTpl,loadingTpl,loadingOpcTpl,headerTpl,cardHeadTpl,cardhubTpl,layoutTpl,errorTpl,emptyTpl,areYouTpl,okTpl,infoPanelTpl,bgInfoTpl,footerPanelTpl,confirmTpl){

    var Views={};



	Views.LoadingBackdropOpc = Marionette.View.extend({
	template: loadingOpcTpl,

	title: "Loading Data",
	message: "Please wait, data is loading...",

	serializeData: function(){
		return {
			title: Marionette.getOption(this,"title"),
			message: Marionette.getOption(this,"message")
		}
	},

	onAttach: function(){
			var opts = {
		lines:13,
		length:8,
		width:3.7,
		radius:9,
		corners:1.0,
		rotate:0,
		trail:41,
		speed:1.1,
		direction:1,
		color:"#FFF",
		trail:60,
		shadow:false,
		hwaccel:false,
		className:"spinner",
		zIndex: 2e9,
		top:"27px",
		left:"28px"
		};

		var target = document.getElementById('spinner');
		var spinner = new Spinner(opts).spin(target);

	} // onAttach()

	});



Views.LoadingBackdrop = Marionette.View.extend({
	template:loadingTpl,
	title: "Loading Data",
	message: "Please wait, data is loading...",

	serializeData: function(){
		return {
			title: Marionette.getOption(this,"title"),
			message: Marionette.getOption(this,"message")
		}
	},
onAttach: function(){

		var opts = {
		lines:13,
		length:8,
		width:3.7,
		radius:9,
		corners:1.0,
		rotate:0,
		trail:41,
		speed:1.1,
		direction:1,
		color:"#FFF",
		trail:60,
		shadow:false,
		hwaccel:false,
		className:"spinner",
		zIndex: 2e9,
		top:"27px",
		left:"28px"
		};

		var target = document.getElementById('spinner');
		var spinner = new Spinner(opts).spin(target);

		} // onAttach()

	});



 Views.Header = Marionette.View.extend({
	template:headerTpl,

	title: "Header Title",
	
	isDefault:true,
	h2:true,
	 events: {
   "click .bg_info_btn_parent": "showInfoClicked",
   },


		serializeData: function(){
			return {
				title: Marionette.getOption(this,"title"),
				icon: Marionette.getOption(this,"icon"),
				color: Marionette.getOption(this,"color"),
				backgrounds: Marionette.getOption(this,"backgrounds"),
    			isDefault: Marionette.getOption(this,"isDefault"),
    			h2: Marionette.getOption(this,"h2"),
			}
		}
		

		 
	});



 Views.CardHeader = Marionette.View.extend({
	template:cardHeadTpl,

	title: "Header Title",
	
		serializeData: function(){
			return {
				title: Marionette.getOption(this,"title"),
				icon: Marionette.getOption(this,"icon"),
				color: Marionette.getOption(this,"color"),
				stroke: Marionette.getOption(this,"stroke"),
			}
		}
		

		 
	});




   Views.Error = Marionette.View.extend({
	template:errorTpl,
	className: "errorDiv",
	errortext: "Error",
	message: "an error occurred.",

		serializeData: function(){
			return {
				errortext: Marionette.getOption(this,"errortext"),
				message: Marionette.getOption(this,"message"),
    		    asModal: Marionette.getOption(this,"asModal")

			}
		},
		onAttach : function(){
		   if(this.options.asModal){

          this.$el.attr("ID","login-modal");
          this.$el.addClass("modal fade login-modal");
          this.$el.on('shown.bs.modal', function (e) {
           })

           this.$el.modal();
   		 }
	}


	});



   Views.DataTable = Marionette.View.extend({
    template: listTpl,

    headers: [{}],
	items:[{}],
    columns: [{}],
    header:"",
    className:"row bg_style",

	urlroot:"",

		serializeData: function(){
			return {
				headers: Marionette.getOption(this,"headers"),	
				urlroot: Marionette.getOption(this,"urlroot"),
				columns: Marionette.getOption(this,"columns"),
				items: this.collection.toJSON(),
     		    header:"",
     		    backBtn:""

			}
		},
	  onDomRefresh: function(){
	  var table = $('#table').DataTable({
	           pageLength:25,
	           autoWidth: false,
	           stateSave: true,
	           "columnDefs": [
   				 { "width": "30%", "targets": 0 },
        		 { "width": "10%", "targets": 0 }

  					]
	        });
	   

 
		     
		  }

  });


 Views.Icon_DataTable = Marionette.View.extend({
    template: listTpl,

    headers: [{}],
	items:[{}],
    columns: [{}],
    header:"",
    className:"",
    header_bg:"",
    border_color:"",
	urlroot:"",
	datatable_options:{},

	events:{
	'click .clickable-row' : 'row_clicked'
	},

		serializeData: function(){
			return {
				headers: Marionette.getOption(this,"headers"),	
				urlroot: Marionette.getOption(this,"urlroot"),
				columns: Marionette.getOption(this,"columns"),
        		header_bg: Marionette.getOption(this,"header_bg"),	
        		border_color: Marionette.getOption(this,"border_color"),	
				items: Marionette.getOption(this,"collection"),
     		    header:"",
     		    backBtn:"",
     		    datatable_options: Marionette.getOption(this,"datatable_options"),
     		    hover: Marionette.getOption(this,"hover")

			}
		},

		row_clicked : function(e){

			var url = $(e.currentTarget).attr('data-href')

			if(url=="#"){ var idx = $(e.currentTarget).attr('data-idx'); this.trigger('go:list_clicked',{idx:idx}); }
			else window.location = url;

		},

	  onDomRefresh: function(){


	  	var old_table_height = 0;

	  	this.datatable_options['initComplete'] = function(){

	           	   setTimeout(function() {old_table_height = $('#table tr').length;}, 1000);
	          
	           }
	       
	      var that = this;
	  	  var table = $('#table').DataTable(that.datatable_options);


	  	    var page_changed = false;
	  	    var length_changed = false;


	  	  	 table.on( 'length.dt', function () {
	  	  		length_changed = true;
		  	 });



	  	    table.on('draw.dt', function () {

	  	    if(old_table_height!=$('#table tr').length){	

	  	  			$(window).scrollTop(0);
	  	  			page_changed = false;

	  	  		 }

	  	     if(length_changed){
	  	     	length_changed = false;
	  	     } 


	  	     old_table_height=$('#table tr').length;

		  	 });

	   
		     
		  }

  });


 Views.RespTable = Marionette.View.extend({
    template: listTpl,
    header:[],
    headers: [{}],
	items:[{}],
    columns: [{}],
    className:"row bg_style",
    backBtn:false,
	urlroot:"",
	selfRef:false,
		serializeData: function(){
			return {
				headers: Marionette.getOption(this,"headers"),	
				urlroot: Marionette.getOption(this,"urlroot"),
				columns: Marionette.getOption(this,"columns"),
				items: this.collection.toJSON(),
				backBtn: Marionette.getOption(this,"backBtn"),
				header: Marionette.getOption(this,"header"),
				selfRef: Marionette.getOption(this,"selfRef")
			}
		}
  });






    Views.CardHub = Marionette.View.extend({
    template: cardhubTpl,
    cards:[],
    columns:2,
    currentRoute:undefined,
    events:{
    	'click .custom_card.active' : 'custom_card_clicked',
        'click button.js-back':   'backClicked',

    },
    custom_card_clicked: function(e){
    	var url = $(e.currentTarget).attr('url');
	 	this.trigger('cardHub:clicked',{url:url});

    },

     backClicked: function(e){
      e.preventDefault();
        window.history.back();
       // this.trigger("go:back");

     }, 

		serializeData: function(){
			return {
				cards: Marionette.getOption(this,"cards"),
		    	columns: Marionette.getOption(this,"columns"),
        		back_btn: Marionette.getOption(this,"back_btn"),

			}
		},


      balanceCards: function(){

		  var window_width = window.innerWidth;
			
					if(window_width>767){

						$('.card-block').height("");

						$('.card_row').each(function(){

						  var max_height=0;

						  $(this).find('.hub_col').each(function(){
						  	var current = $(this);
						  	if(current.find('.card').outerHeight()>max_height){
						  		max_height = current.find('.card').outerHeight();
						  	}
						  })


						$(this).find('.hub_col').each(function(){


								var current = $(this);
								var next = $(this).next();

								var current_height = current.find('.card').outerHeight();
								var next_height = next.find('.card').outerHeight();

								if(current_height<max_height){

									var diff = max_height - current_height;
									var new_height_block =  current.find('.card-block').outerHeight() + diff - 40;  //-40 padding
									current.find('.card-block').height(new_height_block);
								}

			

						})

					})


					}

				else{

				  $('.card-block').height("");

				}

		},

		onAttach: function(){

				var that = this;
				that.balanceCards();

				setTimeout(function() { that.balanceCards();}, 100); // TODO FIX TIMEOUT FOR RELOAD

				$(window).on('resize',function(){
					that.balanceCards();
				})

				$(window).scroll(function(){
					var op = 1 - ($(window).scrollTop() / 100);
					if(op<0.15)op=0.15;

				    $(".startlogocontainer").css("opacity",op);

					    if(op<0.85){
					   	 $(".cardhl").css("opacity",op);
					    }
				  });



		} // onAttach()
	
  });



Views.Layout = Marionette.View.extend({
	template: layoutTpl,
	 regions:{
	  headerRegion: "#hl-region",
      contentRegion: "#content-region",
      panelRegion: "#panel-region"

	}
	});



//----





 Views.ErrorLayout = Marionette.View.extend({
	errortext: "Error",
	message: "an error occurred.",

		serializeData: function(){
			return {
				errortext: Marionette.getOption(this,"errortext"),
				message: Marionette.getOption(this,"message")
			}
		},
		onRender: function(){
		   $('.alert-box').hide();

		},
		  onDomRefresh: function(){
		   $('.alert-box').hide();
		  $('.alert-box').show(500,function(){})
		  // $('.alert-box').show("slide", { direction: "left" }, 10000);
		  },
		  onShow:function(){
		  $('.alert-box').hide();
		  $('.alert-box').show(200,function(){})
		  }
		 
	});



  
	Views.AreYouSure = Marionette.View.extend({
    template: areYouTpl,
    events:{
     "click #js-yes":"yesClicked",
      "click #js-no":"noClicked",
    },

     onShow: function(){
      if(this.options.asModal){
      this.$el.addClass("reveal-modal small");
        this.$el.append('<a class="close-reveal-modal">&#215;</a>');
      this.$el.attr("data-reveal","");

          

      }
     }

	});

 Views.OkDialog = Marionette.View.extend({
	 template: okTpl,
	 events: {
	 "click .js-ok": "okClicked",

	 },

	 okClicked: function(e){
	 e.preventDefault();
    
	 },
	serializeData: function(){
			return {
		massagetitle: Marionette.getOption(this,"massagetitle"),
		massagecontent: Marionette.getOption(this,"massagecontent")
		}
	},
	 onShow: function(){
		 if(this.options.asModal){
		  this.$el.addClass("reveal-modal");
  		  this.$el.append('<a class="close-reveal-modal">&#215;</a>');
		  this.$el.attr("data-reveal","");

          

		}

  }


 });



  Views.Confirm = Marionette.View.extend({
   template: confirmTpl,
   events: {
   "click .js-confirm"        : "confirmClicked"
  ,"click .js-cancel" : "cancelClicked"
   },

    serializeData: function(){
      return {
    asModal: Marionette.getOption(this,"asModal"),
    model: Marionette.getOption(this,"model"),
    text:  Marionette.getOption(this,"text"), 
    title:  Marionette.getOption(this,"title"), 
    color:  Marionette.getOption(this,"color"), 

    }
  },

   onAttach: function(){

     if(this.options.asModal){

          this.$el.attr("ID","confirm-modal");
          this.$el.addClass("modal fade confirm-modal");
	 	  this.$el.on('shown.bs.modal', function (e) {

          })

          var that = this;
         
           this.$el.modal();
    }
     else {
       var $title = $('#formhl');
     $title.text(this.title);
    }
     
   }, // onAttach()
   confirmClicked: function(e){
     e.preventDefault();
     // $('#role-modal').modal('hide');
   },
   cancelClicked: function(e){
     e.preventDefault();
     this.$el.modal('hide');
   },
  
});

Views.FooterPanel = Marionette.View.extend({
     events:{
        "click button.js-back":   "backClicked",
      },
      backClicked: function(e){
      e.preventDefault();
       window.history.back();
      // this.trigger("go:back");
     }, 
      template: footerPanelTpl,
  });


return Views;


});





