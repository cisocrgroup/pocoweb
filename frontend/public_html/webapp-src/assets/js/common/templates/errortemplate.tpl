<%
if(asModal) {
%>

  <div class="modal-dialog" role="document">
  <div class="modal-content red-border">

<div class="modal-header red-border">
        <h3 class="modal-title red"><i class="fa fa-exclamation-triangle" aria-hidden="true"></i> Error </h3>
       
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } %>



<div class="alert alert-danger <%if(asModal){%> modal_errortext <%} else {%> errortext<%}%>" role="alert">
  <%- errortext %> 
</div>



<%
if(asModal) {
%>

  </div> 

  </div>
  </div>

<% } %>
