

<div class="container">


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
       <% for (key in pr.always) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.always[key]%></td>

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
   <% for (key in pr.sometimes) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.sometimes[key]%></td>

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
   <% for (key in pr.never) { %>
       <tr>  
        <td><%-key%></td>
        <td><%-pr.never[key]%></td>

        </tr>
       <% } %> 

 </tbody>

</table> 


</div>



</div>
</div>

<div class="container">
<div class="row"> 


<div class="col col-sm-12">
<div class="btn-group" role="group" style="float:right;margin-bottom: 15px;">
<button class="btn btn-secondary js-pr-redo"> <i class="fas fa-redo"></i> Recalculate </button>
</div>

</div>
</div>
</div>