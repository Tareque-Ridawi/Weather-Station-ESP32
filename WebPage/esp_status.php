<?php
$mysqli = new mysqli("sqlXXX.aeonfree.net", "DB_USER", "DB_PASS", "DB_NAME");
$controls = $mysqli->query("SELECT last_seen FROM controls WHERE id=1")->fetch_assoc();

$last_seen = strtotime($controls['last_seen']);
$is_online = (time() - $last_seen < 30);

if ($is_online) {
    echo '<div class="alert alert-success">ESP32 Status: <strong>Online</strong></div>';
} else {
    echo '<div class="alert alert-danger">ESP32 Status: <strong>Offline</strong></div>';
}
?>
