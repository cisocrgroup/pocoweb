  
  <div class="slidecontainer">
<input type="range" min="1" max="100" value="50" class="slider" id="line_size_slider">
</div>


    <div id="page-container" class="border rounded">


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
      <div class="line-container">
       <div class="line-nr"> <%-line['lineId']%></div>
       <div class="text-image-line" title="<%-text%>">

       	<a class="line-anchor" id="line-anchor-<%-anchor%>"></a>
	   	<div class="line-img">   
    <img id="line-img-<%-anchor%>" src='<%-line["imgFile"]%>' alt='<%-text%>' title='<%-text%>' width="auto" height="30"></div>

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
		      <div class="rounded-right btn btn-outline-dark correct-btn js-correct" title="correct line #<%-line['lineId']%>" anchor="<%-anchor%>"><i class="far fa-arrow-alt-circle-up"></i></div>
        </span>
	    </div>



       </div>

     </div>


      <% }) %>
	</div>
