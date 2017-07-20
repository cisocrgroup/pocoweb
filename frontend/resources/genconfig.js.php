<?php
require_once(dirname(__FILE__) . "/config.php");
global $config;
echo 'var pcw = {};';
echo 'pcw.config = ', json_encode($config), ';';
?>
