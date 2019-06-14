

<div class="container">
<hr>


<div class="row">
<div class="col-lg-5">
<h4>Extensions</h4> 
<table id="extensions" class="table table-bordered sortable table-hover" style='margin-bottom: 30px;'> 
<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
       <% for (key in le.yes) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-le.yes[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>

<div class="col-lg-2">
<div style="text-align: center;">
<i style="margin-top: 118px;font-size: 50px;" class="fas fa-exchange-alt"></i>
</div>
</div>

<div class="col-lg-5">

<table id="unknown" class="table table-bordered sortable table-hover" style='margin-bottom: 30px;'>
<h4>Unknown</h4> 
<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
   <% for (key in le.no) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-le.no[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>

<div class="container">
<div class="row"> 


<div class="col col-sm-12">

<button class="btn btn-primary"><i class="fas fa-play"></i> Start Postcorrection </button>

</div>
</div>
</div>



</div>
</div>

