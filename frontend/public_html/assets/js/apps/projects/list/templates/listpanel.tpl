<div class="container">
<div class="row">
<div class="col-lg-12">

<%if (user.admin) {%>
	 <button type="submit" class="btn btn-outline-primary blue hover js-create" style="margin-bottom:15px">
		<i class="fa fa-plus" aria-hidden="true"></i> Create new project</button>
	<div class="btn-group float-right">
		<a  class="btn btn-outline-primary blue hover"href="https://pocoweb.cis.lmu.de/rest/pool/global?auth=<%-auth%>" download="global-pool.zip"> 
		<i class="fas fa-cloud-download-alt"></i> Download project pool</a>
		<a class="btn btn-outline-primary blue hover"  href="https://pocoweb.cis.lmu.de/rest/pool/global?auth=<%-auth%>" download="user-pool.zip">
		<i class="fas fa-file-archive"></i> Download <%-user.name%>'s projects</a>
		
	</div>
<% } %>

</div>
</div>
</div>