<html>
 <head>
  <title>AQUALOOP</title>
  <link rel="stylesheet" href="Sty.c" type="text/css"/>
 </head>
 <body>
  <h1>Welcome to AQUALOOP!</h1>
  <p>This is the http server 'value_HOSTNAME' <img src="gr.gif" alt=":-)"></p>
  <p>We have now the basics ...
  <pre align="center">
       ___  ____  __  _____            __    __  ___          _ __          
      / _ |/ __ \/ / / / _ | ___  ___ / /_  /  |/  /__  ___  (_) /____  ____
     / __ / /_/ / /_/ / __ |/ _ \/ -_) __/ / /|_/ / _ \/ _ \/ / __/ _ \/ __/
    /_/ |_\___\_\____/_/ |_/_//_/\__/\__/ /_/  /_/\___/_//_/_/\__/\___/_/   

</pre>
  <p>For details on what AQUALOOP is, check <a href="http://www.intewa.de/products/aqualoop">www.intewa.de/aqualoop</a>.</p>

ifdef(`conf_MOTD_INLINE', `<p><a href="mo.ht">MOTD</a>: <b id="motd" style="border:1px dashed white"></b></p>
<script src="scr.js" type="text/javascript"></script>
<script>
function motd_get(request, data) {
	_(data, request.responseText);
}

window.onload = function() {
	ArrAjax.ecmd("motd", motd_get, "GET", "motd");
}
</script>
')
  <hr>
ifdef(`conf_VFS_IO_INLINE', `<p>To do some control tasks, see <a href="io.ht">here</a>.</p>')
ifdef(`conf_ADC_INLINE', `<p>To see the adc channels see <a href="adc.ht">here</a>.</p>')
ifdef(`conf_KTY_INLINE', `<p>To see the KTY temperature see <a href="Xky.ht">here</a>.</p>')
ifdef(`conf_ONEWIRE_INLINE', `<p>The onewire temperature values are accessible <a href="ow.ht">here</a>,
   or with a SVG-capable browser see the <a href="Xow.ht">graphical variant</a>.</p>')
ifdef(`conf_RFM12_INLINE', `<p>To control <a href="rf.ht">RFM12 ASK</a>.</p>')
ifdef(`conf_STELLA_INLINE', `<p>To control <a href="ste.ht">StellaLight</a>.</p>')
ifdef(`conf_MOTORCURTAIN_INLINE', `<p>To control <a href="cur.ht">MotorCurtain</a>.</p>')
ifdef(`conf_I2C_INLINE', `<p>Set <a href="i2c.ht">I<sup>2</sup>C-Bus</a> stuff.</p>')
ifdef(`conf_CAMERA_INLINE', `<p>See the <a href="cam.ht">camera examples</a>.</p>')
ifdef(`conf_NAMED_PIN_INLINE', `<p>Control <a href="np.ht">named outputs</a>.</p>')
ifdef(`conf_CONFIG_INLINE', `<p>Configure network <a href="cfg.ht">here</a></p>')
ifdef(`conf_NETSTAT', `<p>Visit <a target="_blank" href="http://www.ethersex.de/~habo/stat/">Ethersex online statistic</a></p>')
ifdef(`conf_MCUF_INLINE', `<p>Select MCUF Modul <a href="mm.ht">here</a></p>')
ifdef(`conf_PWM_SERVO_INLINE', `<p>Easy set <a href="ps.ht">Servo Position</a></p>')
ifdef(`conf_LCD_INLINE', `<p>Handle <a href="lcd.ht">LCD and other displays</a></p>')
ifdef(`conf_CW_INLINE', `<p>Send <a href="cw.ht">Morse code</a></p>')
ifdef(`conf_DMX_STORAGE_INLINE', `<p>Control <a href="dmx.ht">DMX Channels</a></p>')
ifdef(`conf_TANKLEVEL_INLINE', `<p>To query the tank level see <a href="tnk.ht">here</a>.</p>')
ifdef(`conf_WOL_INLINE', `<p>Send Wake on LAN signals <a href="wol.ht">here</a>.</p>')
 </body>
</html>
