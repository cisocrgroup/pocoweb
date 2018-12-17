


<div class="container">

<%
 var rows = Math.ceil(cards.length/columns);
 var col_count = 0 

 %>

<% for(var i = 0; i < rows; i++) { %>
 <div class="row card_row">
          <% for(var j = 0; j < columns; j++) {
          if(col_count+1>cards.length){break;}
          %>
          <% var item = cards[col_count];%>
                <div class="col-md-<%-12/columns%> hub_col">
              
                 <div class="card custom_card active" url="<%-item['url']%>" id="<%-item['id']%>">
                 <!-- <img class="card-img-top" src="assets/images/enmap_header_1.png." alt="Card image cap"> -->
                   <div class="card-header">
                   <h3 class="card-title"><i class="fa <%-item['icon']%> card_main_icon <%-item['color']%>" aria-hidden="true"></i> <%-item['name']%></h3>
                      </div>
                  <div class="card-block">


                    <p class="card-text"> <%-item['text']%>  </p>
                    <!-- <a href="#" class="btn btn-primary">Go somewhere</a> -->

                  </div>


                  <div class="card-footer text-muted">

                   <div class="row">

                     <div class="footer_item_container">

                          <div style="visibility: hidden" class="dummy footer_item "><i class="fa fa-user-o" aria-hidden="true"></i> <span class="rolename">X</span> </div>

                     </div>

                     <div class="footer_item_enter_container">  
                        <div class="footer_item footer_item_enter hover <%if(item['available']!=undefined&&item['available']==true){%><%-item.color%>-border<%}%>">  <i class="fa fa-sign-in" aria-hidden="true"></i> Enter </div>
                     </div>

                   </div>
                     
                  </div>


                </div>

                </div>

              <% col_count++;  } %>
    </div> 
  <% } %>
  </div>

<% if (back_btn !=undefined) { %>

<div class="container">
   <div class="row">
  <div class="col col-md-12">
  <button class="btn back_btn js-back <%-cards[0]['color']%>-border hover" style="margin-left:0px;"> <i class="fa fa-caret-left" aria-hidden="true"></i> Back</button>


  </div>
  </div>

</div>


<% } %>
