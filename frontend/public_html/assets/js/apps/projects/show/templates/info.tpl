

<div class="container">
<hr>


<div class="row">
<div class="col-lg-12">


<table class="table table-bordered" style='margin-bottom: 30px;'> 

<thead class="thead-light">
      <tr>
        <th>Title</th>
        <th>Author</th>
        <th>Language</th>
        <th>Pages</th>
      </tr>
 </thead>
 <tbody>

  <tr>  
        <td><%-title%></td>
        <td><%-author%></td>
        <td><%-language%></td>
        <td><%-pages%></td>

    </tr>

 </tbody>

</table> 

</div>

</div>
</div>
<!--
<div class="row">
<div class="col-lg-12">

<h4> <b>Books:</b>  </h4> 


</div>
</div>
 
<table class="table table-bordered" 
 id="book_table" cellspacing="0" width="100%"  style="margin-top: 20px !important; margin-bottom: 30px !important;" >

 <thead>
      <tr>
        <th>Title</th>
        <th>Author</th>
        <th>Language</th>
        <th>Ocr Engine</th>
        <th></th>
      </tr>
 </thead>
 <tbody>

 	<% _.each(books, function(book) { %>
 	<tr>  
        <td><%-book.title%></td>
        <td><%-book.autdor%></td>
        <td><%-book.language%></td>
        <td><%-book.ocrEngine%></td>
        <td><button class="btn no_bg_btn float-right js-delete-book table_row_button btn-warning"><i class="fa fa-minus" aria-hidden="true"></i> Remove</button></td>

    </tr>
    <% }); %>   

 </tbody>

</table> 
-->
</div>

