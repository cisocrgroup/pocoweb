
<%
if(asModal) {
%>

  <div class="modal-dialog" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h5 class="modal-title">Role Selection</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
<div class="modal-body">

<% } %>

<div class="btn-group-vertical" role="group">
 <% _.each(model.get('roles'), function(item) { %>
      <a id="<%-item['id']%>_btn" href="" class="btn btn-primary btn-lg"><%-item['role']%></a>
<% }); %>
</div> 
</div> 

<%
if(asModal) {
%>

  </div> 

  </div>
  </div>

<% } %>
