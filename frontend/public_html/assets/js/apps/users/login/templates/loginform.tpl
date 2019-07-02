<%
if(asModal) {
%>
<div class="modal fade" id="loginModal" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
 <div class="modal-dialog">
 <div class="modal-content">
      <div class="modal-header">
            <legend>Login</legend>
              <div class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true"><i class="fas fa-times fa-xs"></i></span>
        </div>
      </div>
      <div class="modal-body">
 <% } %>

<form>
  <div class="form-group">
    <label for="email">Email address</label>
    <input type="email" class="form-control" id="email" name="email"aria-describedby="emailHelp" placeholder="Enter email">
  </div>
  <div class="form-group">
    <label for="password">Password</label>
    <input type="password" class="form-control" name="password" id="password" placeholder="Password">
  </div>

  <button type="submit" class="btn btn-primary js-loginsubmit">Submit</button>
</form>

<%
if(asModal) {
%>
</div>
</div>

</div>
</div>

 <% } %>