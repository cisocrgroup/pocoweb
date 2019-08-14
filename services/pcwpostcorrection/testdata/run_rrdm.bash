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
alex="$dir/alex.json"

# for now
sleep 15
exit 0

cat "$config" \
	| jq ".decisionMakerProtocol=\"$out\"" \
	| jq ".profiler.config=\"$profile\"" \
	| jq '.profiler.cacheDir=""' \
	| jq '.profiler.executable=""' \
	| jq '.profiler.type="file"' \
	| jq '.runLexiconExtension=false' \
	| jq '.runDecisionMaker=true' \
    > "$dir/rrdm-config.json"

# run post correction over aligned files
jar=$(cat "$dir/rrdm-config.json" | jq --raw-output .jar)
main="de.lmu.cis.ocrd.cli.Main"
ocrd-cis-log-debug java -Dfile.encoding=UTF-8 -Xmx3g -cp "$jar" "$main" -c post-correct\
	 --log-level DEBUG \
	 --mets "$mets" \
	 --parameter "$dir/rrdm-config.json" \
	 --output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
	 --input-file-grp "OCR-D-CIS-ALIGN"
java -Dfile.encoding=UTF-8 -Xmx3g -cp "$jar" "$main" -c post-correct\
	 --log-level DEBUG \
	 --mets "$mets" \
	 --parameter "$dir/rrdm-config.json" \
	 --output-file-grp "OCR-D-CIS-POCOWEB-POST-CORRECTION" \
	 --input-file-grp "OCR-D-CIS-ALIGN"
