	
<%
if(asModal) {
%>

  <div class="modal-dialog modal-xl" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h3 class="modal-title">Concordance view for "<%-tokendata.query%>"</h3>
       
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } else { %>


	<div class="container">
	<div class="row">
    <div class="col col-md-12">

			<div id="concordance-heading">
			<p><h2>Concordance view for "<%-tokendata.query%>"</h2></p>
			</div>

	
<% } %>


	  <%
      _.each(tokendata.matches, function(match) {
      var line = match['line'];
    	  _.each(match['tokens'], function(word) {

    var offset = word['offset'];
    var anchor = word['projectId']+"-"+word['pageId']+"-"+word['lineId']+"-"+word['tokenId'];
    var link = "#/projects/"+word['projectId']+"/page/"+word['pageId'];
    var inputclass = "";
   
      %>



<div class="text-image-line">

<div class="left_div div_inline">
	<!-- if ($images["leftImg"] != NULL) { -->
		<div class="invisible=link" href="<%-link%>">
    <div id ="img_<%-line['pageId']%>_<%-line['lineId']%>_parent">
		<img src="<%-line['imgFile']%>" id="img_<%-line['pageId']%>_<%-line['lineId']%>" width="auto" height="25"/>
    </div>
	
		</div>
	</div>
	
	</div>


     <%
     		});
     	});
     %>


	</div>
    </div>
 	</div>