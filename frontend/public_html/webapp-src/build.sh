#!/bin/bash
# build script for linux/Unix systems

cp index.html ../webapp/index.html
cp assets/js/popper.js ../webapp/assets/js

cp assets/js/vendor/jquery.js ../webapp/assets/js/jquery.js

rm -rf ../webapp/assets/images
cp -r assets/images ../webapp/assets
rm -rf ../webapp/assets/css
cp -r assets/css ../webapp/assets

cd assets/js
node r.js -o build.js

if [ "$1" == "-doc" ]; then
	cd ../../../
		cd resources/raml
		ryaml2json api.raml > api.json
    	cp api.json ../../webapp/assets/js/api.json

		cd ../../
		cd webapp-src
else
  cd ../../
fi

