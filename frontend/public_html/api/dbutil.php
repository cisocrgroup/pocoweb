
<?php
 
function get($table,$columns,$condition,$group_by,$having,$order_by,$limit) {
global $mysqli;


$colsarray = array();
$colsarray = explode(",",$columns);

$sql = "SELECT ";
$sql.= $columns." FROM ".$table;

if ( $condition != "" ) $sql.= " WHERE ".$condition;
if ( $group_by != "" ) $sql.= " GROUP BY ".$group_by;
if ( $having != "" ) $sql.= " HAVING ".$having;
if ( $order_by != "" ) $sql.= " ORDER BY ".$order_by;
if ( $limit != "" ) $sql.= " LIMIT ".$limit;
// echo $sql;

$res = $mysqli->query($sql);


$output=array();
if($res){
while($row = $res->fetch_assoc()){
	foreach ($colsarray as $col){
    $col=str_replace("'","",$col);

    if($row[$col]==null) {$row_array[$col]=""; }
	  else $row_array[$col] = $row[$col];
	}	
 array_push($output,$row_array);
}

return $output;
}
else return -1;
}


 
function get_aliases($table,$columns,$resultcolumns,$condition,$group_by,$having,$order_by,$limit) {
global $mysqli;

$colsarray = array();
$colsarray = explode(",",$resultcolumns);

$sql = "SELECT ";
$sql.= $columns." FROM ".$table;

if ( $condition != "" ) $sql.= " WHERE ".$condition;
if ( $group_by != "" ) $sql.= " GROUP BY ".$group_by;
if ( $having != "" ) $sql.= " HAVING ".$having;
if ( $order_by != "" ) $sql.= " ORDER BY ".$order_by;
if ( $limit != "" ) $sql.= " LIMIT ".$limit;
  // echo $sql;

$res = $mysqli->query($sql);


$output=array();
if($res){
while($row = $res->fetch_assoc()){
  foreach ($colsarray as $col){
    $col=str_replace("'","",$col);

    if($row[$col]==null) {$row_array[$col]=""; }
    else $row_array[$col] = $row[$col];
  } 
 array_push($output,$row_array);
}

return $output;
}
else return -1;
}


function query_single($table,$columns,$condition,$group_by,$having,$order_by,$limit) {
global $mysqli;
global $column_items, $column_aliases;
//$sql_calc_found_rows = ($_GET["sql_calc_found_rows"]!="");

$order = array("\r\n", "\n", "\r");

$sql = "SELECT ";
//if ( $sql_calc_found_rows ) $sql.= " SQL_CALC_FOUND_ROWS ";
$sql.= $columns." FROM ".$table;
if ( $condition != "" ) $sql.= " WHERE ".$condition;
if ( $group_by != "" ) $sql.= " GROUP BY ".$group_by;
if ( $having != "" ) $sql.= " HAVING ".$having;
if ( $order_by != "" ) $sql.= " ORDER BY ".$order_by;
if ( $limit != "" ) $sql.= " LIMIT ".$limit;
get_column_items($columns);


 // echo($sql);
$res = $mysqli->query($sql);

if($res->num_rows>0){
while($row = $res->fetch_assoc())
{

  $idx=0;
  foreach ($column_items as $column)
  {
    if ( $column_aliases[$idx]!="" ) $column = $column_aliases[$idx];
    $column = preg_replace('/(\'|")/',"",$column);
 
     $row_array[$column] = $row[$column];

    $idx++;
  } // foreach
} // while
return $row_array;
}
else return -1;

 $mysqli->close();

}

function geoLookup($string){
 
   $string = str_replace (" ", "+", urlencode($string));
   $details_url = "http://maps.googleapis.com/maps/api/geocode/json?address=".$string."&sensor=false";
 
   $ch = curl_init();
   curl_setopt($ch, CURLOPT_URL, $details_url);
   curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
   $response = json_decode(curl_exec($ch), true);
 
   // If Status Code is ZERO_RESULTS, OVER_QUERY_LIMIT, REQUEST_DENIED or INVALID_REQUEST
   if ($response['status'] != 'OK') {
    return null;
   }
 
   //print_r($response);
    $geometry = $response['results'][0]['geometry'];
 
 
    $array = array(
        'lat' => $geometry['location']['lat'],
        'lng' => $geometry['location']['lng']
    );
 
    return $array;
 
}

