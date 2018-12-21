<?php

$column_items = array();
$column_aliases = array();

function get_column_items($col_expr)
{
  global $column_items, $column_aliases;

  $s = $col_expr;
  $item="";

  $opening_bracket=0;
  $single_quote=false;
  $double_quote=false;
  $r = "";
  $as_found = false;

  while (true)
  {
    if ($single_quote) $relevant_characters = "'";
    elseif ($double_quote) $relevant_characters = '"';
    else { $relevant_characters = "'\"()"; if ( ! $opening_bracket) $relevant_characters.= ","; }

    $pattern = "[$relevant_characters]";

    if ( preg_match("/,/",$relevant_characters) ) $pattern.= "| as ";

    if ( !preg_match('/('.$pattern.')/i', $s, $matches, PREG_OFFSET_CAPTURE)) break;
    $c =  $matches[0][0];
    $p =  $matches[0][1];

    $r.= substr($s,0,$p);
    $s = substr($s,$p+strlen($c));
    if ($c == '(' ) $opening_bracket++;
    elseif ($c == ')' ) $opening_bracket--;
    elseif ($c == '\'') $single_quote= ! $single_quote;
    elseif ($c == '"' ) $double_quote= ! $double_quote;
    elseif ($c == ',' ) { if ( $item == "" ) { $item = $r; $r = ""; }
    if ( $as_found ) { $col_item = trim($item); $col_alias = $r; }
    else { $col_item = trim($item); $col_alias = ""; }
    array_push($column_items,trim($col_item)); array_push($column_aliases,trim($col_alias));
    $r=""; $item=""; $as_found=false;}
    elseif (preg_match("/^ as $/i",$c)) { $as_found=true; $item = $r; $r=""; }

    if ( $c != ',' && !preg_match("/^ as $/i",$c) ) { $r.=$c; }

  } // while

  if ( $r != "" || $s != "" )
  {
    if ( $item == "" ) { $item = $s; $s=""; }
    if ( $as_found ) { $col_item = trim($item); $col_alias = $r.$s; }
    else { $col_item = trim($r.$item); $col_alias = ""; }
    array_push($column_items,trim($col_item)); array_push($column_aliases,trim($col_alias));
    $r=""; 
  }

} // get_column_items()

?>
