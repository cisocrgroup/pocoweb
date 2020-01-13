

<div class="container">


<div class="row">
<div class="col-lg-12">


<table class="table table-bordered" style='margin-bottom: 30px;'>

<thead class="thead-light">
      <tr>
        <th>Title</th>
        <th>Author</th>
        <th>Language</th>
        <th>Pages</th>
		<th>Status</th>
      </tr>
 </thead>
 <tbody>

  <tr>
        <td><%-title%></td>
        <td><%-author%></td>
        <td><%-language%></td>
        <td><%-pages%></td>
		<td>
        <i class="profile_icon fas fas fa-history card_main_icon disabled" title="not yet profiled"></i>
        <i class="le_icon fas fa-list card_main_icon disabled" title="lexicon extension not yet started"></i>
        <i class="post_cor_icon fas fas fa-cogs card_main_icon disabled" title="postcorrection not yet started"></i>
    </td>
    </tr>

    <tr>
        <td colspan="5">
            <h4 class="align-self-center"><i class="fas fa-stream green"></i> Statistics</h4>
            <div class="row">

                <% if(statistics.lines!=0) { %>

                <div class="col-md-4">
                  <div class="progress_title">Corrected lines</div>
                </div>
                <div class="col-md-8 align-self-center" title="<%-statistics.corLines%> of <%-statistics.lines%> lines corrected">
                  <div class="progress">
                    <% val = Math.round(statistics.corLines/statistics.lines*100) %>
                    <div class="progress-bar blue-bg" role="progressbar" style="width: <%-val%>%;" aria-valuenow="<%-val%>" aria-valuemin="0" aria-valuemax="100"><%-val%>%</div>
                    </div>
                </div>

                <% } if(statistics.tokens!=0) { %>

                <div class="col-md-4">
                  <div class="progress_title">Corrected words</div>
                </div>
                <div class="col-md-8 align-self-center" title="<%-statistics.corTokens%> of <%-statistics.tokens%> words corrected">
                  <div class="progress">
                    <% val = Math.round(statistics.corTokens/statistics.tokens*100) %>
                    <div class="progress-bar blue-bg" role="progressbar" style="width: <%-val%>%;" aria-valuenow="<%-val%>" aria-valuemin="0" aria-valuemax="100"><%-val%>%</div>
                    </div>
                </div>

                <% } if(statistics.corTokens!=0) { %>

                 <div class="col-md-4" >
                  <div class="progress_title">Estimated OCR word accuracy</div>
                </div>
                <div class="col-md-8 align-self-center" title="<%-statistics.ocrCorTokens%> of <%-statistics.corTokens%> correct ocr words">
                  <div class="progress">
                    <% val = Math.round(statistics.ocrCorTokens/statistics.corTokens*100) %>
                    <div class="progress-bar blue-bg" role="progressbar" style="width: <%-val%>%;" aria-valuenow="<%-val%>" aria-valuemin="0" aria-valuemax="100"><%-val%>%</div>
                    </div>
                </div>

                <% } if(statistics.acTokens!=0) { %>

                <div class="col-md-4" >
                  <div class="progress_title">Automatically corrected words</div>
                </div>
                <div class="col-md-8 align-self-center" title="<%-statistics.acCorTokens%> of <%-statistics.acTokens%> words automatically corrected">
                  <div class="progress">
                    <% val = Math.round(statistics.acCorTokens/statistics.acTokens*100) %>
                    <div class="progress-bar blue-bg" role="progressbar" style="width: <%-val%>%;" aria-valuenow="<%-val%>" aria-valuemin="0" aria-valuemax="100"><%-val%>%</div>
                    </div>
                </div>

                <% } %>

              </div>


        </td>
    </tr>


 </tbody>

</table>

</div>

</div>

</div>
