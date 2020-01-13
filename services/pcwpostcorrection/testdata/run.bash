#!/bin/bash

source "ocrd-cis-lib.sh" || exit 1

if [[ "$#" -ne 3 ]]; then
	echo "usage: $0 [dm|le] DIR PROFILE"
	exit 1
fi

cmd=$1
dir=$2
profile=$3
ws="$dir/workspace"
config="/etc/skel/pcwpostcorrection.json"
alignfg="OCR-D-CIS-POCOWEB-ALIGN"
outputfg="OCR-D-CIS-POCOWEB-POST-CORRECTION"
METS="$ws/mets.xml"
LOG_LEVEL=DEBUG

# make the directory with all children
mkdir -p "$dir"

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

# Select configuration file to use.
PARAMETER=$config
case "$cmd" in
	le) PARAMETER=$dir/le-config.json;;
	dm) PARAMETER=$dir/dm-config.json;;
	*) echo "invalid command: $cmd"; exit 1;;
esac

# Remove master OCR and according image workspace, add them again and
# align if the workspace already exists.  Initialize a new workspace.
doOCR=true
if [[ -d "$ws" ]]; then
	doOCR=false
	pushd "$ws"
	ocrd-cis-debug "cleaning up workspace $ws"
	ocrd workspace remove-group --recursive --force \
		 "OCR-D-CIS-POCOWEB-MOCR" "OCR-D-CIS-POCOWEB-IMG" "$alignfg"
	popd
else
	ocrd-cis-debug "intializing workspace $ws"
	ocrd workspace init "$ws"
fi

# Add new master OCR and the according images.  The directories
# src/{xml,img} are created and updated by the pcwpostcorrect process.
# They contain the master OCR page XML files and their according image
# files.
for xml in $dir/src/xmls/*.xml; do
	img=$(ocrd-cis-find-image-for-xml "$dir/src/imgs" "$xml")
	ocrd-cis-debug "adding $xml $img"
	ocrd-cis-add-xml-image-pair "$METS" "$xml" "OCR-D-CIS-POCOWEB-MOCR" "$img" "OCR-D-CIS-POCOWEB-IMG"
done

# Run slave OCRs (if $doOCR=true) and build up file group list for
# alignment.
# PARAMETER, METS, XML_INPUT_FILE_GRP, XML_OUTPUT_FILE_GRP and
# LOG_LEVEL must be set, since they must be available for the OCR eval
# command.
XML_INPUT_FILE_GRP="OCR-D-CIS-POCOWEB-MOCR"
XML_OUTPUT_FILE_GRP="OCR-D-CIS-POCOWEB-MOCR"
alignfgs="$XML_INPUT_FILE_GRP" # master ocr comes first
n=1
for run in $(cat "$PARAMETER" | jq -r '.ocrSteps[] | @base64'); do
	XML_OUTPUT_FILE_GRP="$XML_OUTPUT_FILE_GRP-OCR$n"
	n=$((n+1))
	alignfgs="$alignfgs,$XML_OUTPUT_FILE_GRP"

	# run OCR only if it not yet exists
	if [[ "$doOCR" == true ]]; then
		run=$(echo $run | base64 -d)
		eval ocrd-cis-debug "$run"
		eval $run || exit 1
	fi
done

# Align master OCR with slave OCRs.
ocrd-cis-debug ocrd-cis-align --mets "$METS" \
			   --input-file-grp "$alignfgs" \
			   --output-file-grp "$alignfg"
ocrd-cis-align --mets "$METS" \
			   --input-file-grp "$alignfgs" \
			   --output-file-grp "$alignfg"

# Run post correction with the according configuration file.  The
# configuration file contains settings that either run le or dm.
main="de.lmu.cis.ocrd.cli.Main"
jar=$(ocrd-cis-data -jar)
nocr=$(jq ".ocrSteps | length+1" "$PARAMETER")
ocrd-cis-debug java -Dfile.encoding=UTF-8 -Xmx3g -cp "$jar" "$main" \
	 --log-level "$LOG_LEVEL" \
	 -c post-correct \
	 --mets "$METS" \
	 --parameter <(jq ".postCorrection.nOCR = \"$nocr\" | .postCorrection" "$PARAMETER") \
	 --input-file-grp "$alignfg" \
	 --output-file-grp "$outputfg"
java -Dfile.encoding=UTF-8 -Xmx3g -cp "$jar" "$main" \
	 --log-level "$LOG_LEVEL" \
	 -c post-correct \
	 --mets "$METS" \
	 --parameter <(jq ".postCorrection.nOCR = \"$nocr\" | .postCorrection" "$PARAMETER") \
	 --input-file-grp "$alignfg" \
	 --output-file-grp "$outputfg"
