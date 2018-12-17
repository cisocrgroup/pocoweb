


<%
if(asModal) {
%>

  <div class="modal-dialog" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h5 class="modal-title">User Login</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
<div class="modal-body">

<% } %>




<form novalidate="novalidate">

<% if(!asModal){ %>
  <fieldset>
    <legend>User Login</legend>

<% } %>

   <div class="row">
      <div class="col col-md-12">
      <div class="form-group">
        <label for="username">Username </label>
         <input type="text"  id="username" class="form-control" name="username" placeholder="Username" autocomplete="off">
         <small class="error" style="display: none;"></small>
       </div>
      </div>

     </div>

    <div class="row">
      <div class="col col-md-12">
        <div class="form-group">
        <label for="password">Password  </label>
          <input type="password" id="password"  class="form-control" placeholder="Password" name="password">
          <small class="error" style="display: none;"></small>
        </div>
      </div>


     </div>

    <div class="row">
      <div class="col col-md-12">
        <hr>
      </div>
    </div>

  
    <div class="row">
      <div class="col col-md-12">
        <button type="submit" class="js-submit btn btn-primary" id="login_submit_button">Login</button>
      </div>
    </div>



  <% if(!asModal){ %> </fieldset> <% } %>

</form>


<%
if(asModal) {
%>

  </div> 

  </div>
  </div>

<% } %>