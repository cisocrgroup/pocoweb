#!/bin/bash

source "ocrd-cis-lib.sh" || exit 1

# set log level
LOG_LEVEL=DEBUG

if [[ "$#" -ne 3 ]]; then
	echo "usage: $0 CMD DIR PROFILE"
	exit 1
fi

cmd=$1
dir=$2
profile=$3
ws="$dir/workspace"
mets="$ws/mets.xml"
config="/etc/skel/pcwpostcorrection.json"

# link model file into dir
if [[ ! -f "$dir/model.zip" ]]; then
	ln -s "/apps/models/model.zip" "$dir/model.zip"
fi

# le configuration
cat "$config" \
	| jq ".postCorrection.dir=\"$dir\"" \
	| jq ".postCorrection.profiler.path=\"$profile\"" \
	| jq '.postCorrection.runLE=true' \
	| jq '.postCorrection.runDM=false' \
    > "$dir/le-config.json"

# dm configuration
cat "$config" \
	| jq ".postCorrection.dir=\"$dir\"" \
	| jq ".postCorrection.profiler.path=\"$profile\"" \
	| jq '.postCorrection.runLE=false' \
	| jq '.postCorrection.runDM=true' \
    > "$dir/dm-config.json"

# Remove any old workspace and add image/mocr files to workspace
# if the workspace does not contain a ALIGN filegroup already
rm -rf "$ws"
ocrd workspace init "$ws"
for xml in $dir/src/xmls/*.xml; do
	img=$(ocrd-cis-find-image-for-xml "$dir/src/imgs" "$xml")
	ocrd-cis-add-xml-image-pair "$mets" "$xml" "OCR-D-CIS-POCOWEB-MOCR" "$img" "OCR-D-CIS-POCOWEB-IMG"
done

# run le or dm post correction
case "$cmd" in
	le)
		ocrd-cis-debug ocrd-cis-post-correct.sh \
					   --log-level "$LOG_LEVEL" \
					   --output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
					   --input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
					   --mets "$mets" \
					   --parameter "$dir/le-config.json";
		ocrd-cis-post-correct.sh \
			--log-level "$LOG_LEVEL" \
			--output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
			--input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
			--mets "$mets" \
			--parameter "$dir/le-config.json"
		;;
	dm)
		ocrd-cis-debug ocrd-cis-post-correct.sh \
					   --log-level "$LOG_LEVEL" \
					   --output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
					   --input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
					   --mets "$mets" \
					   --parameter "$dir/el-config.json"
	    ocrd-cis-post-correct.sh \
			--log-level "$LOG_LEVEL" \
			--output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
			--input-file-grp "OCR-D-CIS-POCOWEB-MOCR" \
			--mets "$mets" \
			--parameter "$dir/dm-config.json"
		;;
	*)
		echo "invalid command: $cmd"
		exit 1
		;;
esac
