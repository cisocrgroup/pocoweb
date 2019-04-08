
<div class="container" style="margin-top: 40px">

<div class="row">

<div class="col-12-md">

<div class="loading_background">
         <i class="fa fa-spinner fa-spin fa-3x fa-fw"></i>
</div>


<% if(results.length>0){ %>

<ul id="tabs" class="nav nav-tabs" role="tablist">


<%
var count=0;
 _.each(results, function(result) { count++; %>

<li class="nav-item">
    <% if(count==1) { %>
    <a class="nav-link active" data-toggle="tab" href="#<%-result.type%>" role="tab"><%-result.name%></a>
    <% } else { %>
    <a class="nav-link" data-toggle="tab" href="#<%-result.type%>" role="tab"><%-result.name%></a>
    <% } %>

  </li>


<% }); %>

</ul>

<div class="tab-content">
 
<%
var count=0;
 _.each(results, function(result) { 
 count++; 
 %>
<% if(count==1) { 
 %>
 <div class="tab-pane active" id="<%-result.type%>" role="tabpanel">
 <%=  result.evalstring  %>
 </div>
<% } else { %>
 <div class="tab-pane" id="<%-result.type%>" role="tabpanel">
<%= result.evalstring %>
 </div>
<% } %>


<% }); %>

</div>

<% } %>



</div>

</div>

</div>