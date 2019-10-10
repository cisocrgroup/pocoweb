#!/bin/bash

source "/apps/ocrd-cis-lib.sh"

if [[ "$#" -ne 3 ]]; then
	echo "usage: $0 CONFIG PROFILE OUTPUT"
	exit 1
fi

config=$1
profile=$2
out=$3
dir=$(dirname "$3")
ws="$dir/workspace"
mets="$ws/mets.xml"

cat "$config" \
	| jq ".lexiconExtensionProtocol=\"$out\"" \
	| jq ".profiler.config=\"$profile\"" \
	| jq '.profiler.cacheDir=""' \
	| jq '.profiler.executable=""' \
	| jq '.profiler.type="file"' \
	| jq '.runLexiconExtension=true' \
	| jq '.runDesicionMaker=false' \
    > "$dir/el-config.json"

rm -rf "$ws" # remove any old workspace
ocrd workspace init "$ws"
for xml in $dir/src/xmls/*.xml; do
	img=$(ocrd-cis-find-image-for-xml "$dir/src/imgs" "$xml")
	ocrd-cis-add-xml-image-pair "$mets" "$xml" "OCR-D-CIS-POCOWEB-MOCR" "$img" "OCR-D-CIS-POCOWEB-IMG"
done

ocrd-cis-log-debug /apps/ocrd-cis-post-correct.sh \
	--output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
	--input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
	--mets "$mets" \
	--parameter "$dir/el-config.json"
/apps/ocrd-cis-post-correct.sh \
	--output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
	--input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
	--mets "$mets" \
	--parameter "$dir/el-config.json"
