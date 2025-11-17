<?php
$mysqli = new mysqli("fdb17494.awardepace.net", "449e321_weather", "password", "4et4781_weather");
$id = $_POST['id'] ?? 1;
$mysqli->query("UPDATE controls SET last_seen = NOW() WHERE id=$id");

echo "OK";
?>
