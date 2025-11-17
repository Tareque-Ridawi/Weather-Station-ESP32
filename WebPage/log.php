<?php
$mysqli = new mysqli("fdb17494.awardepace.net", "449e321_weather", "password", "4et4781_weather");
$temp = $_POST['temp'] ?? null;
$press = $_POST['press'] ?? null;
$alt = $_POST['alt'] ?? null;

if ($temp !== null && $press !== null && $alt !== null) {
    $stmt = $mysqli->prepare("INSERT INTO logs (temperature, pressure, altitude) VALUES (?, ?, ?)");
    $stmt->bind_param("ddd", $temp, $press, $alt);
    $stmt->execute();
    echo "OK";
} else {
    echo "Missing data";
}
?>
