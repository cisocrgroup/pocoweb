
<% if(packages.length>0){ %>

<div class="container">

<hr>

<div class="row">
<div class="col-lg-12">


<h4> <b>Packages:</b>  </h4> 


 
<table class="table table-bordered" 
 id="book_table" cellspacing="0" width="100%"  style="margin-top: 20px !important; margin-bottom: 30px !important;" >

 <thead class="thead-light">
      <tr>
        <th>Title</th>
        <th>Author</th>
        <th>Language</th>
        <th>Pages</th>
      </tr>
 </thead>
 <tbody>

 	<% _.each(packages, function(package) { %>
 	<tr class="clickable-row" data-href="#projects/<%-package.projectId%>">  
        <td><%-package.title%></td>
        <td><%-package.author%></td>
        <td><%-package.year%></td>
        <td><%-package.pages%></td>
    </tr>
    <% }); %>   

 </tbody>

</table> 

</div>
</div>
</div>

<% } %>