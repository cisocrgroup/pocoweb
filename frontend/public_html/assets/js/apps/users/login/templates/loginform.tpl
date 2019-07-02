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
 <% } else {%>
 <div class="container">
  <div class="row" style="padding-bottom:50px;">
        <div class="col col-sm-12"><div class="defaulthl"> <span> <i class="fas fa-sign-in-alt card_main_icon red" aria-hidden="true"></i>
         Login </span>
         </div>
         </div>
       <div class="col col-sm-4"></div>
      <div class="col col-sm-4">

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

 <% } else { %>

</div>
<div class="col col-sm-4"></div>

</div>


   <div class="row">

  <div class="col col-md-12">
  <button class="btn back_btn js-back btn-primary hover"> <i class="fa fa-caret-left" aria-hidden="true"></i> Back</button>


  </div>
  </div>

</div>

<% } %>