




<nav class="navbar navbar-expand-lg navbar-dark bg-primary navbar-pcw">
   <a class="navbar-brand" href="#home">PoCoWeb</a>
  <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>
  <div class="collapse navbar-collapse" id="navbarSupportedContent">
     <ul class="navbar-nav mr-auto">
      <li class="nav-item">
        <a class="nav-link" href="#projects">Projects</a>
      </li>
	  	<% if (user != null && user.admin) { %>
       <li class="nav-item">
        <a class="nav-link" href="#users">Users</a>
      </li>
	  <% } %>
       <li class="nav-item">
        <a class="nav-link" href="#users/account">Account</a>
      </li>
      <li class="nav-item">
        <a class="nav-link" href="#docs">Documentation</a>
      </li>
       <li class="nav-item">
        <a class="nav-link" href="#about">About</a>
      </li>
    </ul>

     <ul class="navbar-nav my-2 my-lg-0 right-nav">
      <% if (user != null) { %>
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
  </div>
</nav>
