

<div class="container">
<hr>


<div class="row">
<div class="col-sm-4">
<h4>Always</h4> 
<table id="always" class="table table-bordered sortable table-hover" style='margin-bottom: 30px;'> 
<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
       <% for (key in pr.yes) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.yes[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>


<div class="col-sm-4">

<table id="sometimes" class="table table-bordered sortable table-hover" style='margin-bottom: 30px;'>
<h4>Sometimes</h4> 
<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
   <% for (key in pr.no) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.no[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>


<div class="col-sm-4">

<table id="never" class="table table-bordered sortable table-hover" style='margin-bottom: 30px;'>
<h4>Never</h4> 
<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
   <% for (key in pr.no) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.no[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>



</div>
</div>

