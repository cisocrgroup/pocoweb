<div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title">Adaptive tokens for project <%-pid%></h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
           <div class="adaptive-tokens-list">
           <% adaptiveTokens.forEach(function(token) { %>
           <div class="adaptive-token"> <%-token%> </div>
           <% }); %>
           </div>
      </div>
      <div class="modal-footer">
        <button type="button" data-dismiss="modal" class="btn btn-primary js-ok">OK</button>
      </div>
    </div>
  </div>
