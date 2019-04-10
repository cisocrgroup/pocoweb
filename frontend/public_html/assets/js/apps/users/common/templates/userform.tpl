
<% if(asModal){%>

  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><%-modaltitle%></h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">×</span>
        </button>
       </div>
 
  <div class="modal-body">
       
<%} else {%>

<div class="container">
<div class="row">
 <div class="col col-md-12">

<%}%>

<div class="form-group row ">
    <label class="col-md-3 form-control-label required">Username</label>
    <div class="col-md-6">     
          <input class="form-control" name="name" type="text" value="<%-name%>" required="">
    </div>
    <div class="col-md-3 form-control-comment"> 
    </div>
  </div>

  <div class="form-group row ">
    <label class="col-md-3 form-control-label required">Email</label>
    <div class="col-md-6">
          <input class="form-control" name="email" type="email" value="<%-email%>" required="">
    </div>
    <div class="col-md-3 form-control-comment">              
    </div>
  </div>

   <div class="form-group row ">
    <label class="col-md-3 form-control-label">Institute</label>
      <div class="col-md-6">
    <input class="form-control" name="institute" type="text" value="<%-institute%>">
    </div>

    <div class="col-md-3 form-control-comment">
    
    </div>
  </div>

  <div class="form-group row ">
    <label class="col-md-3 form-control-label required">
              Password
          </label>
    <div class="col-md-6">

          <div class="input-group">
            <input class="form-control" name="password" type="password" value="<%-password%>" pattern=".{5,}" required="">
            <!-- <span class="input-group-btn">
              <button class="btn" type="button" data-action="show-password" data-text-show="Show" data-text-hide="Hide">
                Show
              </button>
            </span> -->
          </div>
   
    </div>
    <div class="col-md-3 form-control-comment">   
    </div>
  </div>
       
  <div class="form-group row ">
    <label class="col-md-3 form-control-label">
               Password (retype)
          </label>
    <div class="col-md-6">

          <div class="input-group">
            <input class="form-control " name="new_password" type="password" value="<%-password%>" pattern=".{5,}">
           <!--  <span class="input-group-btn">
              <button class="btn" type="button" data-action="show-password" data-text-show="Show" data-text-hide="Hide">
                Show
              </button>
            </span> -->
          </div>
    </div>
    <div class="col-md-3 form-control-comment">
    </div>
  </div>
 <% if(admincheck) {%>
 
    <div class="form-group form-check">
    <input type="checkbox" class="form-check-input" id="admin_check">
    <label class="form-check-label" for="admin_check">Administrator</label>
  </div>
  <% }%>

<% if(asModal){%>
 </div>
  <div class="modal-footer">
        <button type="button" class="btn btn-primary js-submit">Submit</button>
      </div>
    </div>

</div>
</div>

<%} else  { %>
<% if (create) {%>
<div class="row">
<div class="col-md-3"></div>
  <div class="col-md-6">
     <button type="button" style="float:right;" class="btn btn-primary js-submit">Submit</button>
  </div>
  </div>

<% } %>

</div>
</div>
</div>
<%}%>

