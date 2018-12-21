
<?php

while ($record = mysqli_fetch_assoc($result)) {
   $bmlo = $record["F41"];
   $wd = "http://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&property=P18&entity=Q".$record["F45"];
   curl_setopt($ci, CURLOPT_URL, $wd);
   $cr = curl_exec($ci);
   if (preg_match("/\"value\":\"(.*\.jpg)\"/", $cr, $matches)) {
     $matches[1] = str_replace(" ", "_", $matches[1]);
     while ($u = strpos($matches[1], "\\u")) {
       $matches[1] = str_replace(substr($matches[1], $u, 6), chr(hexdec(substr($matches[1], $u+2, 4))), $matches[1]);
     }
     $file = "http://commons.wikimedia.org/wiki/Special:Redirect/file/".$matches[1];
     curl_setopt($cl, CURLOPT_URL, $file);
     $cr = curl_exec($cl);
     if (preg_match("/Location: (.*)\\r/", $cr, $location)) {
       $fp = fopen($tempfile, "w");
       curl_setopt($cp, CURLOPT_URL, $location[1]);
       curl_setopt($cp, CURLOPT_FILE, $fp);
       curl_exec($cp);
       fclose($fp);
       if (filesize($tempfile)) {
         $target = $mediadir.$bmlo.".jpg";
         $cmd = "convert $tempfile -resize @120000 $target";
         exec($cmd);
         unlink($tempfile);
         $qstr = "INSERT IGNORE Bild SET F41='$bmlo', Credits='Wikimedia Commons', Link='".mysqli_real_escape_string($pj_Link, "http://commons.wikimedia.org/wiki/File:".utf8_encode($matches[1]))."', Cache=1";
echo $qstr.";<br />";
         mysqli_query($pj_Link, $qstr);
       }
     }
   }
   sleep(1);
}

?>