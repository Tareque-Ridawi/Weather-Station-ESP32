<?php
$mysqli = new mysqli("fdb1032.awardspace.net", "4494781_weather", "VE11938JX", "4494781_weather");

$id = $_POST['id'] ?? 1;
$mysqli->query("UPDATE controls SET last_seen = NOW() WHERE id=$id");

echo "OK";
?>
