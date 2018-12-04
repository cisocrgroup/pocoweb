


<div class="container">
<div class="row">

   <div class="col-md-5">


    <div style="margin-bottom:15px">

    <button type="button" class="btn js-edit-project"> <i class="fa fa-pencil-square-o" aria-hidden="true"></i> Edit</button>
    <button type="button" class="btn js-delete-project btn-danger"><i class="fa fa-trash-o" aria-hidden="true"></i> Delete</button>
    <button type="button" class="btn js-add-book"> <i class="fa fa-plus" aria-hidden="true"></i> Add Book</button>

    </div>


    <h4> <b>Project Information:</b>  </h4> 


  </div>
   <div class="col-md-7">
   </div>

</div>

<div class="row">
<div class="col-lg-12">

<div style="margin-bottom:15px">

	<div>
	<b>Title : </b> <%-title%>
	</div>

	<div>
	<b>Author : </b> <%-author%>
	</div>

	<div>
	<b>Year : </b> <%-year%>
	</div>

	<div>
	<b>Language : </b> <%-language%>
	</div>


	<div>
	<b>User : </b> <%-user%>
	</div>

<hr>

</div>

</div>
</div>

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

</div>

