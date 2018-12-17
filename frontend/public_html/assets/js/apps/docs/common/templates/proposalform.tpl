
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

<% } %>

<form>

<div class="form-group row">
  <div class="col-12">
    <label for="title">Title</label>
    <input class="form-control" type="text" value="<%-title%>" id="title" name="title">
  </div>
</div>

<div class="form-group row">
  <div class="col-4">
    <label for="proposalId">Proposal Identifier</label>
    <input class="form-control" type="text" value="<%-proposalId%>" id="proposalId" name="proposalId">
  </div>

  <div class="col-2">
  <label for="proposalType">Type</label>
    <select class="form-control" id="proposalType" value="<%-proposalType%>">
      <option >spc</a>
      <option >spco</a>
      <option >ac</a>
    </select>
  </div>

  <div class="col-6">
  <label for="topic_id">Topic</label>
    <select class="form-control" id="topic_id" value="<%-topic_id%>">
     <% _.each(topics, function(topic) { %>  
      <option value="<%-topic.topicacronym%>"><%-topic.topicdescription%></option>
      <% }); %>
    </select>
  </div>

</div>

<div class="form-group row">
  <div class="col-6">
    <label for="observation_start">Observation Start</label>
    <input class="form-control" type="text" value="<%-observationDateRange%>" id="observation_start" name="observation_start">
  </div>
  <div class="col-6">
    <label for="observation_end">Observation End</label>
    <input class="form-control" type="text" value="<%-observationDateRange%>" id="observation_end" name="observation_end">
  </div>
</div>

<div class="form-group row">
  <div class="col-3">
    <label for="entryDate">Entry</label>
<% if(entryDate!=""){%>
    <input class="form-control" type="text" value="<%-entryDate%>" id="entryDate" name="entryDate">
<% } else { %>
    <input class="form-control" type="text" value="<%-entryDate%>" id="entryDate" name="entryDate">
<%}%>
  </div>
  <div class="col-3">
    <label for="expirationDate">Expiration</label>
    <input class="form-control" type="text" value="<%-expirationDate%>" id="expirationDate" name="expirationDate">
  </div>

  <div class="col-6">
<% if(entryDate!=""){%>
  <label> class="custom-control custom-checkbox" style="margin-top: 1.8em;">
    <input type="checkbox" class="custom-control-input">
    <span class="custom-control-indicator"></span>
    <span class="custom-control-description">Source Proposal: (!) <%-copiedFlag%></span>
  </label>
<%}%>
  
  <label class="custom-control custom-checkbox" style="margin-top: 1.8em;">
    <input type="checkbox" class="custom-control-input">
    <span class="custom-control-indicator"></span>
    <span class="custom-control-description">Publication Agreed <%-publicationAgreed%></span>
  </label>
  </div>
</div>

<div class="form-group row">
  <div class="col-12">
    <label for="objectives">Objectives</label>
    <input class="form-control" type="text" value="<%-objectives%>" id="objectives" name="objectives">
  </div>
</div>

<div class="form-group row">
  <div class="col-12">
    <label for="methodology">Methodology</label>
    <input class="form-control" type="text" value="<%-methodology%>" id="methodology" name="methodology">
  </div>
</div>

<div class="form-group row">
  <div class="col-12">
    <label for="expectedResults">Expected Results</label>
    <input class="form-control" type="text" value="<%-expectedResults%>" id="expectedResults" name="expectedResults">
  </div>
</div>

<div class="form-group row">
  <div class="col-12">
    <label for="projectReferences">Project References</label>
    <input class="form-control" type="text" value="<%-projectReferences%>" id="projectReferences" name="projectReferences">
  </div>
</div>

<div class="form-group row">
  <div class="col-2">
    <label for="quota">Quota</label>
    <input class="form-control" type="text" value="<%-quota%>" id="quota" name="quota">
  </div>
  <div class="col-2">
    <label for="contingent">Contingent</label>
    <input class="form-control" type="text" value="<%-contingent%>" id="contingent" name="contingent">
  </div>
   <div class="col-2">
    <label for="priority">Priority</label>
    <input class="form-control" type="text" value="<%-priority%>" id="priority" name="priority">
  </div>
  <div class="col-2">
    <label for="defaultPriority">Default</label>
    <input class="form-control" type="text" value="<%-defaultPriority%>" id="defaultPriority" name="defaultPriority">
  </div>
  <div class="col-4">
    <label for="numDtakesRequestedTotal">Requested Datatakes</label>
    <input class="form-control" type="text" value="<%-numDtakesRequestedTotal%>" id="numDtakesRequestedTotal" name="numDtakesRequestedTotal">
  </div>
</div>

</form>



<%
if(asModal) {
%>

  </div> 

  <% if(proposalId==null){%>

 <div class="modal-footer">
      <button class="btn no_bg_btn red-border hover js-submit-proposal" style="margin-bottom:15px"> <i class="fa fa-check" aria-hidden="true"></i> Submit</button>
 </div>

  
<%}%>


  </div>
  </div>

<% } %>
