

<div class="container">


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
        <td>
          <% for (var i =0;i<pr.always[key].length;i++){%>
           <div>
          <%-pr.always[key][i].ocr%> <i class="fas fa-long-arrow-alt-right"></i> <%-pr.always[key][i].cor%> <i class="green far fa-check-circle"></i>
          </div>
          <% } %>
        </td>

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
        <td><% for (var i =0;i<pr.sometimes[key].length;i++){%>
            <div>
              <%-pr.sometimes[key][i].ocr%> <i class="fas fa-long-arrow-alt-right"></i> <%-pr.sometimes[key][i].cor%>
              <%if(pr.sometimes[key][i].taken) { %> <i class="green far fa-check-circle"></i> 
              <% } else { %>
              <i class="red far fa-times-circle"></i> 
              <% } %>
          </div>
          <% } %>
        </td>

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
        <td>
          <% for (var i =0;i<pr.never[key].length;i++){%>
           <div>
          <%-pr.never[key][i].ocr%> <i class="fas fa-long-arrow-alt-right"></i> <%-pr.never[key][i].cor%> <i class="red far fa-times-circle"></i> 
          </div>
          <% } %>
        </td>

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
<div class="btn-group" role="group" style="float:right;margin-bottom: 15px; margin-top: 15px;">
<button class="btn btn-secondary js-pr-redo"> <i class="fas fa-redo"></i> Recalculate </button>
</div>

</div>
</div>
</div>