<div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title">Adaptive tokens for project <%-pid%></h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        
        <table class="table adaptive-tokens table-sm">
        <thead>
          <tr>
            <th>Token</th>
          </tr>
        </thead>
        <tbody>
            <% adaptiveTokens.forEach(function(token) { %>
           <tr class="adaptive-token"> <td><%-token%></td> </tr>
           <% }); %>
        </tbody>
      </table>

      </div>
      <div class="modal-footer">
        <button type="button" data-dismiss="modal" class="btn btn-primary blue-bg js-ok">OK</button>
      </div>
    </div>
  </div>
