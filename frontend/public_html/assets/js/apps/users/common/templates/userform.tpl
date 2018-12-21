
<div class="large-12 columns bg_layer">

<form id="user-form" >
  <fieldset>
   <% if(username=="") { %> 
    <legend>Register</legend>
   <% } %> 

   <div class="row">
      <div class="small-6 columns ">
        <label for="username">Username <small>required</small>
         <input type="text" id="username" name="username" required pattern="[a-z-A-Z]+" placeholder="Username" autocomplete="off" value="<%-username%>">
         <small class="error" style="display: none;"></small>
        </label>

      </div>
     </div>


 <div class="row">
      <div class="large-6 columns">
        <label for="firstname">Firstname <small>required</small>
          <input  type="text" id="firstname" placeholder="Firstname" name="firstname" required="" value="<%-firstname%>">
          <small class="error" style="display: none;"></small>
        </label>
      </div>
     <div class="large-6 columns">
        <label for="lastname">Lastname <small>required</small>
          <input  type="text" id="lastname" placeholder="Lastname" name="lastname" required="" value="<%-lastname%>">
          <small class="error" style="display: none;"></small>
        </label>
      </div> 
    </div>


     <% if(username=="") { %> 

    <div class="row">
      <div class="large-6 columns">
        <label for="password">Password <small>required</small>
          <input type="password" id="password" placeholder="Password" name="password" required="">
          <small class="error" style="display: none;"></small>
        </label>
      </div>
     <div class="large-6 columns">
        <label for="confirmPassword">Confirm Password <small>required</small>
          <input type="password" id="confirmPassword" placeholder="Confirm Password" name="confirmPassword" required="" data-equalto="password">
          <small class="error" style="display: none;"></small>
        </label>
      </div> 
    </div>

    <% } %> 



    <div class="row">
      <div class="large-6 columns">
        <label for="email">Email
          <input type="email" id="email" placeholder="me@xyz.com"  name="email"  required=""value="<%-email%>" >
          <small class="error" style="display: none;"></small>
        </label>
      </div>
    </div>



 <% if(role_idx>1) { %> 


  <div class="row">
        <div class="small-6 columns ">
          <label for="role">Role <small>required</small>
          <select id="roles">
          <% _.each(roles, function(role_item) { %>  
           <option value="<%-role_item%>"><%-role_item%></option>
          <% }); %>

           </select> 
          <small class="error" style="display: none;"></small>
          </label>

        </div>
 </div>

  <div class="row">


  <div class="medium-6 columns">
  <input id="verified_checkbox" type="checkbox"><label for="verified_checkbox">Verified </label>
  </div>


  <div class="medium-6 columns">
  <input id="notify" type="checkbox"><label for="notify">Notify user via email about account verification </label>
  </div>
</div>

 <% } %> 



 <% if(username=="") { %> 

    <div class="row">
      <div class="large-6 columns">
        <label for="securitycode">Security Code
          <input type="text" id="securitycode" placeholder="Please enter the correct answer to the calculation on the right"  name="securitycode"  required=""value="<%-securitycode%>" >
          <small class="error" style="display: none;"></small>
        </label>
      </div>

      <div class="large-6 columns" style="text-align: center">
        <fieldset class="captcha_set" >
         <img src="assets/images/captcha/<%-captcha%>" style="margin-bottom:2px" >
         </fieldset>
      </div>
    </div>

<% } %>


    <div class="row">
      <div class="large-12 columns">
        <hr>
      </div>
    </div>


    <div class="row">
      <div class="large-12 columns">

       <% if(username=="") { %> 
         <!-- <button type="submit" class="medium button success js-submit">Submit</button> -->

         <div class="left">

         <div class="small_icon_parent">

                 
                  <img src="assets/images/icons/bg_icon.png">
                  <a class="js-submit">
                  <div class="small_btniconcontainer">
                  <i class="fa fa-check"></i>
                  </div>
                  </a>
                 

                  <a class="js-submit">
                  <div class="small_bubblebtnbackground"> </div>
                  </a>
                  <div class="small_bubbleshadow"> </div>
        </div>

      <div class="center_parent">
      <p class="defaultsh subheader">Submit</p>
      </div>
      </div>

       <% } else { %> 
         <div type="submit" class="medium button success js-submit">Update</div>
         <div type="submit" class="medium button alert success js-delete">Delete</div>
       <% } %> 

        <% if (asModal) { %>
         <!-- <button  type="submit" class="medium button right js-close"><i class="fa fa-times"></i></button> -->

         <div class="right">

         <div class="small_icon_parent">

                 
                  <img src="assets/images/icons/bg_icon.png">
                  <a class="js-close">
                  <div class="small_btniconcontainer">
                  <i class="fa fa-close"></i>
                  </div>
                  </a>
                 
                  <a class="js-close">
                  <div class="small_bubblebtnbackground"> </div>
                  </a>
                  <div class="small_bubbleshadow"> </div>
        </div>

        <div class="center_parent">
        <p class="defaultsh subheader">Close</p>
        </div>

      </div>

       <% } else { %>  
          <button type="submit" class="medium button right js-back"><i class="fa fa-chevron-left"></i> Back</button>
       <% } %> 
      </div>
    </div> 

  </fieldset>
</form>

</div>