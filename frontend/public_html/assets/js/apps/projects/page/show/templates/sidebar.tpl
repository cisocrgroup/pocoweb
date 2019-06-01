
    <div class="card">

	

		<ul class="nav  navbar-light justify-content-center">
		<li class="nav-item js-firstpage"><a class="nav-link" href="#" title="go to first page">
			<i class="fas fa-angle-double-left"></i>
			</a></li>
		<li class="nav-item js-stepbackward"><a class="nav-link" href="#" title="go to previous page #<%-prevPageId%>">
			<i class="fas fas fa-angle-left"></i>
		</a></li>

		<li>
		<div id="pageId" style="margin-top: 7px;"> Page <%-pageId%></div>
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
		  <hr style="margin: 0;">
		  <div class="card-header">
		    Suspicious words
		  </div>
		   <div class="suspicious-words-container">
		   	<div class="loading_background2">
	         <div class="loading_text_parent">
	           <div class="loading_text2"> Loading <i class="fas fa-sync fa-spin fa-3x fa-fw"></i> </div>
        	 </div>
        	 </div>
		   <table class="table suspicious-words">
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
		 <hr style="margin: 0;">
		  <div class="card-header ">
		    Error patterns
		  </div>
		   <div class="error-patterns-container">
		   	<div class="loading_background2">
	         <div class="loading_text_parent">
	           <div class="loading_text2"> Loading <i class="fas fa-sync fa-spin fa-3x fa-fw"></i> </div>
        	 </div>
        	 </div>
		   <table class="table error-patterns">
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