<?php

include 'get_column_items.php';
//  error_reporting(0);

function query($table,$columns,$condition,$group_by,$having,$order_by,$limit) {
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
$result = $mysqli->query($sql);


$output = array();
while($row = $result->fetch_assoc())
{

  $idx=0;
  foreach ($column_items as $column)
  {
    if ( $column_aliases[$idx]!="" ) $column = $column_aliases[$idx];
    $column = preg_replace('/(\'|")/',"",$column);
 
      // if($row[$column]==null){
      // 	if ($column=="venuetype"||$column=="capacity") $row_array[$column]="unknown";
      // 	else $row_array[$column]="";
      // } 
      // else
       $row_array[$column] = $row[$column];

    $idx++;
  } // foreach
 array_push($output,$row_array);
} // while

return $output;

 $mysqli->close();

}

?>
