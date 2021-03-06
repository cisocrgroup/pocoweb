
<%
if(asModal) {
%>

  <div class="modal-dialog modal-xxl" role="document">
  <div class="modal-content">

<div class="modal-header">
        <h3 class="modal-title">Concordance for "<%-selection%>"</h3>

        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>

      </div>
<div class="modal-body">

<% } else { %>


	<div class="container">
	<div class="row">
    <div class="col col-md-12">

	<div id="concordance-heading">
	<p><h2>Concordance view for "<%-selection%>"</h2></p>
	</div>


<% } %>

 <!--  <nav class="navbar navbar-static-top" id="page-header" data-spy="affix" data-offset-top="197">
  <div class="container-fluid">
  <div class="collapse navbar-collapse">
  <ul class="nav navbar-nav">
  <li>
  <form class="navbar-form">-->

  <% if (!le){ %>

  <div class="input-group mb-3">
  <div class="input-group-prepend">
  <button class="js-toggle-selection btn btn-outline-secondary" title="Toggle selection">
  <i class="fas fa-toggle-on"></i> Toggle selection
  </button>
  </div>
  <input class="js-global-correction-suggestion form-control" title="correction" type="text" placeholder="Correction"/>
  <div class="input-group-append">
  <button class="js-set-correction btn btn-outline-secondary" title="Set correction">
  <i class="fas fa-level-down-alt"></i> Set correction
  </button>
  <button class="js-correct-conc selected btn btn-outline-secondary" title="Correct selected">
  <i class="far fa-arrow-alt-circle-right"></i> Correct selected
  </button>
  </div>
  </div>

  <% } %>

 <!-- </form>
   </li>
  </ul>
  </div>
  </div>
  </nav> -->

  <div class="all_lines_parent">
	  <%
	  let nmatches=0;
   for (key in tokendata['matches']) {
           for (var i=0;i<tokendata['matches'][key].lines.length;i++){
            var line = tokendata['matches'][key].lines[i];
			nmatches++;
        	var link = "#/projects/"+line['projectId']+"/page/"+line['pageId'];
   %>

<div class="line-container">
<div class="line-nr line-nr-conc" title="page <%-line.pageId%>, line <%-line.lineId%>" > <span><%-line.pageId%>:<%-line['lineId']%></span></div>

<div class="text-image-line" title="page <%-line.pageId%>, line <%-line.lineId%>">

<div class="left_div div_inline">
		<div class="invisible=link" href="<%-link%>">
    <div id ="img_<%-line['pageId']%>_<%-line['lineId']%>_parent" class="line-img">
		<img src="<%-line['imgFile']%>" id="img_<%-line['pageId']%>_<%-line['lineId']%>" width="auto" height="<%-lineheight%>"/>
    </div>

		</div>
	</div>

	</div>
 <div class="page_jump" pid="<%-line['projectId']%>" pageid="<%-line['pageId']%>" lineid="<%-line['lineId']%>" title="Jump to page <%-line['pageId']%>, line <%-line['lineId']%>"> <span><i class="fas fa-chevron-right"></i></span></div>
 </div>

     <%
     	  
        }
     	};
     %>


</div>

<%
if(asModal) {
%>

  <div class="modal-footer">

    <div class="col col-md-6">

       <div class="concordance-number" style="margin-bottom: 12px;">
        Showing <%-tokendata.skip+1%> to <%-nmatches%> of <%-tokendata.total%> search results
     </div>

   </div>
    <div class="col col-md-6">

      <nav aria-label="Page navigation" class="float-right">
        <ul class="pagination justify-content-center js-paginate">
          <li value ="-2" class="page-item">
            <a class="page-link" href="">Previous</a>
          </li>
          <li value ="1" class="page-item active"><a class="page-link" href="">1</a></li>

          <% var max_pages = Math.ceil(tokendata.total / tokendata.max);
          if(max_pages > 5) {
          for(var i=2;i<=5;i++) { %>
          <li value="<%-i%>" class="page-item"><a class="page-link" href="" ><%-i%></a></li>
          <% } %>
          <li value="-1" class="page-item">
		  	  <a class="page-link">...</a>
		  </li>
          <li value="<%-max_pages%>"class="page-item">
		  	  <a class="page-link" href=""><%-max_pages%></a>
		  </li>
          <%
			} else {
             for(var i=2;i<=max_pages;i++) {
		  %>
          <li value="<%-i%>" class="page-item">
		  	  <a class="page-link" href="" ><%-i%></a>
		  </li>
          <% } %>
          <% } %>
          <li value="-3" class="page-item">
            <a  class="page-link" href="">Next</a>
          </li>
        </ul>
      </nav>
     </div>
  </div>

<% } %>


	</div>
    </div>
 	</div>
