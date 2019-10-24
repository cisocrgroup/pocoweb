

<div class="container">


<div class="row">
<div class="col col-lg-6">
<div style="text-align: center;">
<h4>Extensions</h4>
</div>

<table id="extensions" class="table table-hover table-bordered table-sm" style='margin-bottom: 30px;'> 
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
        <td><%-le.yes[key]%><div class="table_chevron unknown" style="display:inline-block; float: right; margin-right: 2px;" title="Move to unknown"> <span style="float: right;"><i class="fas fa-chevron-right"></i></span></div></td>
        </tr>
       <% } %> 

 </tbody>

</table> 


</div>

<!-- <div class="col-lg-2">
<div style="text-align: center;">
<i style="margin-top: 118px;font-size: 50px; color: #1857e6;" class="fas fa-exchange-alt"></i>
</div>
</div> -->

<div class="col col-lg-6">

<table id="unknown" class="table table-hover table-bordered table-sm" style='margin-bottom: 30px;'>
<div style="text-align: center;">
<h4>Unknown</h4> 
</div>

<thead class="thead-light">
      <tr>
        <th>Word</th>
        <th>Frequency</th>
      </tr>
 </thead>
 <tbody>
   <% for (key in le.no) { %>
       <tr>  
        <td><div class="table_chevron" style="display:inline-block;" title="Move to extensions"> <span ><i class="fas fa-chevron-left"></i></span></div><%-key%></td>
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
<div class="btn-group" role="group" style="float:right;margin-bottom: 15px;margin-top: 15px;">
<button class="btn btn-secondary js-le-redo"> <i class="fas fa-redo"></i> Recalculate </button>
<button class="btn btn-primary blue-bg js-le-profile"><i class="fas fa-play"></i> Profile with extended lexicon </button>
</div>

</div>
</div>
</div>