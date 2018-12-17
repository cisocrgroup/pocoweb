
<%
if(asModal) {
%>

  <div class="modal-dialog" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h3 class="modal-title"><%-title%> </h3>
       
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } %>

   <div class="small_headline_for_input"><small> <em><%=text%></em> </small></div> 


</div> 

<%
if(asModal) {
%>


<div class="modal-footer">
   <div class="role_item_container">
    <button class="btn no_bg_btn  green-border hover js-confirm"> <i class="fa fa-check" aria-hidden="true"></i> Confirm</button>
    <button class="btn no_bg_btn  red-border hover js-cancel"> <i class="fa fa-times" aria-hidden="true"></i> Cancel</button>

   </div>
 </div>
<% } %>



<%
if(asModal) {
%>

  </div> 

  </div>
  </div>

<% } %>
