	
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


<div>
	  <%
      _.each(tokendata.matches, function(match) {
      console.log(match);
      var line = match['line'];
    	  _.each(match['tokens'], function(word) {

    var offset = word['offset'];
    var anchor = word['projectId']+"-"+word['pageId']+"-"+word['lineId']+"-"+word['tokenId'];
    var link = "#/projects/"+word['projectId']+"/page/"+word['pageId'];
    var inputclass = "";
      %>
<div class="text-image-line row">

<div class="left_div div_inline">
	<!-- if ($images["leftImg"] != NULL) { -->
		<a class="invisible=link" href="<%-link%>">
		<img src="<%-line['leftImg']%>" width="auto" height="25"/>
		<br/>
		<label>
			<%
			 var left_side = line['cor'].trim();
			 left_side = left_side.substring(0,offset);
			%>
			<%-left_side%>
			</label>
		</a>
	<!-- } -->
	</div>
	<div class="middle_div div_inline">
	<!-- if ($images["middleImg"] != NULL) {
		$inputclass = frontend_get_correction_class($word); -->
		<a class="invisible=link" href="<%-link%>">
		<img src="<%-line['middleImg']%>" width="auto" height="25"/>

		</a>
		<br/>



		<div class="input-group">
	    <!-- checkbox -->
		<div class="input-group-prepend">
			<div class="input-group-text">
				<input id="concordance-token-checkbox-<%-anchor%>"	type="checkbox" aria-label="...">
			</div>
		</div>

		<!--  input -->
		<input id="concordance-token-input-<%-anchor%>" class="form-control <%-inputclass%>" type="text" value="<%-word['cor']%>" title="Correction"/>
		<div class="input-group-append">
		    <button class="btn btn-outline-secondary dropdown-toggle" style="border-right:0; border-top-right-radius: 0;border-bottom-right-radius: 0;" type="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false"></button>
		<div id="concordance-token-suggestions-<%-anchor%>" class="dropdown-menu dropdown-menu-right">
			<%
				_.each(suggestions, function(s) {                
                      var content = s.suggestion + " (patts: " + s.ocrPatterns.join(',') + ", dist: " +
                      s.distance + ", weight: " + s.weight.toFixed(2) + ")";
                      %>
                    <a class="dropdown-item noselect"><%-content%></a>
              <% }); %>
		</div>
		  <button class=" btn btn-outline-secondary js-correct" title="correct line #<%line['lineId']%>" anchor="<%-anchor%>"><i class="far fa-arrow-alt-circle-up"></i></button>
	
		</div>


		</div>

	</div>
	<div class="right_div div_inline">
	<!-- if ($images["rightImg"] != NULL) { -->
		<a class="invisible=link" href="<%-link%>">
		<img src="<%-line['rightImg']%>" width="auto" height="25"/>

		<br/>
		<label>
			<%
			 var right_side = line['cor'].trim();
			 right_side = right_side.substring(offset+1,line['cor'].length+1);
			%>
			<%-right_side%>
			</label>
		</a>
	<!-- } -->
	</div>
	</div>


     <%
     		});
     	});
     %>

</div>
</div>
	</div>
    </div>
 	</div>
