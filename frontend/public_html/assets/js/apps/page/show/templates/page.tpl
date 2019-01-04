	<div class="container">
	<div class="row">
    <div class="col col-md-12">
	<ul class="nav sticky-top navbar-light bg-light">
	<li class="nav-item js-firstpage"><a class="nav-link" href="#" title="go to first page">
		<i class="fas fa-fast-backward"></i>
		</a></li>
	<li class="nav-item js-stepback"><a class="nav-link" href="page.php?u=none&p=', $prevpageid, '&pid=', $pid,'" title="got to previous page #', $prevpageid, '">
		<i class="fas fa-step-backward"></i>
		</a></li>
	<li class="nav-item"> <a class="nav-link" href="#">
		<label id="concordance-search-label">Show concordance of (0 occurences)</label>
		</a></li>
	<!-- suggestions -->
	<li class="nav-item dropdown"> 
	<a href="#" class="dropdown-toggle nav-link" data-toggle="dropdown" role="button">
		Correction suggestions<span class="caret"></span></a>
        <ul id="pcw-suggestions-dropdown" class="dropdown-menu">
        </ul>
        </li>
	<!--error-patterns -->
	<li class="nav-item dropdown">
	<a href="#" class="dropdown-toggle nav-link" data-toggle="dropdown" role="button">
		Error patterns<span class="caret"></span></a>
        <ul id="pcw-error-patterns-dropdown" class="dropdown-menu scrollable-menu">
        </ul>
        </li>
	<!-- error-tokens -->
	<li class="nav-item dropdown"> 
	<a href="#" class="dropdown-toggle nav-link" data-toggle="dropdown" role="button">
		Error tokens<span class="caret"></span></a>
        <ul id="pcw-error-tokens-dropdown" class="dropdown-menu scrollable-menu">
        </ul>
        </li>
	<!--nextpage and last page -->
	<li class="nav-item js-stepforward"><a class="nav-link" href="page.php?u=none&p=', $nextpageid, '&pid=', $pid,'" title="got to next page #', $nextpageid, '">
		<i class="fas fa-step-forward"></i>
		</a></li>
	<li class="nav-item js-lastpage"><a class="nav-link" href="page.php?u=none&p=last&pid=', $pid,'" title="got to last page">
		<i class="fas fa-fast-forward"></i>
		</a></li>
	</ul>

	<div id="page-heading">
	<p><h2>Project <%-projectId%>, page <%-pageId%></h2></p>
	</div>


	   <%
     _.each(lines, function(line) { 
  	   var text = "line," + line['lineId'] + " " + line["imgFile"];
  	   var anchor = line["projectId"]+"-"+line["pageId"]+"-"+line['lineId'];
  	   var inputclass = Util.get_correction_class(line);
      %>
       <div class="text-image-line" title="<%-text%>">

       	<a class="line-anchor" id="line-anchor-<%-anchor%>" anchor="<%-anchor%>"></a>
		<img src='<%-line["imgFile"]%>' alt='<%-text%>' title='<%-text%>' width="auto" height="25">
		<div id="line-text-<%-anchor%>" class="line-text <%-inputclass%>">
        <%-line["cor"]%>
        </div>

       </div>
	
     


      <% }) %>
	</div>
    </div>
 	</div>
