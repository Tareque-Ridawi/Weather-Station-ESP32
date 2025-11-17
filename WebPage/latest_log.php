<?php
$mysqli = new mysqli("sqlXXX.aeonfree.net", "DB_USER", "DB_PASS", "DB_NAME");
$latest = $mysqli->query("SELECT * FROM logs ORDER BY id DESC LIMIT 1")->fetch_assoc();

if ($latest) {
    echo "<p><strong>Temperature:</strong> {$latest['temperature']} °C</p>";
    echo "<p><strong>Pressure:</strong> {$latest['pressure']} hPa</p>";
    echo "<p><strong>Altitude:</strong> {$latest['altitude']} m</p>";
    echo "<small class='text-muted'>Logged at {$latest['timestamp']}</small>";
} else {
    echo "<p>No logs yet.</p>";
}
?>
