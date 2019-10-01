
    <div class="card" id="sidebar-container">



		<ul class="nav  navbar-light justify-content-center" style="margin-top: 15px;">
		<li class="nav-item js-firstpage"><a class="nav-link" href="#" title="go to first page">
			<i class="fas fa-angle-double-left"></i>
			</a></li>
		<li class="nav-item js-stepbackward"><a class="nav-link" href="#" title="go to previous page #<%-prevPageId%>">
			<i class="fas fas fa-angle-left"></i>
		</a></li>

		<li>
    	<div class="dropdown js-page-dropdown">
    		<div class="btn-group" style="margin-top: 3px;">

		<button id="pageId" type="button" class="btn btn-sm btn-outline-primary blue"> Page <%-pageId%></button>
		<button id="pageDP"  type="button" class="btn btn-sm btn-primary blue-bg hover dropdown-toggle dropdown-toggle-split" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
    <i class="dp_icon fas fa-sort-down"></i>
  </button>
		<div class="dropdown-menu dropdown-menu-right js-page-dropdown-content" aria-labelledby="pageDP">
   			 <% _.each(project.get('pageIds'), function(pageId) { %>
   			   <a class="js-page-link dropdown-item" pid="<%-project.get('projectId')%>" pageid="<%-pageId%>"><%-pageId%></a>

   			<% }); %>
 		</div>
	    </div>
		</div>
		</li>

	<!--error-patterns -->
	<!-- <li class="nav-item dropdown">
	<a href="#" class="dropdown-toggle nav-link" id="pcw-error-patterns-link" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false" data-flip="false" data-target="#pcw-error-patterns-dropdown">
		Error patterns<span class="caret"></span></a>
        <div id="pcw-error-patterns-dropdown" class="dropdown-menu scrollable-menu" aria-labelledby="pcw-error-patterns-link">
        </div>
    </li> -->
	<!-- error-tokens -->
	<!-- <li class="nav-item dropdown">
	<a href="#" class="dropdown-toggle nav-link" id="pcw-error-tokens-link" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false" data-flip="false" data-target="#pcw-error-tokens-dropdown">
		Error tokens<span class="caret"></span></a>
		 <div class="dropdown-menu scrollable-menu" id="pcw-error-tokens-dropdown" aria-labelledby="pcw-error-tokens-link">
        </div>
     </li> -->



	<!--nextpage and last page -->
		<li class="nav-item js-stepforward"><a class="nav-link"  href="#" title="go to next page #<%-nextPageId%>">
			<i class="fas fa-angle-right"></i>
			</a></li>
		<li class="nav-item js-lastpage"><a class="nav-link" href="#" title="go to last page">
			<i class="fas fa-angle-double-right"></i>
			</a></li>
		</ul>

 <ul class="nav nav-tabs" id="sidebar_tabs" role="tablist" style="margin-top: 10px;">
  <li class="nav-item">
    <a class="nav-link active" id="sp-tab" data-toggle="tab" href="#sp" role="tab" aria-controls="sp" aria-selected="true">Suspicious </br> words</a>
  </li>
  <li class="nav-item">
    <a class="nav-link" id="ep-tab" data-toggle="tab" href="#ep" role="tab" aria-controls="ep" aria-selected="true">Error </br>  patterns</a>
  </li>
  <li class="nav-item">
        <a class="nav-link" id="chars-tab" data-toggle="tab" href="#chars" role="tab" aria-controls="chars" aria-selected="true">Special </br>  characters</a>
  </li>
   <li class="nav-item">
        <a class="nav-link" id="options-tab" data-toggle="tab" href="#options" role="tab" aria-controls="chars" aria-selected="true">Display </br> settings</a>
  </li>
</ul>

<div class="tab-content">

  <div class="tab-pane fade show active" id="sp" role="tabpanel" aria-labelledby="sp-tab">
  		 <div id="suspicious-words-container" class="sidebar-table-container">

  	   	<div class="loading_background2">
	         <div class="loading_text_parent">
	           <div class="loading_text2"> Loading <i class="fas fa-sync fa-spin fa-3x fa-fw"></i> </div>
        	 </div>
        	 </div>
        	   <div id="suspicious-words_filter" class="sidebar-filter-container">
               <input type="search" class="" placeholder="Search...">
          	   </div>

		   <table class="table suspicious-words table-hover table-sm">
		    <thead>
		      <tr>
		        <th>Word</th>
		        <th>Count</th>
		      </tr>
		    </thead>
		    <tbody>

		    </tbody>
		  </table>
		</div>
	  </div>

  <div class="tab-pane fade" id="ep" role="tabpanel" aria-labelledby="ep-tab">

<div id="error-patterns-container" class="sidebar-table-container">
		   	<div class="loading_background2">
	         <div class="loading_text_parent">
	           <div class="loading_text2"> Loading <i class="fas fa-sync fa-spin fa-3x fa-fw"></i> </div>
        	 </div>
        	 </div>
        	 <div id="error-patterns_filter" class="sidebar-filter-container">
               <input type="search" placeholder="Search...">
          	 </div>
		   <table class="table error-patterns table-hover table-sm">
		    <thead>
		      <tr>
		        <th>Word</th>
		        <th>Count</th>
		      </tr>
		    </thead>
		    <tbody>

		    </tbody>
		  </table>
	</div>


 </div>
  <div class="tab-pane fade" id="chars" role="tabpanel" aria-labelledby="chars-tab">

	<div id="special-characters-container" class="sidebar-table-container">
			   	<div class="loading_background2">
		         <div class="loading_text_parent">
		           <div class="loading_text2"> Loading <i class="fas fa-sync fa-spin fa-3x fa-fw"></i> </div>
	        	 </div>
	        	 </div>
	        	 <div id="special-characters_filter" class="sidebar-filter-container">
	               <input type="search" placeholder="Search...">
	          	 </div>
			   <table class="table special-characters table-hover table-sm">
			    <thead>
			      <tr>
			        <th>Character</th>
			        <th>Count</th>
			      </tr>
			    </thead>
			    <tbody>

			    </tbody>
			  </table>
		</div>

  </div>


<div class="tab-pane fade" id="options" role="tabpanel" aria-labelledby="options-tab">

  <div class="form-group options_row"
  	   title="Adjust the height of the line images">
  <label for="line_size_slider"> Line height: <span id="lineheight_value"><%-lineheight%>px</span></label>
   <input  type="range" min="20" max="60" value="30" class="slider form-control" id="line_size_slider">
  </div>
  <hr>
  <div class="form-group options_row" title="Display line numbers">
  <span><input type="checkbox" id="line_nr_toggle" name="line_nr_toggle" checked></span>
  <span for="line_nr_toggle"> Line numbers </span>
  </div>
    <hr>
 <div class="form-group options_row"
 	  title="Hide correction text lines">
  <span><input type="checkbox" id="cor_toggle" name="cor_toggle"></span>
  <span for="cor_toggle"> Hide corrections </span>
  </div>
  <hr>
  <div class="form-group options_row"
  	   title="Set the maximum number of search results in concordance view">
  <label for="page_hits_slider"> Search hits per page: <span id="page_hits_value"><%-pagehits%></span></label>
   <input  type="range" min="1" max="100" value="8" class="slider form-control" id="page_hits_slider">
  </div>
  <hr>
  <div class="form-group options_row"
  	   title="Ignore case for search results">
	   <span><input type="checkbox" id="ignore_case_toggle" name="ignore_case_toggle"></span>
	   <span for="ignore_case_toggle"> Ignore case </span>
  </div>
  <hr>
</div>


</div>

</div>