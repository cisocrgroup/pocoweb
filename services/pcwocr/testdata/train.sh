#!/bin/sh

src=shift;
dest=shift;

if [[ ! -d "$dest" ]]; then
	cp -r "$src" "$dest" || exit 1
	for json in "$dest/*.json"; do
		cat "$json" | jq ".earlyStoppingBestModelOutputDir=$dest" > "$json.tmp" || exit 1
		mv "$json.tmp" "$json"
	done
fi
cat > "$dest.desc.txt" <<EOF
$(basename $dest) crated by $(basename $src) at $(date)
EOF

calamari-resume-training "$@"
exit $?
