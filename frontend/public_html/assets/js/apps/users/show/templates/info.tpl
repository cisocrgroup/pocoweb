 <div class="large-12 columns bg_layer">

  <h3> User Information: </h3>
  <fieldset> 
  <div> Firstname: <%- firstname %> <div>  
  <div> Lastname: <%- lastname %> <div>  
  <div> Role: <%- role %> <div>  
  <div> Email: <%- email %> <div>
<!-- 7  <div> Last Updated: <%- changed %> <div> -->
  <div> Verified: <%- verified %> </div>
  </fieldset>

  <% if(logs.length>0){%><h3> Change Log: </h3> 
   <table id="log_table">
     <thead>
      <tr>
        <th>Table</th>
        <th>Action</th>
        <th>Entity ID</th>
        <th>Date</th>
      </tr>
     </thead>
     <tbody>
    <% _.each(logs, function(log) { %>  
      <tr>
      <td> <div class="resp_header" style="margin-right:5px">  Table: </div>  <%- log.tablename %> </a> </td>
      <td> <div class="resp_header" style="margin-right:5px"> Action: </div>  <%- log.action %> </a> </td>
      <td> <div class="resp_header" style="margin-right:5px"> Entity ID: </div> <%- log.entity_id %> </a> </td>
      <td> <div class="resp_header" style="margin-right:5px"> Date: </div> <%- log.time_tag %></a>  </td>
      </tr>
    <% }); %>

     </tbody>
    </table>

<%}%>

  <button type="submit" class="medium button right js-back"> <i class="fa fa-chevron-left"></i> Back</button>

  </div>
