

<div class="container">


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


</div>
</div>

<div class="container">
<div class="row"> 


<div class="col col-sm-12">
<div class="btn-group" role="group" style="float:right;margin-bottom: 15px;">
<button class="btn btn-secondary js-le-redo"> <i class="fas fa-redo"></i> Recalculate </button>
<button class="btn btn-primary blue-bg js-le-profile"><i class="fas fa-play"></i> Profile with extended lexicon </button>
</div>

</div>
</div>
</div>