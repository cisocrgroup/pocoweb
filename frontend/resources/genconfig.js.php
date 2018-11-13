<?php
require_once(dirname(__FILE__) . "/config.php");
global $config;
echo 'var pcw = {}', ";\n";
echo 'pcw.config = ', json_encode($config,JSON_PRETTY_PRINT), "\n";
?>
