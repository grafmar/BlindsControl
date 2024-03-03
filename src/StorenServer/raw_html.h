// Workaround for sketch preprocessor bug to put this raw string in separate header file

const char HTML[] PROGMEM = R"=====( 
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Storensteuerung</title>
	<link rel="shortcut icon" href="data:image/x-icon;base64,AAABAAEAEBACAAEAAQCwAAAAFgAAACgAAAAQAAAAIAAAAAEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////AAAAAAB//gAAfv4AAH7+AAB+/gAAfv4AAH7+AAB6vgAAfH4AAH7+AAD//wAAAAAAAP//AAAAAAAA//8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" />
    <style>
        body{margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}
        table{border-collapse:collapse}
        th,td{padding:5px;padding-left:20px;padding-right:20px;font-size:50px;text-align:center}
        a{text-decoration:none;color:inherit}
        button{width:100px;height:100px;font-size:50px}
    </style>
</head>
<body>
    <table>
        <tr><th>1</th><th>2</th><th>3</th></tr>
        <tr><td><button title="NumpadSubtract" onclick="sendControl('/l0')">&#8607;</button></td><td><button title="NumpadDivide" onclick="sendControl('/l2')">&#8607;</button></td><td><button title="NumpadMultiply" onclick="sendControl('/l4')">&#8607;</button></td></tr>
        <tr><td><button title="Numpad7" onclick="sendControl('/s0')">&#8593;</button></td><td><button title="Numpad8" onclick="sendControl('/s2')">&#8593;</button></td><td><button title="Numpad9" onclick="sendControl('/s4')">&#8593;</button></td></tr>
        <tr><td><button title="Numpad4" onclick="sendControl('/s1')">&#8595;</button></td><td><button title="Numpad5" onclick="sendControl('/s3')">&#8595;</button></td><td><button title="Numpad6" onclick="sendControl('/s5')">&#8595;</button></td></tr>
        <tr><td><button title="Numpad1" onclick="sendControl('/l1')">&#8609;</button></td><td><button title="Numpad2" onclick="sendControl('/l3')">&#8609;</button></td><td><button title="Numpad3" onclick="sendControl('/l5')">&#8609;</button></td></tr>
    </table>
<script>
function sendControl(url) {
  const xhr = new XMLHttpRequest();
  xhr.open("GET", url);
  xhr.send();
}
document.onkeydown = function(e) {
  if (e.code == "NumpadSubtract") {e.preventDefault();sendControl('/l0'); }
  if (e.code == "Numpad7")        {e.preventDefault();sendControl('/s0'); }
  if (e.code == "Numpad4")        {e.preventDefault();sendControl('/s1'); }
  if (e.code == "Numpad1")        {e.preventDefault();sendControl('/l1'); }
  
  if (e.code == "NumpadDivide")   {e.preventDefault();sendControl('/l2'); }
  if (e.code == "Numpad8")        {e.preventDefault();sendControl('/s2'); }
  if (e.code == "Numpad5")        {e.preventDefault();sendControl('/s3'); }
  if (e.code == "Numpad2")        {e.preventDefault();sendControl('/l3'); }
  
  if (e.code == "NumpadMultiply") {e.preventDefault();sendControl('/l4'); }
  if (e.code == "Numpad9")        {e.preventDefault();sendControl('/s4'); }
  if (e.code == "Numpad6")        {e.preventDefault();sendControl('/s5'); }
  if (e.code == "Numpad3")        {e.preventDefault();sendControl('/l5'); }
};
</script>
</body></html>
)=====";
