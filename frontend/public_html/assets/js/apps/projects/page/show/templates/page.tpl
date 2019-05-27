	<div class="container">
	<div class="row">
    <div class="col col-md-12">
	<ul class="nav sticky-top navbar-light justify-content-center" style="background-color: white; margin-top: 15px;margin-bottom: 15px;">
	<li class="nav-item js-firstpage"><a class="nav-link" href="#" title="go to first page">
		<i class="fas fa-angle-double-left"></i>
		</a></li>
	<li class="nav-item js-stepbackward"><a class="nav-link" href="#" title="go to previous page #<%-prevPageId%>">
		<i class="fas fas fa-angle-left"></i>
		</a></li>

	<!--error-patterns -->
	<li class="nav-item dropdown">
	<a href="#" class="dropdown-toggle nav-link" id="pcw-error-patterns-link" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false" data-flip="false" data-target="#pcw-error-patterns-dropdown">
		Error patterns<span class="caret"></span></a>
        <div id="pcw-error-patterns-dropdown" class="dropdown-menu scrollable-menu" aria-labelledby="pcw-error-patterns-link">
        </div>
    </li>
	<!-- error-tokens -->
	<li class="nav-item dropdown"> 
	<a href="#" class="dropdown-toggle nav-link" id="pcw-error-tokens-link" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false" data-flip="false" data-target="#pcw-error-tokens-dropdown">
		Error tokens<span class="caret"></span></a>
		 <div class="dropdown-menu scrollable-menu" id="pcw-error-tokens-dropdown" aria-labelledby="pcw-error-tokens-link">
        </div>
     </li>

   

	<!--nextpage and last page -->
	<li class="nav-item js-stepforward"><a class="nav-link"  href="#" title="go to next page #<%-nextPageId%>">
		<i class="fas fa-angle-right"></i>
		</a></li>
	<li class="nav-item js-lastpage"><a class="nav-link" href="#" title="go to last page">
		<i class="fas fa-angle-double-right"></i>
		</a></li>
	</ul>

	<div class="defaulthl" style="line-height:1; margin-top:15px;">
 <!--    <i class="fas fa-book-open card_main_icon green"></i>
	Project <%-projectId%> -->
	<div style="font-size: 20px; margin-top: 10px;"> Page <%-pageId%></div>
	</div>

	   <%
     _.each(lines, function(line) {

  
       var split_img = line["imgFile"].split("/");
  	   var imgbasename = split_img[4];
  	   var text = "line " + line['lineId'] + ", " + imgbasename;
  	   var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];
  	   var inputclass = Util.get_correction_class(line);
  	   var correction_class="";
  	   var setlinehightlighting=false;
  	   if(line['isFullyCorrected']){
  	  	 correction_class = "line_fully_corrected";
  	   }
  	   else if(line['isPartiallyCorrected']){
  	  	 correction_class = "line_partially_corrected";
  	   }


      %>
       <div class="text-image-line" title="<%-text%>">

       	<a class="line-anchor" id="line-anchor-<%-anchor%>"></a>
		<div class="line-img"><img id="line-img-<%-anchor%>" src='<%-line["imgFile"]%>' alt='<%-text%>' title='<%-text%>' width="auto" height="30"></div>

		<div class="line-text-parent">	
		<div id="line-<%-anchor%>" class="<%-correction_class%> line-text" anchor="<%-anchor%>"
	    class="<%-inputclass%> line-text">
       <div class="line" contenteditable="true">
       
        <%-line['cor']%>
      	</div> 
      	<div class="line-tokens">
      	</div>


        </div>
        <span>
		      <div class="rounded-right btn btn-outline-dark correct-btn js-correct" title="correct line #<%line['lineId']%>" anchor="<%-anchor%>"><i class="far fa-arrow-alt-circle-up"></i></div>
        </span>
	    </div>



       </div>
	
     


      <% }) %>
	</div>
    </div>
 	</div>
