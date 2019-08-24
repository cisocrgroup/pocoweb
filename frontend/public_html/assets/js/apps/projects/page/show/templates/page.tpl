

<div class="row">
<div class="col col-sm-3">
  <div class="form-group">
  <label for="line_size_slider"> Line Height </label>
   <input  type="range" min="1" max="60" value="30" class="slider form-control" id="line_size_slider">
</div>
</div>
<div class="col col-sm-3">
  <div class="form-group">
  <span for="line_nr_toggle"> Line Numbers </span>
  <div><input type="checkbox" id="line_nr_toggle" name="line_nr_toggle" checked></div>
 </div>
</div>

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
       <div class="line-nr"> <span><%-line['lineId']%></span></div>
       <div class="text-image-line" title="<%-text%>">

       	<a class="line-anchor" id="line-anchor-<%-anchor%>"></a>
	   	<div class="line-img">
    	<img id="line-img-<%-anchor%>" src='<%-line["imgFile"]%>' alt='<%-text%>' title='<%-text%>' width="auto" height="<%-lineheight%>"></div>

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
