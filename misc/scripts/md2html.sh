#!/bin/bash
text=$(<"$1")
text=${text//\"/\\\"}
text=${text//	/\\\t}
text=${text//
/\\\n}

#curl -Ss https://api.github.com/markdown -d "{\"text\":\"$text\",\"mode\":\"gfm\",\"context\":\"\"}"
curl -Ss https://api.github.com/markdown -d "{\"text\":\"$text\",\"mode\":\"markdown\",\"context\":\"\"}"
echo ""
