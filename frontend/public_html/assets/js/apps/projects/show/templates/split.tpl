  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title">Split project <%title%></h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">

        <form class="splitform">
          <div class="form-group row">
          <div class="col-12">
             <button type="button" class="btn btn-primary js-addpackage"><i class="fas fa-folder-plus"></i> Add new package</button>
          </div>
        </div> 
        <div class="userrows">
        </div>

       <!--  <div class="form-group row">
          <div class="col-4">
          <label id="splitLabel" for="splitPage">Split into <%-n%> pages</label>
          <input id="split-n" name="split-n" size="3" type="number" min="1" max="100" step="1" value="<%-n%>" class="form-control">
          </div>
        </div> -->
        <div class="form-group form-check">
          <input type="checkbox" class="form-check-input" id="checkRnd">
          <label class="form-check-label" for="checkRnd">Random</label>
        </div>
      </form>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-primary js-confirm">Confirm</button>
        <button type="button" class="btn btn-secondary" data-dismiss="modal">Cancel</button>
      </div>
    </div>
  </div>
