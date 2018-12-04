rem ========================
rem build script for windows
rem ========================

@echo off

set arg1=%1

copy index.html ..\webapp\index.html 
copy assets\js\popper.js ..\webapp\assets\js\popper.js

copy assets\js\vendor\jquery.js ..\webapp\assets\js\jquery.js

rd /s /q ..\webapp\assets\images
robocopy assets\images ..\webapp\assets\images /s /e 

rd /s /q ..\webapp\assets\css
robocopy assets\css ..\webapp\assets\css /s /e

cd assets\js
node r.js -o build.js

if "%arg1%" == "-doc" (
	cd ..\..\..\
	cd resources\raml
	ryaml2json api.raml > api.json
	copy api.json ..\..\webapp\assets\js\api.json
	cd ..\..\
	cd webapp-src
) else (
  cd ..\..\
)

@echo on