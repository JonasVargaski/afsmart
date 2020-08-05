const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>TechNow - Wifi</title>
</head>

<body>
  <div>
    <h3 onclick="show()">Wifi Manager</h3>

    <label>SSID
      <input id="ssid" type="text">
    </label>
    <label>Password
      <input id="password" type="text">
    </label>

    <div id="hidden">
      <h4>Server Settings</h4>
      <label>Server
        <input id="server" type="text">
      </label>
      <label>Port
        <input id="port" type="text">
      </label>
    </div>

    <button onclick="save()">Save Settings</button>
  </div>
</body>

</html>
<style>
  body {
    width: 100%;
    margin: 0;
    background: #fbfbfb;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  div {
    width: 100%;
    max-width: 350px;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    padding: 25px;
    border: 1px solid #ccc;
    border-radius: 8px;
    background: #fff;
    margin: 15px;
  }

  #hidden {
    background: #eee;
    padding: 10px 25px;
    margin-top: 15px;
    max-width: 90%;
    display: none;
  }

  h3,
  h4 {
    padding: 0;
    margin: 8px;
  }

  label {
    font-weight: 600;
    font-size: 13px;
    width: 100%;
    margin-right: 17px;
  }

  input {
    width: 100%;
    color: #444;
    margin: 7px 0;
    border: 1px solid lightsteelblue;
    border-radius: 4px;
    padding: 8px;
    background: #fbfbfb;
    outline: none;
  }

  input:focus {
    border: 1px solid rgb(188, 173, 230);
  }

  button {
    outline: none;
    font-size: 16px;
    color: #fff;
    background: lightseagreen;
    border: 1px solid lightsteelblue;
    padding: 7px 8px;
    border-radius: 4px;
    width: 100%;
    margin-top: 25px;
    cursor: pointer;
  }
</style>

<script>
  function get(id) {
    return document.getElementById(id).value
  }

  function show() {
    document.getElementById('hidden').style.display = 'block';
  }

  function save() {
    fetch(`/save?ssid=${get('ssid')}&password=${get('password')}&server=${get('server')}&port=${get('port')}`, {
      method: 'POST',
    }).then(({ status }) => { if (status === 200) alert("Saved successfully!") })
  }
</script>
)rawliteral";