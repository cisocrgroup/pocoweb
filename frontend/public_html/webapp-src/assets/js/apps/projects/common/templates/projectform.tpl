


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
         <i class="fa fa-circle-o-notch fa-spin fa-3x fa-fw"></i>
         <div class="loading_text_parent">
           <div class="loading_text"> <%-loading_text%> </div>
         </div>
</div>

<% } %>

<form>

<% if(!add_book) { %>

<div class="form-group row">
  <div class="col-6">
    <label for="title">Title</label>
    <input class="form-control" type="text" value="<%-title%>" id="title" name="title">
  </div>
   <div class="col-6">
  <label for="author">Author</label>
    <input class="form-control" type="text" value="<%-author%>" id="author" name="author">
  </div>

</div>

<div class="form-group row">
  <div class="col-6">
    <label for="language">Language</label>
    <input class="form-control" type="text" value="<%-language%>" id="language" name="language">
  </div>
 <div class="col-6">
    <label for="year">Year</label>
    <input class="form-control" type="text" value="<%-year%>" id="year" name="year">
  </div>
 
  </div>

<% } %>

<% if(!edit_project) { %>


<div class="form-group row">


  <div class="col-3">
  <label for="ocrEngine">OCR Engine</label>

    <select style="width: 100%" class="form-control" id="ocrEngine">
      <option value="ocropus">Ocropus</option>
      <option value="tesseract">Tesseract</option>
      <option value="abbyy_finereader">Abbyy Finereader</option>

    </select>
  </div>

</div>




<div class="form-group row">
<div class="col-6">



<label for="file-upload" class="btn" style="margin-top:15px; background: #dddddd;">
 <i class="fa fa-file-archive-o" aria-hidden="true"></i> Upload data (.zip)
</label>
<input id="file-upload" type="file" style="display:none">

<div id="selected_file"></div>

</div>
</div>

<% } %>

</form>



<%
if(asModal) {
%>

  </div> 



 <div class="modal-footer">
      <button class="btn no_bg_btn hover js-submit-project"> <i class="fa fa-check" aria-hidden="true"></i> Submit</button>
 </div>

  


  </div>
  </div>

<% } %>


<div class="modal" tabindex="-1" role="dialog">
  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title">Modal title</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        <p>Modal body text goes here.</p>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-primary">Save changes</button>
        <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
      </div>
    </div>
  </div>
</div>