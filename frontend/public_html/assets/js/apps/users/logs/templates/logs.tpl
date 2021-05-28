 
 <%
if(asModal) {
%>

  <div class="modal-dialog modal-xl" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h3 class="modal-title">Logs</h3>
       
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } %>



 <div class="container">

<div class="row">

 <div class="col col-md-12">

 <%
if(!asModal) {
%>

 	<hr>
    <h2 style="margin-bottom: 15px;"> Logs: </h2>
  <% } %>
   <table class="table table-bordered table-striped hover" 
 id="<%-table_id%>" cellspacing="0" width="100%"  style="margin-top: 20px !important; margin-bottom: 30px !important;" >


     <thead>
      <tr>
      <% _.each(headers, function(header) { %>  
        <th><%-header.name%></th>
       <% }); %>         
      </tr>
     </thead>
     <tbody>


     <%
     var count = 0; 
     _.each(items, function(item) {
     
      %>

      <% if(columns[0]['clickrow']) { %>

       <tr class='clickable-row' data-href="#<%-urlroot%>/<%-item[columns[0]['id']]%>">
       
       <% } else { %>
       <tr>
       <% } %>

        <% for(var i=0;i<columns.length;i++){ %>  

          <% column = columns[i]; %>
          <% if (column.name == "msg") {  %>
          <td>

          	<div style="margin-bottom: 0px;" class="alert alert-<%-item['type']%> show" role="alert"><%-item[column.name]%>
			 </div>
            
          </td>      
          <% } else if(column.name == "date") { var date = new Date(item[column.name]).toDateString(); %>
          <td> <%- date %> </td>
          <td> <%- item[column.name] %> </td>          
          <% } else { %>
          <td> <%- item[column.name] %> </td>      
          <% } %>
        <% } %>  
       
     
      </tr>

    <% count++;  }); %>

     </tbody>
    </table>              
  
</div>

</div>

</div>

<%
if(asModal) {
%>

<div class="modal-footer">
 </div>


  </div> 
  </div>
  </div>
<% } %>