function deleteFromDb($table,$condition) {
global $mysqli;

$sql = "DELETE FROM ";
$sql.= $table." WHERE ".$condition;

//echo "SQL:". $sql."</br>";

$mysqli->query($sql);
$result=$mysqli->insert_id;

return -1;
}


function clearTable($table) {
global $mysqli;

$sql = "DELETE FROM ";
$sql.= $table;

//echo "SQL:". $sql."</br>";

$mysqli->query($sql);
$result=$mysqli->insert_id;

return -1;
}

function deleteFromDbReturnIds($table,$condition) {
global $mysqli;

$ids = get($table,"id",$condition,"","","","");
// $sql = "DELETE FROM ";
// $sql.= $table." WHERE ".$condition;

// //echo "SQL:". $sql."</br>";

// $mysqli->query($sql);
if ($ids!=-1) return $ids;
return array();
}

function insertIntoDb($table,$columns,$values) {
global $mysqli;

$sql = "INSERT INTO ";
$sql.= $table." (".$columns.") VALUES (".$values.") ";

//echo "SQL:". $sql."</br>";

$mysqli->query($sql);
$result=$mysqli->insert_id;

return $result;
}


function updateDb($table,$column,$value,$condition) {
global $mysqli;

$sql = "UPDATE ";
$sql.= $table." SET ".$column."='".$value."' WHERE ".$condition;

// echo "SQL:". $sql."</br>";

$mysqli->query($sql);
$result=$mysqli->insert_id;

return $result;
}



function updateDbMult($table,$cols_array,$value_array,$condition) {
global $mysqli;

$sql = "UPDATE ".$table." SET ";

for($i=0;$i<sizeOf($cols_array);$i++){

if ($i<sizeOf($cols_array)-1) $sql.=$cols_array[$i]."='".$value_array[$i]."', ";
else $sql.=$cols_array[$i]."='".$value_array[$i]."'";
}

$sql .=$condition;

//echo "SQL:". $sql."</br>";


$mysqli->query($sql);
$result=$mysqli->insert_id;

return $result;
}


function idCheck($table,$column,$value) {
global $mysqli;

$sql = "SELECT id FROM ".$table." WHERE ".$column."='".$value."';" ;

//echo "SQL:". $sql."</br>";
$res = $mysqli->query($sql);

$output=array();
while($row = $res->fetch_assoc()){
 $row_array['id'] = $row['id'];
 array_push($output,$row_array);
}

//echo "checkoutput: ".$output[0]['id'];
if (sizeOf($output)>0) return $output[0]['id'];
else return -1;
}

function locCheck($table,$country_id,$place,$address,$lat,$lon) {
global $mysqli;

$sql = "SELECT id,lat,lon FROM ".$table." WHERE country_id='".$country_id."' AND place='".$place."' AND address='".$address."';" ;

//echo "SQL:". $sql."</br>";
$res = $mysqli->query($sql);

$output=array();
while($row = $res->fetch_assoc()){
 $row_array['id'] = $row['id'];
 $row_array['lat'] = $row['lat'];
 $row_array['lon'] = $row['lon'];

 array_push($output,$row_array);
}


//echo "checkoutput: ".$output[0]['id'];
if (sizeOf($output)>0) return $output[0]['id'];
else return -1;
}


function parseDate($date){

    $array =explode('-', $date);

    if(sizeof($array)==1) {

    	if(strlen($array[0])==1) return "000".$array[0]."-00-00";
    	if(strlen($array[0])==2) return "00".$array[0]."-00-00";
    	if(strlen($array[0])==3) return "0".$array[0]."-00-00";
    	if(strlen($array[0])==4) return $array[0]."-00-00";
     }

    if(sizeof($array)==2&&strlen($array[0])==2&&strlen($array[1])==2) {
      return "0000-".$array[0]."-".$array[1];
    }
    if(sizeof($array)==2&&strlen($array[0])==4&&strlen($array[1])==2) {
      return $array[0]."-".$array[1]."-00";
    }

    if(sizeof($array)==3&&strlen($array[2])==4&&strlen($array[0])==2&&strlen($array[1])==2) {
     return $array[2]."-".$array[1]."-".$array[0];	
    }

    return $date;
}

?>