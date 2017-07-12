<?php
require_once(dirname(__FILE__) . "/config.php");
global $config;
echo 'var config = ', json_encode($config), ';';
?>
