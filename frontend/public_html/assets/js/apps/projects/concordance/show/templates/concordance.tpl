	
<%
if(asModal) {
%>

  <div class="modal-dialog modal-xl" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h3 class="modal-title">Concordance view for</h3>
       
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } else { %>


	<div class="container">
	<div class="row">
    <div class="col col-md-12">

			<!-- #frontend_render_concordance_header_div(); -->
			<div id="concordance-heading">
			<!--<p><h2>Concordance view for '", urldecode($q), "'</h2></p>-->
			<p><h2>Concordance view for</h2></p>

			</div>

	
<% } %>

	

	</div>
    </div>
 	</div>
