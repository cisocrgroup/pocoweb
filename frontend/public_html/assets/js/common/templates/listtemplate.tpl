 <div class="container" style="padding-bottom:50px;">
 <div class="row">
 <div class="col col-lg-12" >


 <table class="table table-bordered <% if(hover){%>table-hover<%}%>" 
 id="table" cellspacing="0" width="100%"  style="margin-top: 20px !important; margin-bottom: 30px !important;" >


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
     console.log(items);
     _.each(items, function(item) {
     
      %>

      <% if(columns[0]['clickrow']) { %>

       <tr class='clickable-row' data-href="#<%-urlroot%>/<%-item[columns[0]['id']]%>">
       
       <% } else { %>
       <tr>
       <% } %>

        <% for(var i=0;i<columns.length;i++){ %>  

          <% column = columns[i]; %>
          <% if (column.name == "action") {  %>
          <td><button type="button" class="close btn js-delete-user" id="<%-item[columns[0]['id']]%>"> <span aria-hidden="true"><i class="far fa-times-circle"></i></span></button></td>      
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