
<% _.each(buttongroups, function(buttongroup) { %>

	<ul class=" stack-for-small button-group <%-buttongroup.side%> ">

	<% _.each(buttongroup.buttons, function(button) { %>
    <li> <a class="button small <%- button.type %>  <%- button.selector %>" id="<%- button.id %>"> <i class="<%- button.icon%>"></i> <%=button.name%> </a></li>
	<% }); %>

	</ul>

<% }); %>
