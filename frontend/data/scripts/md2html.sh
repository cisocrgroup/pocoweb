#!/bin/bash
text=$(<"$1")
text=${text//\"/\\\"}
text=${text//	/\\\t}
text=${text//
/\\\n}

curl -Ss https://api.github.com/markdown -d "{\"mode\":\"gfm\",\"text\":\"$text\",\"mode\":\"markdown\",\"context\":\"\"}" \
	| sed -e 's#<span class="pl-c1">false</span>#<span class="pl-bl">false</span>#' \
		  -e 's#<span class="pl-c1">true</span>#<span class="pl-bl">true</span>#'
echo ""
