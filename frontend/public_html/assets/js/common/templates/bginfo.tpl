

<a class="close-reveal-modal">×</a> 


<h3> Image Reference: </h3>
</br>


<table id="table" class="table"  width="100%">

<tbody>

<tr><td><b>Filename:</b> <%- original_name%></td></tr>
<%if(collection!=""){%>  <tr> <td> <div><b>Collection:</b> <%- collection%></td></tr> <%}%> 
<%if(author!=""){%>  <tr> <td> <b>Author:</b> <%-author%></td></tr> <%}%> 
<%if(source!=""){%> <tr> <td>
	 <% if(source.indexOf("http")>-1) { %>
	 <a href="<%-source%>" class="info_link"><b>Source:</b> <%-source%></a>
	 <% } else { %>
	 <b>Source:</b> <%-source%>
	 <%}%>
 <% } %>

<%if(link!=""){%> <tr> <td> <a href="<%-link%>" class="info_link"><b>Link:</b> <%-link%></a></td> </tr><%}%> 


</tbody>

</table>

 <div class="line_parent">  
        <div class="left_line_parent"><hr class="fancyline fancyline_left"/></div>  
        <div class="center_line_parent">~</div>  
        <div class="right_line_parent"><hr class="fancyline fancyline_right"/></div>
 </div>

