<?php
$mysqli = new mysqli("fdb1032.awardspace.net", "4494781_weather", "VE11938JX", "4494781_weather");

// Get controls
$controls = $mysqli->query("SELECT * FROM controls WHERE id=1")->fetch_assoc();
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Weather Station Dashboard</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body class="bg-light">
<div class="container py-4">

  <div class="text-center mb-4">
    <h1 class="fw-bold">IoT Smart Weather Station</h1>
    <p class="lead">Monitor & Control Your System</p>
  </div>

  <!-- ESP32 Status -->
  <div id="esp-status">
    <div class="alert alert-secondary">Loading ESP32 status...</div>
  </div>

  <!-- Latest Sensor Data -->
  <div class="card mb-4 shadow-sm">
    <div class="card-header bg-primary text-white">Latest Sensor Data</div>
    <div class="card-body" id="sensor-data">
      <p>Loading sensor data...</p>
    </div>
  </div>

  <!-- Controls -->
  <div class="card shadow-sm">
    <div class="card-header bg-success text-white">Controls</div>
    <div class="card-body">
      <form method="post" action="update_controls.php" class="row g-3">

        <div class="col-md-6">
          <label class="form-label">Temperature Threshold (°C)</label>
          <input type="number" step="0.1" name="temp_threshold" value="<?= $controls['temp_threshold'] ?>" class="form-control">
        </div>

        <div class="col-md-6">
          <label class="form-label">Pressure Threshold (hPa)</label>
          <input type="number" step="0.1" name="press_threshold" value="<?= $controls['press_threshold'] ?>" class="form-control">
        </div>

        <div class="col-md-6">
          <label class="form-label">Fan</label>
          <select name="fan_state" class="form-select">
            <option value="0" <?= $controls['fan_state']==0?'selected':'' ?>>Auto/Off</option>
            <option value="1" <?= $controls['fan_state']==1?'selected':'' ?>>Force ON</option>
          </select>
        </div>

        <div class="col-md-6">
          <label class="form-label">Window</label>
          <select name="window_state" class="form-select">
            <option value="0" <?= $controls['window_state']==0?'selected':'' ?>>Auto/Closed</option>
            <option value="1" <?= $controls['window_state']==1?'selected':'' ?>>Force Open</option>
          </select>
        </div>

        <input type="hidden" name="id" value="1">

        <div class="col-12">
          <button type="submit" class="btn btn-success">Update Controls</button>
        </div>
      </form>
    </div>
  </div>

</div>

<!-- JS for Auto Updates -->
<script>
function fetchSensor() {
  fetch("latest_log.php")
    .then(res => res.text())
    .then(html => {
      document.getElementById("sensor-data").innerHTML = html;
    });
}

function fetchStatus() {
  fetch("esp_status.php")
    .then(res => res.text())
    .then(html => {
      document.getElementById("esp-status").innerHTML = html;
    });
}

setInterval(fetchSensor, 5000); // refresh sensor every 5s
setInterval(fetchStatus, 5000); // refresh status every 5s

fetchSensor();
fetchStatus();
</script>

</body>
</html>
