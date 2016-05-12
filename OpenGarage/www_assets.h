const char html_ap_home[] PROGMEM = R"=====(<head>
<meta name='viewport' content='width=device-width,initial-scale=1.0,minimum-scale=1.0,user-scalable=no'>
<title>OpenGarage</title>
<style>body{font-family:'helvetica';}</style>
</head>
<body>
<caption><b>OpenGarage WiFi Config</caption><br><br>
<table cellspacing=4 id='rd'>
<tr><td>(Scanning...)</td></tr>
</table>
<table cellspacing=16>
<tr><td><input type='text' name='ssid' id='ssid'></td><td>(SSID)</td></tr>
<tr><td><input type='password' name='pass' id='pass'></td><td>(Password)</td></tr>
<tr><td colspan=2><p id='msg'></p></td></tr>
<tr><td><button type='button' id='butt' onclick='submit_form();' style='height:36px;width:180px'>Submit</button></td><td></td></tr>
</table>
<script>
function id(s) {return document.getElementById(s);}
function sel(i) {id('ssid').value=id('rd'+i).value;}
var tci;
function tryConnect() {
var xhr=new XMLHttpRequest();
xhr.onreadystatechange=function() {
if(xhr.readyState==4 && xhr.status==200) {
var jd=JSON.parse(xhr.responseText);
if(jd.ip==0) return;
var ip=''+(jd.ip%256)+'.'+((jd.ip/256>>0)%256)+'.'+(((jd.ip/256>>0)/256>>0)%256)+'.'+(((jd.ip/256>>0)/256>>0)/256>>0);
id('msg').innerHTML='<b><font color=green>Connected! Device IP: '+ip+'</font></b><br>Device is rebooting. Switch back to<br>the above WiFi network, and then<br>click the button below to redirect.'
id('butt').innerHTML='Go to '+ip;id('butt').disabled=false;
id('butt').onclick=function rd(){window.open('http://'+ip);}
clearInterval(tci);
}
}    
xhr.open('POST', 'jt', true); xhr.send();    
}
function submit_form() {
id('msg').innerHTML='';
var xhr=new XMLHttpRequest();
xhr.onreadystatechange=function() {
if(xhr.readyState==4 && xhr.status==200) {
var jd=JSON.parse(xhr.responseText);
if(jd.result==1) { tci=setInterval(tryConnect, 10000); return; }
id('msg').innerHTML='<b><font color=red>Error code: '+jd.result+', item: '+jd.item+'</font></b>';
id('butt').innerHTML='Submit';
id('butt').disabled=false;id('ssid').disabled=false;id('pass').disabled=false;
}
}
var comm='cc?ssid='+encodeURIComponent(id('ssid').value)+'&pass='+encodeURIComponent(id('pass').value);
xhr.open('POST', comm, true); xhr.send();
id('butt').innerHTML='Connecting...';
id('butt').disabled=true;id('ssid').disabled=true;id('pass').disabled=true;
}
		
