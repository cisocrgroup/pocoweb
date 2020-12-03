


    <div id="page-container">


	   <%
     _.each(lines, function(line) {

       var split_img = line["imgFile"].split("/");
  	   var imgbasename = split_img[4];
  	   var text = "line " + line['lineId'] + ", img" + imgbasename;

  	   var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];
  	   var correction_class="";
  	   var setlinehightlighting=false;
  	   if(line['isManuallyCorrected']){
  	  	 correction_class = "manually_corrected";
  	   }

      %>
      <div class="line-container">
       <div class="line-nr"> <span><%-line['lineId']%></span></div>
       <div class="text-image-line" title="<%-text%>">

       	<a class="line-anchor" id="line-anchor-<%-anchor%>"></a>
	   	<div class="line-img">
    	<img id="line-img-<%-anchor%>" src='<%-line["imgFile"]%>' alt='<%-text%>' title='<%-text%>' width="auto" height="<%-lineheight%>"></div>

		<div class="line-text-parent">
		<div id="line-<%-anchor%>" class="<%-correction_class%> line-text" anchor="<%-anchor%>">
       <div class="line" contenteditable="true">
        <%-line['cor']%>
      	</div>
      	<div class="line-tokens">
      	</div>


        </div>
        <span>
		      <div class="rounded-right btn btn-outline-dark correct-btn js-correct" title="correct line #<%-line['lineId']%>" anchor="<%-anchor%>"><i class="far fa-arrow-alt-circle-up"></i></div>
        </span>
	    </div>



       </div>

     </div>


      <% }) %>


	</div>

   