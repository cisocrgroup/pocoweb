

<div class="container-fluid">


<div class="row">
<div class="col col-lg-4">
<div style="text-align: center;">
<h4>Always</h4> 
</div>
<table id="always" class="table table-hover table-bordered table-sm" style='margin-bottom: 30px;'> 
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


<div class="col col-lg-4">

<table id="sometimes" class="table table-hover table-bordered table-sm" style='margin-bottom: 30px;'>
<div style="text-align: center;">
<h4>Sometimes</h4> 
</div>
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


<div class="col col-lg-4">

<table id="never" class="table table-hover table-bordered table-sm" style='margin-bottom: 30px;'>
<div style="text-align: center;">
<h4>Never</h4> 
</div>
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

<div class="container-fluid">
<div class="row"> 


<div class="col col-sm-12">
<div class="btn-group" role="group" style="float:right;margin-bottom: 15px; margin-top: 15px;">
<button class="btn btn-secondary js-pr-redo"> <i class="fas fa-redo"></i> Recalculate </button>
</div>

</div>
</div>
</div>