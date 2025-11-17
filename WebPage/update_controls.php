<?php
$mysqli = new mysqli("fdb1032.awardspace.net", "4494781_weather", "VE11938JX", "4494781_weather");

$id = $_POST['id'] ?? 1;
$temp_threshold = $_POST['temp_threshold'] ?? 30;
$press_threshold = $_POST['press_threshold'] ?? 1005;
$fan_state = $_POST['fan_state'] ?? 0;
$window_state = $_POST['window_state'] ?? 0;

$stmt = $mysqli->prepare("UPDATE controls SET temp_threshold=?, press_threshold=?, fan_state=?, window_state=? WHERE id=?");
$stmt->bind_param("ddiii", $temp_threshold, $press_threshold, $fan_state, $window_state, $id);
$stmt->execute();

echo "Updated";
?>