function loadSSIDs() {
var xhr=new XMLHttpRequest();
xhr.onreadystatechange=function() {
if(xhr.readyState==4 && xhr.status==200) {
var response=JSON.parse(xhr.responseText), i;
id('rd').deleteRow(0);
for(i=0;i<response.networks.length;i++) {
var cell=id('rd').insertRow(-1).insertCell(0);
cell.innerHTML ="<tr><td><input type='radio' name='ssids' id='rd" + i + "' value='" + response.networks[i].ssid + "' onclick=sel(" + i + ")>" + response.networks[i].ssid + "(" + response.networks[i].rssi + ")" + "</td></tr>";

}
}
};
xhr.open('GET','/json/networks',true); xhr.send();
}
setTimeout(loadSSIDs, 1000);
</script>
</body>
)=====";
const char html_portal[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<!-- Standard Meta -->
<meta charset="utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">

<title>OpenGarage</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/semantic-ui/1.11.8/semantic.min.css"/>
<script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/semantic-ui/1.11.8/semantic.min.js"></script>

</head>
<body>
<div class="ui top attached menu">
<a href="/" class="header item">
OpenGarage
</a>
<a href="#" class="active item" data-tab="tab-status">
<i class="heartbeat icon"></i> Status
</a>
<a href="#/logs" class="item" data-tab="tab-logs">
<i class="history icon"></i> Logs
</a>
<a href="#/options" class="item" data-tab="tab-options">
<i class="options icon"></i> Options
</a>
<a href="#/about" class="item right" data-tab="tab-about">
<i class="info icon"></i> About
</a>
</div>

<p id="msg"></p>

<div class="ui segment">

<div class="ui bottom attached active tab" data-tab="tab-status">
<h3>Status</h3>

<div class="ui buttons">
<div id="status-refresh" class="ui button">
<i class="refresh icon"></i>Refresh Status
</div>

<div id="door-button" class="ui button primary">
<i class="grid layout icon"></i> Door Button
</div>
</div>

<b>Distance:</b>
<label id="lbl_dist">-</label>
			
<b>Door State:</b>
<label id="lbl_status">-</label>
			
<b>Read Count:</b>
<label id="lbl_beat">-</label>     


<script>
$(document).ready(function(){
/*$("#btn_rbt").click(function(e){
if(confirm("Reboot the device now?")){
var comm = "controller?reboot=1";
clear_msg();
$.getJSON(comm, function(jd) {
if(jd.result!=1) show_msg("Check device key and try again.",2000,"red");
else {
show_msg("Rebooting. Please wait...",0,"green");
clearInterval(si);
setTimeout(function(){location.reload(true);}, 10000);
}
});
}
}); */

$("#door-button").click(function(e) {
var comm = "controller?click=1";
clear_msg();
$.getJSON(comm, function(jd) {
if(jd.result!=1) {
show_msg("Check device key and try again.",2000,"red");
}
});
});

$("#status-refresh").click(function(){
refresh_status();
});
});


function refresh_status() {
$("#status-refresh i").addClass("loading");
$.getJSON("/json/controller", function(jd) {
$("#firmware_version").text("v"+(jd.firmware_version/100>>0)+"."+(jd.firmware_version/10%10>>0)+"."+(jd.firmware_version%10>>0));
$("#lbl_dist").text(""+jd.dist+" (cm)");
$("#lbl_status").text(jd.door?"OPEN":"closed").css("color", jd.door?"red":"black");
$("#lbl_beat").text(jd.read_count);
$("#head_name").text(jd.name);

$("#status-refresh i").removeClass("loading");
});
}

</script>

</div>




<div class="ui bottom attached tab" data-tab="tab-options">
<h3>Options</h3>

<div id="options_refresh" class="ui button">
<i class="refresh icon"></i> Refresh Options
</div>
<form id="options_form" action="/options" method="POST" class="ui form">

<h4 class="ui dividing header">Device Settings</h4>

<div class="fields">
<div class="three wide field">
<label>Device Name</label>
<input type="text" size=20 maxlength=32 id="name" name="name" value="-">
</div>


<div class="three wide field">
<label>HTTP Port</label>
<input type="text" size=5 maxlength=5 id="http_port" name="http_port" value=1>
</div>
</div>

<div class="fields">
<div class="three wide field">
<label>
<div class="ui checkbox">
<input type="checkbox" id="change_devicekey" name="change_devicekey">
<label>Change DeviceKey</label>
</div>
</label>
<input type="password" size=24 maxlength=32 id="new_devicekey" name="new_devicekey" disabled>
</div>

<div class="three wide field">
<label>Confirm</label>
<input type="password" size=24 maxlength=32 id="confirm_devicekey" name="confirm_devicekey" disabled>
</div>
</div>

<h4 class="ui dividing header">Sensor Settings</h4>

<div class="fields">
<div class="three wide field">
<label>Mount Type</label>
<select name="mount_type" id="mount_type" class="dropdown">
<option value=0>Ceiling Mount</option>
<option value=1>Side Mount</option>
</select>
</div>

<div class="two wide field">
<label>Threshold (cm)</label>
<input type="text" size=4 maxlength=4 id="dth" name="dth" value=1>
</div>

<div class="two wide field">
<label>Read Interval (s)</label>
<input type="text" size=3 maxlength=3 id="read_interval" name="read_interval" value=1>
</div>
</div>

<h4 class="ui dividing header">Firmware Update</h4>

<div class="six wide field">
<label>New Firmware File (.bin)</label>
<div class="ui action input">
<input type="text" readonly id="_file">
<input type="file" id="file" name="file" accept=".bin" style="display: none">
<label for="_file" class="ui icon button btn-file">
<i class="folder basic icon"></i>
</label>
</div>
</div>  

<h4 class="ui dividing header">Save Changes</h4>

<div id="options_submit" class="ui button primary">Submit Changes</div>
</form>
<script>
$(document).ready(function(){
// setup log refresh button and load initial data
$("#options_refresh").click(function(){
refresh_options();
});

// enable/disable the change key textboxes when the "change key"
// checkbox is changed
$("#change_devicekey").on("change", function(e){
thisStatus = $(this).is(":checked");
$("#new_devicekey").prop("disabled", !thisStatus)
$("#confirm_devicekey").prop("disabled", !thisStatus);
});

// setup the update input box
$("input:text, .ui.button", ".ui.action.input").on("click", function(e){
$("input:file", $(e.target).parents()).click();
});
$("input:file", ".ui.action.input").on("change", function(e){
var name = e.target.files[0].name;
$("input:text", $(e.target).parent()).val(name);
});

// intercept the form sumission
$("#options_submit").on("click", function(e){
$(this).addClass("loading");

if(confirm("Submit changes?")) {

var options_form = $("#options_form");
$(this).addClass("loading");

// send xhr request
$.ajax({
type: options_form.attr("method"),
url: options_form.attr("action"),
data: options_form.serialize(),
success: function(config) {
if(config.result!=1) {
if(config.result==2) show_msg("Check device key and try again.");
else show_msg("Error code: "+config.result+", item: "+config.item);
} else {
show_msg("Options are successfully saved. Note that changes to some options may require a reboot");
}

$("#options_submit").removeClass("loading");
}
});
}
});
});

function refresh_options() {
$("#options_form").addClass("loading");
$("#options_refresh i").addClass("loading");
$.getJSON("/json/options", function(jd) {
$("#firmware_version").text("v"+(jd.firmware_version/100>>0)+"."+(jd.firmware_version/10%10>>0)+"."+(jd.firmware_version%10>>0));
$("#mount_type").val(jd.mount_type);
$("#dth").val(jd.dth);
$("#read_interval").val(jd.read_interval);
$("#http_port").val(jd.http_port);
$("#name").val(jd.name);
$("#auth").val(jd.auth);
$("#options_form").removeClass("loading");
$("#options_refresh i").removeClass("loading");
});
}
</script>

</div>


<div class="ui bottom attached tab" data-tab="tab-logs">
<h3>Logs</h3>
<div id="logs-refresh" class="ui button">
<i class="refresh icon"></i> Refresh Logs
</div>
<table id="table-logs" class="ui table">
<thead>
<tr>
<th>Time Stamp</th>
<th>Status</th>
<th>D (cm)</th>
</tr>
</thead>
<tbody>
<tr>
<td colspan="3">No Logs Loaded</td>
</tr>
</tbody>
</table>
<script>
$(document).ready(function(){
// setup log refresh button and load initial data
$("#logs-refresh").click(function(){
refresh_logs();
});
});
function refresh_logs() {
$("#logs-refresh i").addClass("loading");

$.getJSON("/json/logs", function(response) {
// clear the table contents
$("#table-logs").find("tbody tr").remove();

var logs=response.logs;

// quickly sort the logs by timestamp
logs.sort(function(a,b){return b["tstamp"]-a["tstamp"];});

$("#logs-count").text(logs.length);

var ldate = new Date();

for(var i=0;i<logs.length;i++) {
ldate.setTime(logs[i]["tstamp"]*1000);
var r="<tr><td>"+ldate.toLocaleString()+"</td><td>"+(logs[i]["status"]?"OPEN":"closed")+"</td><td>"+logs[i]["dist"]+"</td></tr>";
$("#table-logs tbody").append(r);
}

$("#logs-refresh i").removeClass("loading");

});
}
</script>
</div>


<div class="ui bottom attached tab" data-tab="tab-update">
<h3>Update</h3>

<form method="POST" action="/update" id="fm" enctype="multipart/form-data" class="ui form">

<div class="fields">

<div class="five wide inline field">
<label>Enter Device Key:</label>
<input type="password" size=24 maxlength=32 id="devicekey">
</div>

<button class="ui button primary" type="submit">Update</button>

</div>

</form>

<script>
$(document).ready(function(){
$("#btn_submit").click(function(e){
var files= id("file").files;
if(files.length==0) {show_msg("Please select a file.",2000,"red"); return;}
if(id("devicekey").value=="") {
if(!confirm("You did not input a device key. Are you sure?")) return;
}
var btn = id("btn_submit");
show_msg("Uploading. Please wait...",0,"green");
var fd = new FormData();
var file = files[0];
fd.append("file", file, file.name);
fd.append("devicekey", id("devicekey").value);
var xhr = new XMLHttpRequest();
xhr.onreadystatechange = function() {
if(xhr.readyState==4 && xhr.status==200) {
var jd=JSON.parse(xhr.responseText);
if(jd.result==1) {
show_msg("Update is successful. Rebooting. Please wait...",0,"green");
setTimeout(close, 10000);
} else if (jd.result==2) {
show_msg("Check device key and try again.", 0, "red");
} else {
show_msg("Update failed.",0,"red");
}
}
};
xhr.open("POST", "update", true);
xhr.send(fd);
});
});
</script>

</div>

<div class="ui bottom attached tab" data-tab="tab-about">
<h3>About</h3>
</div>

</div>



<div id="login-modal" class="ui small modal">
<div class="header">
Authentication
</div>
<div class="image content">
<div class="image">
<i class="lock icon"></i>
</div>
<form id="auth_form" action="/auth" method="POST" class="ui form">
<div class="field">
<label>Enter Device Key:</label>
<input type="text" name="auth_devicekey" id="auth_devicekey" />
</div>
<div class="field">
<div id="auth_submit" class="ui green button right floated">
<i class="unlock icon"></i> Authenticate
</div>
</div>
</form>
</div>

<script>
$(document).ready(function(){

// intercept form submission
$("#auth_submit").on("click", function(){
$(this).addClass("loading");

auth_form = $("#auth_form");

// send xhr request
$.ajax({
type: auth_form.attr("method"),
url:  auth_form.attr("action"),
data: auth_form.serialize(),
success: function(auth) {
if(auth.result == "AUTH_SUCCESS"){
setCookie("OG_AUTH", auth.token, 0);

$("#auth_submit").removeClass("loading");
$("#login-modal").modal("hide");
} else {
$(this).removeClass("loading");
alert("Please try again");
}
}
});

// prevent submitting again
return false;
		 
});

if(!getCookie("OG_AUTH")){
$("#login-modal").modal("show");//"setting","closable",false).modal("show");
}

});
</script>

</div>


<script>
$(document).ready(function() {
$("select.dropdown").dropdown();
$(".checkbox").checkbox();

// setup tab behavior on top menu items
$(".ui.menu a.item").tab().on("click", function(){
$(this)
.addClass("active")
.siblings()
.removeClass("active");
});

});		
function clear_msg() {
$("#msg").text("");
}  

function show_msg(s) {
$("#msg").text(s).css("color","red");
setTimeout(clear_msg, 2000);
}	

function id(s) {return document.getElementById(s);}

function setCookie(key, value, expire_days=1) {
if(expire_days != 0){
var expires = new Date();
expires.setTime(expires.getTime() + (expire_days * 24 * 60 * 60 * 1000));
document.cookie = key + "=" + value + ";expires=" + expires.toUTCString();
} else {
document.cookie = key + "=" + value + ";";
}
}

function getCookie(key) {
var keyValue = document.cookie.match("(^|;) ?" + key + "=([^;]*)(;|$)");
return keyValue ? keyValue[2] : null;
}
</script>
</body>
</html>
)=====";
