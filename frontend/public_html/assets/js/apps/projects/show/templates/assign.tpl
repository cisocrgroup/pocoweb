  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title">Assign package <%-id%></h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        <form class="assignform">
		  <div class="col-6">
			<label id="assignLabel" for="assignPackage">Select User</label>
			<select class="form-control">
			  <% _.each(users.users, function(user) { %>
			  <option value="<%-user.id%>"><%-user.name%></option>
			  <% }); %>
			  </select>
			</div>
      </form>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-primary js-confirm">Assign</button>
        <button type="button" class="btn btn-secondary" data-dismiss="modal">Cancel</button>
	</div>
    </div>
  </div>
