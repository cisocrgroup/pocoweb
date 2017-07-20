<?php
require_once(dirname(__FILE__) . "/config.php");
global $config;
echo 'var PCW = {};';
echo 'PCW.config = ', json_encode($config), ';';
?>
