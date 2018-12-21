
<?php



// $host = "gwi-sql.gwi.uni-muenchen.de:3312";
// $user = "u1";
// $db = "tools";
// $pw = "A)dchVI405";
// $port = 3312;


$host = "127.0.0.1";
$user = "root";
$db = "armep";
$pw = "";

function connect() {
global $host;
global $pw;
global $user;
global $db;
global $mysqli;

$mysqli = new mysqli($host,$user,$pw,$db);

if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}


$mysqli->set_charset("utf8");

}



?>
