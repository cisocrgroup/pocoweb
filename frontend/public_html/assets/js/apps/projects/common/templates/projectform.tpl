


<%
if(asModal) {
%>

  <div class="modal-dialog modal-lg" role="document">
  <div class="modal-content">

<div class="modal-header red-border-bottom">
        <h3 class="modal-title"><%-text%> </h3>

        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<div class="loading_background" style="display: none;">
         <i class="fas fa-sync fa-spin fa-3x fa-fw"></i>
         <div class="loading_text_parent">
           <div class="loading_text"> <%-loading_text%> </div>
         </div>
</div>

<% } %>

<form id="uploadForm">

<% if(!add_book) { %>

<div class="form-group row">
  <div class="col-4">
    <label for="title">Title</label>
    <input class="form-control" type="text" value="<%-title%>" id="title" name="title">
  </div>
   <div class="col-4">
  <label for="author">Author</label>
    <input class="form-control" type="text" value="<%-author%>" id="author" name="author">
  </div>
   <div class="col-4">
  <label for="histPatterns">Additional historical patterns</label>
    <input class="form-control" type="text" value="<%-histPatterns%>"
	       id="histPatterns" name="histPatterns"
		   pattern="([^:,]*:[^:,]*(,[^:,]*:[^:,]*)*)?"
		   placeholder="[m1:h1[,m2:h2]...]">
  </div>

</div>

<div class="form-group row">
  <div class="col-4">
    <label for="language">Language</label>
    <select class="form-control" type="text" value="<%-language%>"
	        id="language" name="language">
     <%
     _.each(languages, function(language) { %>
        <option><%-language%></option>
     <% }); %>
    </select>
  </div>
 <div class="col-4">
    <label for="year">Year of publication</label>
    <input class="form-control" type="text" pattern="[0-9]{1,4}"
	       value="<%-year%>" id="year" name="year" placeholder="year">
  </div>
 <div class="col-4">
    <label for="year">Profiler URL</label><small> (leave empty if in doubt)</small>
    <input class="form-control" type="text" value="<%-profilerUrl%>" id="profilerUrl" name="profilerUrl" placeholder="external profiler URL">
  </div>
  </div>

<% } %>

<% if(!edit_project) { %>


<label for="file-upload" class="btn" style="margin-top:15px; background: #dddddd;">
 <i class="fas fa-file-upload"></i> Upload data (.zip)
</label>
<input id="file-upload"
	   type="file"
	   name="archive"
	   accept="application/zip"
	   style="display:none">

<div id="selected_file"></div>
 <button class="btn btn-primary blue-bg js-submit-project" type="submit"> <i class="fa fa-check" aria-hidden="true"></i> Submit</button>


<% } else { %>

 <button class="btn btn-primary blue-bg js-edit-project" type="submit"> <i class="fa fa-check" aria-hidden="true"></i> Update</button>

<% } %>

</form>



<%
if(asModal) {
%>

  </div>



 <div class="modal-footer">
 </div>



  </div>
  </div>

<% } %>
