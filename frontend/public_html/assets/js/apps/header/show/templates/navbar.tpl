




<nav class="navbar navbar-expand-lg navbar-light navbar-pcw">
   <a class="navbar-brand" href="#home"><img style="width: 160px;" src="assets/images/pocoweb_logo.png"></a>
  <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>
  <div class="collapse navbar-collapse" id="navbarSupportedContent">
     <ul class="navbar-nav mr-auto">
      <li class="nav-item">
        <a class="nav-link" href="#projects">Projects</a>
      </li>
	  	<% if (user.id != -1 && user.admin) { %>
       <li class="nav-item">
        <a class="nav-link" href="#users">Users</a>
      </li>
	  <% } %>
       <li class="nav-item">
        <a class="nav-link" href="#users/account">Account</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="#documentation">Documentation</a>
      </li>
    </ul>

     <ul class="navbar-nav my-2 my-lg-0 right-nav">
      <% if (user.id != -1) { %>
        <li><p class="navbar-text" style="margin:0;">Logged in as: <span class="loginname"> <%-user.name%></span></p></li>
        <li class="nav-item js-logout"><a href="#" class="nav-link">Logout</a></li>

      <% } else { %>
       <li class="nav-item js-login">
        <a class="nav-link" href="#"><i class="fas fa-sign-in-alt fa-sm"></i> Login</a>
      </li>
      <% } %>
    </ul>
   <ul class="navbar-nav my-2 my-lg-0 version-nav">
       <li class="nav-item">
        <p class="navbar-text" style="margin:0;"> Api-Version: <%-version%></p>
      </li>
    </ul>
      <ul class="navbar-nav my-2 my-lg-0 logs-nav">
       <li class="nav-item">
      <div class="mini-btn js-logs" style="margin-left: 5px;" title="Show Logs"><i class="fas fa-clipboard-list"></i></div>
      </li>
    </ul>
  </div>
</nav>
