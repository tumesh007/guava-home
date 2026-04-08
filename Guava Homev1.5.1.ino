/*
 * ============================================================================
 * PROJECT  : Guava Home — ESP32 Smart Home Controller
 * VERSION  : V1.5.1
 * BOARD    : ESP32 DevKit V1 (38-pin)
 * PARTITION: Default 4MB with spiffs
 * LICENSE  : MIT
 * SOURCE   : https://github.com/tumesh007/guava-home
 *
 * V1.5.1 CHANGES:
 * FEAT — Double-Clap software filter: two sharp peaks 200–700ms apart required
 * FEAT — cfg_clapDouble (bool) selectable via Web Config and Telegram commands
 * FEAT — cfg_clapMute (bool) disables clap sensor — toggle via Web Config/Telegram
 * CMD  — New Telegram/web commands: clap_single, clap_double, clap_mute, clap_unmute
 * FIX  — ISR debounce reduced 1500ms → 150ms to allow double-clap gap detection
 * FIX  — Bugfixes: Log Rollover, Machine Gun Buttons, Relay Mutes
 * ============================================================================
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

// ═══════════════════════════════════════════════════════════════════════
// PIN DEFINITIONS
// ═══════════════════════════════════════════════════════════════════════
#define PIN_DHT         4
#define PIN_TRIG       12
#define PIN_ECHO       13
#define PIN_BUZZER     14
#define PIN_RELAY1     15   // Fan
#define PIN_RELAY2     16   // Light
#define PIN_RFID_RST  255   // RST hardwired to 3.3V
#define PIN_RFID_SS     5
#define PIN_DOOR_IR    17   // IR obstacle sensor
#define PIN_RELAY3     25   // Door lock
#define PIN_BTN1       26   // Exit button
#define PIN_PIR        27
#define PIN_SOIL       32
#define PIN_RELAY4     33   // Water pump
#define PIN_LDR        34
#define PIN_BTN2       35   // LCD page button
#define PIN_MQ2        36   // MQ-2 gas
#define PIN_CLAP       39   // KY-037 sound sensor
#define PIN_LED         2   // Inbuilt LED

// ═══════════════════════════════════════════════════════════════════════
// FIXED TIMING CONSTANTS
// ═══════════════════════════════════════════════════════════════════════
#define SENSOR_FAST_MS    500UL
#define SENSOR_DHT_MS    2000UL
#define SENSOR_SOIL_MS   5000UL
#define LCD_UPDATE_MS     350UL
#define cfg_doorRelockMs 3000UL
#define AP_TIMEOUT_MS  600000UL
#define LCD_SLEEP_MS    30000UL
#define BTN_DEBOUNCE_MS   250UL
#define ALERT_COOL_MS   30000UL
#define PIR_WARMUP_MS   60000UL
#define LCD_SCROLL_DELAY  350UL
#define LCD_WAKE_DIST_CM   80
#define THRESH_DIST_CM     30

// ── CLAP SENSOR CONSTANTS (RESTORED) ──
#define CLAP_DEBOUNCE_MS      150UL  // Tight debounce to filter noise within rumble
#define DOUBLE_CLAP_MIN_MS    200UL  // Min gap for valid human double clap
#define DOUBLE_CLAP_MAX_MS    700UL  // Max gap for valid human double clap
#define DOUBLE_CLAP_TIMEOUT   800UL  // Reset state machine after this long
#define RELAY_SNAP_IGNORE_MS 1500UL  // Ignore mic immediately after relay fires

#define THRESH_LDR_DARK   1000
#define THRESH_SOIL_WET   1000
#define THRESH_GAS_WARN   2000

// ═══════════════════════════════════════════════════════════════════════
// HARDWARE INTERRUPT — KY-037 Clap Sensor
// ═══════════════════════════════════════════════════════════════════════
volatile uint8_t       clapPulses   = 0;
volatile unsigned long tLastClapISR = 0;

void IRAM_ATTR clapISR() {
  unsigned long now = millis();
  if (now - tLastClapISR < CLAP_DEBOUNCE_MS) return;
  tLastClapISR = now;
  clapPulses++;
}

// ═══════════════════════════════════════════════════════════════════════
// LCD PAGES
// ═══════════════════════════════════════════════════════════════════════
enum LCDPage { PAGE_MAIN = 0, PAGE_SENSORS = 1, PAGE_DEVICES = 2, PAGE_NETWORK = 3, PAGE_COUNT = 4 };
LCDPage lcdPage      = PAGE_MAIN;
int     lcdScrollPos = 0;

// ═══════════════════════════════════════════════════════════════════════
// OBJECTS
// ═══════════════════════════════════════════════════════════════════════
Preferences          prefs;
DHT                  dht(PIN_DHT, DHT11);
LiquidCrystal_I2C    lcd(0x27, 16, 2); 
MFRC522              rfid(PIN_RFID_SS, PIN_RFID_RST);
WiFiClientSecure     tlsClient;
UniversalTelegramBot* tgBot = nullptr;
WebServer            webServer(80);

// ═══════════════════════════════════════════════════════════════════════
// DYNAMIC CONFIG — loaded from flash, editable from web UI
// ═══════════════════════════════════════════════════════════════════════
String        cfg_ssid;
String        cfg_pass;
String        cfg_tgToken;
String        cfg_chatID;
String        cfg_otaPass     = "guavahome";

unsigned long cfg_lightDelay  = 30000UL;
unsigned long cfg_tgPollMs    = 8000UL;
unsigned long cfg_tgInterval  = 0;
unsigned long cfg_pumpOnTime  = 10;
unsigned long cfg_pumpOffTime = 50;
int           cfg_soilDryThresh = 3000;
float         cfg_tempFanOn   = 32.0f;
bool          cfg_irDoorInvert = false;

bool          cfg_clapDouble  = false;
bool          cfg_clapMute    = false;

String rfid_uid[3];
String rfid_name[3];
String lastScannedUID = "";

// ═══════════════════════════════════════════════════════════════════════
// STATE STRUCTURES
// ═══════════════════════════════════════════════════════════════════════
struct SensorState {
  float temp = 0.0f; float humidity = 0.0f;
  int ldr = 0; int soil = 0; int gas = 0;
  long dist = 999;
  bool pir = false; bool doorIR = false;
} sens;

struct DeviceState {
  bool fan = false; bool light = false; bool door = false;
  bool pump = false; bool mute = false;
} dev;

struct PumpCycle {
  bool enabled = true; bool phaseOn = false;
  unsigned long phaseStart = 0; unsigned long cycleCount = 0;
} pump;

bool          apMode       = false;
unsigned long apStart      = 0;
bool          otaActive    = false;
bool          pirWarmup    = true;
bool          pirLastState = false;
unsigned long tMotionEnd   = 0;
bool          lightByPIR   = false;
unsigned int  clapCount    = 0;

// BUTTON & CLAP STATE (RESTORED)
unsigned long tFirstClap   = 0;
unsigned long tLastRelayFire = 0;
bool          waitingForSecondClap = false;
bool          btn1LastState = HIGH;
bool          btn2LastState = LOW;

// TIMERS
unsigned long tFast = 0, tDHT = 0, tSoil = 0, tTG = 0, tTGStatus = 0;
unsigned long tLCD = 0, tDoor = 0, tLCDWake = 0, tScroll = 0, tBtn1 = 0, tBtn2 = 0;
unsigned long tPIRAlert = 0, tGasAlert = 0, tDistAlert = 0, tSoilAlert = 0;

// EVENT LOG
#define LOG_SIZE 10
String  evLog[LOG_SIZE];
unsigned long evIdx = 0; // BUGFIX: 32-bit to prevent rollover blanking
void logEvent(const String& s) { evLog[evIdx % LOG_SIZE] = s; evIdx++; Serial.println("[LOG] " + s); }

// ═══════════════════════════════════════════════════════════════════════
// RELAYS & BUZZER
// ═══════════════════════════════════════════════════════════════════════
void setFan(bool on)   { dev.fan   = on; digitalWrite(PIN_RELAY1, on ? LOW : HIGH); tLastRelayFire = millis(); }
void setLight(bool on) { dev.light = on; digitalWrite(PIN_RELAY2, on ? LOW : HIGH); tLastRelayFire = millis(); }
void setPump(bool on)  { dev.pump  = on; digitalWrite(PIN_RELAY4, on ? LOW : HIGH); tLastRelayFire = millis(); }

void unlockDoor() {
  dev.door = true; digitalWrite(PIN_RELAY3, LOW); tDoor = millis(); tLastRelayFire = millis();
  if (!dev.mute) { digitalWrite(PIN_BUZZER, HIGH); delay(100); digitalWrite(PIN_BUZZER, LOW); }
  logEvent("Door unlocked");
}
void lockDoor() { dev.door = false; digitalWrite(PIN_RELAY3, HIGH); logEvent("Door locked"); }

void beep(int count, int durMs = 100) {
  if (dev.mute) return;
  for (int i = 0; i < count; i++) {
    digitalWrite(PIN_BUZZER, HIGH); delay(durMs); digitalWrite(PIN_BUZZER, LOW);
    if (i < count - 1) delay(durMs);
  }
}

// ═══════════════════════════════════════════════════════════════════════
// SENSORS
// ═══════════════════════════════════════════════════════════════════════
long getDistance() {
  digitalWrite(PIN_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long us = pulseIn(PIN_ECHO, HIGH, 30000);
  return (us > 0) ? us / 58 : 999;
}

String getUID(MFRC522::Uid uid) {
  String s = "";
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) s += "0";
    s += String(uid.uidByte[i], HEX);
    if (i < uid.size - 1) s += " ";
  }
  s.toUpperCase(); return s;
}

String lookupRFID(const String& uid) {
  for (int i = 0; i < 3; i++) if (rfid_uid[i].length() > 0 && rfid_uid[i] == uid) return rfid_name[i];
  return "";
}

// ═══════════════════════════════════════════════════════════════════════
// TELEGRAM & LCD
// ═══════════════════════════════════════════════════════════════════════
void tgSend(const String& msg) {
  if (apMode || otaActive || WiFi.status() != WL_CONNECTED) return;
  if (!tgBot || cfg_chatID.length() < 3) return;
  tgBot->sendMessage(cfg_chatID, msg, "");
  Serial.println("[TG TX] " + msg);
}

void lcdScroll(const String& text, int row = 1) {
  unsigned long now = millis();
  if (now - tScroll < LCD_SCROLL_DELAY) return;
  tScroll = now;
  int len = text.length();
  if (len <= 16) {
    lcd.setCursor(0, row); lcd.print(text);
    for (int i = len; i < 16; i++) lcd.print(' ');
    lcdScrollPos = 0; return;
  }
  String padded = text + "    "; int padLen = padded.length(); String window = "";
  for (int i = 0; i < 16; i++) window += padded[(lcdScrollPos + i) % padLen];
  lcd.setCursor(0, row); lcd.print(window);
  lcdScrollPos = (lcdScrollPos + 1) % padLen;
}

void lcdShowPage(unsigned long now) {
  char line1[17], line2[17];
  switch (lcdPage) {
    case PAGE_MAIN:
      if (!isnan(sens.temp) && sens.temp > 0) snprintf(line1, 17, "T:%.1fC  H:%.0f%%   ", sens.temp, sens.humidity);
      else snprintf(line1, 17, "DHT starting... ");
      lcd.setCursor(0, 0); lcd.print(line1);
      {
        String st = "";
        st += sens.pir ? "PIR:MOTION " : "PIR:clear "; st += dev.light ? "LIGHT:ON " : "LIGHT:OFF ";
        st += dev.fan ? "FAN:ON " : "FAN:OFF "; st += (sens.gas > THRESH_GAS_WARN) ? "GAS:WARN! " : "GAS:OK ";
        lcdScroll(st, 1);
      }
      break;
    case PAGE_SENSORS:
      snprintf(line1, 17, "L:%-4d Soi:%-4d", sens.ldr, sens.soil);
      lcd.setCursor(0, 0); lcd.print(line1);
      snprintf(line2, 17, "Gas:%-4d D:%-3ld%s", sens.gas, sens.dist == 999 ? 0 : sens.dist, (sens.gas > THRESH_GAS_WARN) ? "!" : " ");
      lcd.setCursor(0, 1); lcd.print(line2);
      break;
    case PAGE_DEVICES:
      snprintf(line1, 17, "Fan:%-3s Lgt:%-3s ", dev.fan ? "ON" : "OFF", dev.light ? "ON" : "OFF");
      lcd.setCursor(0, 0); lcd.print(line1);
      snprintf(line2, 17, "Door:%-4s Pmp:%-3s", dev.door ? "OPEN" : "LOCK", dev.pump ? "ON" : "OFF"); lcd.setCursor(0, 1); lcd.print(line2);
      break;
    case PAGE_NETWORK:
      if (apMode) { lcd.setCursor(0, 0); lcd.print("AP: Guava_Setup ");
      } 
      else { lcd.setCursor(0, 0);
      lcd.print(WiFi.status() == WL_CONNECTED ? "WiFi:Connected  " : "WiFi:Lost       ");
      }
      lcdScroll(apMode ? "192.168.4.1 (setup)" : "IP:" + WiFi.localIP().toString(), 1);
      break;
    default: lcdPage = PAGE_MAIN;
      break;
  }
}

// ═══════════════════════════════════════════════════════════════════════
// BUILD JSON
// ═══════════════════════════════════════════════════════════════════════
String buildJSON() {
  String logStr = ""; int count = min((int)evIdx, LOG_SIZE);
  for (int i = 0; i < count; i++) {
    int idx = ((int)evIdx - 1 - i + LOG_SIZE) % LOG_SIZE;
    if (evLog[idx].length() > 0) logStr += evLog[idx] + "|";
  }
  StaticJsonDocument<512> doc;
  doc["t"]   = (!isnan(sens.temp) && sens.temp != 0) ? sens.temp : 0.0f;
  doc["h"]   = (!isnan(sens.humidity) && sens.humidity != 0) ? sens.humidity : 0.0f;
  doc["ldr"] = sens.ldr; doc["pir"] = sens.pir;
  doc["sol"] = sens.soil;
  doc["gas"] = sens.gas; doc["gw"]  = (sens.gas > THRESH_GAS_WARN); doc["dst"] = sens.dist;
  doc["clp"] = clapCount;
  doc["clapmd"] = cfg_clapDouble; doc["clapmt"] = cfg_clapMute;
  doc["f"] = dev.fan; doc["l"] = dev.light;
  doc["d"]   = dev.door; doc["dir"] = sens.doorIR;
  doc["p"] = dev.pump;
  doc["ap"]  = apMode; doc["ota"] = otaActive; doc["pg"] = (int)lcdPage;
  doc["luid"]= lastScannedUID; doc["log"] = logStr;
  String out;
  serializeJson(doc, out); return out;
}

// ═══════════════════════════════════════════════════════════════════════
// COMMAND ROUTER
// ═══════════════════════════════════════════════════════════════════════
void executeCmd(String cmd, int src = 1) {
  cmd.trim(); cmd.toLowerCase();
  cmd.replace("/", "");
  Serial.println("[CMD src=" + String(src) + "] " + cmd);
  if      (cmd == "fan_on")    { setFan(true);    logEvent("Fan ON");    tgSend("Fan ON"); }
  else if (cmd == "fan_off")   { setFan(false);   logEvent("Fan OFF");   tgSend("Fan OFF"); }
  else if (cmd == "light_on")  { setLight(true);  lightByPIR=false; logEvent("Light ON");  tgSend("Light ON"); }
  else if (cmd == "light_off") { setLight(false); lightByPIR=false; logEvent("Light OFF"); tgSend("Light OFF"); }
  else if (cmd == "unlock")    { unlockDoor();    tgSend("Door unlocked"); }
  else if (cmd == "pump_on")   { setPump(true);  pump.phaseOn=true;  pump.phaseStart=millis(); logEvent("Pump ON");  tgSend("Pump ON"); }
  else if (cmd == "pump_off")  { setPump(false); pump.phaseOn=false; pump.phaseStart=millis(); logEvent("Pump OFF"); tgSend("Pump OFF"); }
  else if (cmd == "pump_auto") { pump.enabled=true;  logEvent("Pump auto ON");  tgSend("Pump auto ON"); }
  else if (cmd == "pump_stop") { pump.enabled=false; setPump(false); logEvent("Pump auto OFF"); tgSend("Pump auto OFF"); }
  else if (cmd == "mute_on")   { dev.mute=true;  tgSend("Buzzer muted"); }
  else if (cmd == "mute_off")  { dev.mute=false; tgSend("Buzzer unmuted"); }
  
  else if (cmd == "clap_mute")   { cfg_clapMute=true;  prefs.putBool("clapmt",true);  tFirstClap=0; logEvent("Clap sensor muted");
  if(src==0) tgSend("Clap sensor MUTED\nLight will not toggle via clap until unmuted."); }
  else if (cmd == "clap_unmute") { cfg_clapMute=false;
  prefs.putBool("clapmt",false); logEvent("Clap sensor active"); if(src==0) tgSend("Clap sensor ACTIVE"); }
  else if (cmd == "clap_single") { cfg_clapDouble=false; prefs.putBool("clapmd",false); tFirstClap=0;
  logEvent("Clap: single mode"); if(src==0) tgSend("Clap mode: SINGLE\nOne clap toggles light."); }
  else if (cmd == "clap_double") { cfg_clapDouble=true;  prefs.putBool("clapmd",true);
  tFirstClap=0; logEvent("Clap: double mode"); if(src==0) tgSend("Clap mode: DOUBLE\nTwo claps (200–700ms apart) required."); }
  
  else if (cmd == "next_page") { lcdPage = (LCDPage)(((int)lcdPage + 1) % PAGE_COUNT); lcdScrollPos = 0; lcd.clear();
  logEvent("LCD page: " + String((int)lcdPage)); }
  else if (cmd == "status") {
    String dhtStr = (!isnan(sens.temp) && sens.temp > 0) ? String(sens.temp, 1) + "C / " + String(sens.humidity, 0) + "%" : "DHT not ready";
    String soilStr = String(sens.soil) + (sens.soil > cfg_soilDryThresh ? " DRY" : sens.soil < THRESH_SOIL_WET ? " WET" : " OK");
    String msg = "Guava Home V1.5.1\nTemp/Hum: " + dhtStr + "\nLDR:   " + String(sens.ldr) + (sens.ldr < THRESH_LDR_DARK ? " DARK" : " OK") + "\nSoil:  " + soilStr + " (thresh:" + String(cfg_soilDryThresh) + ")\nGas:   " + String(sens.gas) + (sens.gas > THRESH_GAS_WARN ? " WARN" : " OK") + "\nPIR:   " + String(sens.pir ? "MOTION" : "clear") + "\nDist:  " + String(sens.dist) + "cm\nClaps: " + String(clapCount) + " (mode:" + String(cfg_clapDouble?"DOUBLE":"SINGLE") + (cfg_clapMute?" MUTED":"") + ")\nFan:   " + String(dev.fan ? "ON" : "OFF") + " (Auto:" + String(cfg_tempFanOn, 1) + "C)\nLight: " + String(dev.light ? "ON" : "OFF") + "\nDoor:  " + String(dev.door ? "UNLOCKED" : "LOCKED") + " (IR:" + String(sens.doorIR ? "closed" : "open") + ")\nPump:  " + String(dev.pump ? "ON" : "OFF") + (pump.enabled ? " (auto)" : "") + "\nPump cycle: " + String(cfg_pumpOnTime) + "s ON / " + String(cfg_pumpOffTime) + "s OFF\nOTA: ready";
    if (src == 0) tgSend(msg); else Serial.println(msg);
  }
  else if (cmd == "help") {
    String h = "Guava Home commands:\nfan_on / fan_off\nlight_on / light_off\nunlock\npump_on / pump_off\npump_auto / pump_stop\nmute_on / mute_off\nclap_single / clap_double\nclap_mute / clap_unmute\nnext_page\nstatus / help / reboot";
    if (src == 0) tgSend(h); else Serial.println(h);
  }
  else if (cmd == "reboot") { tgSend("Rebooting..."); delay(500); ESP.restart(); }
  else { Serial.println("[CMD] Unknown: " + cmd); }
}

// ═══════════════════════════════════════════════════════════════════════
// WEB UI
// ═══════════════════════════════════════════════════════════════════════
const char INDEX_HTML[] PROGMEM = R"rawhtml(<!DOCTYPE html>
<html lang='en'><head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'>
<meta name='theme-color' content='#0d1117'>
<title>Guava Home</title>
<link rel='manifest' href='/manifest.json'>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,Arial,sans-serif;background:#0d1117;color:#e6edf3}
.hdr{background:#161b22;border-bottom:1px solid #30363d;padding:12px 15px;display:flex;justify-content:space-between;align-items:center;position:sticky;top:0;z-index:99}
.hdr h1{font-size:15px;color:#58a6ff;display:flex;align-items:center;gap:8px}
.dot{width:9px;height:9px;border-radius:50%;background:#30363d;flex-shrink:0}
.dot.ok{background:#2ea043}.dot.ap{background:#f0883e}
.dot.ota{background:#f0883e;animation:pulse 0.6s infinite}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.3}}
.nav{display:flex;gap:7px}
.nb{background:#21262d;border:1px solid #30363d;color:#c9d1d9;padding:5px 12px;border-radius:6px;cursor:pointer;font-size:13px}
.nb.a{background:#1f6feb;border-color:#1f6feb;color:#fff}
.tab{display:none;padding:14px}.tab.a{display:block}
.g2{display:grid;grid-template-columns:1fr 1fr;gap:9px;margin-bottom:11px}
.g3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:9px;margin-bottom:11px}
.c{background:#161b22;border:1px solid #30363d;border-radius:8px;padding:11px 9px;text-align:center}
.v{font-size:19px;font-weight:bold;margin-top:3px}
.u{font-size:10px;color:#8b949e}
.ok{color:#2ea043}.warn{color:#f0883e}.on{color:#58a6ff}.off{color:#f85149}
.btn{width:100%;padding:10px;background:#21262d;border:1px solid #30363d;color:#fff;border-radius:8px;margin-bottom:8px;cursor:pointer;font-size:14px;font-weight:500}
.btn.b{background:#1f6feb;border-color:#1f6feb}
.btn.r{background:#da3633;border-color:#da3633}
.btn.g{background:#2ea043;border-color:#2ea043}
.row{display:flex;gap:7px;margin-bottom:8px}.row .btn{margin-bottom:0}
.pg{display:flex;gap:7px;margin-bottom:12px;align-items:center}
.pg span{font-size:12px;color:#8b949e}
.inp{width:100%;padding:9px;background:#0d1117;border:1px solid #30363d;color:#e6edf3;border-radius:6px;margin-top:4px;margin-bottom:12px;font-size:14px}
.inp.uid{font-family:monospace;letter-spacing:1px;background:#0a1628}
.lbl{font-size:12px;color:#8b949e;display:block;margin-bottom:2px}
.log{background:#010409;color:#2ea043;padding:9px;font-family:monospace;font-size:11px;border-radius:6px;height:130px;overflow-y:auto;border:1px solid #21262d;white-space:pre-wrap}
h3{font-size:14px;color:#f0f6fc;margin-bottom:10px}
h4{font-size:13px;color:#58a6ff;margin:14px 0 8px}
.sep{border:0;border-top:1px solid #30363d;margin:14px 0}
.uid-row{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.luid{background:#0a1628;border:1px solid #30363d;border-radius:6px;padding:9px;font-family:monospace;font-size:13px;color:#fde68a;letter-spacing:1px;margin-bottom:12px;min-height:36px;cursor:pointer;word-break:break-all}
.soil-ok{color:#2ea043}.soil-dry{color:#f0883e}.soil-wet{color:#58a6ff}
.gas-ok{color:#2ea043}.gas-warn{color:#f85149}
.ota-box{background:#1a1a0a;border:1px solid #f0883e;border-radius:8px;padding:10px;margin-bottom:10px;font-size:13px;color:#fde68a;display:none}
.hint{font-size:11px;color:#64748b;margin-top:-10px;margin-bottom:12px}
.door-locked{color:#2ea043}.door-open{color:#f0883e}.door-unlocked{color:#58a6ff}
.c-row{display:flex;align-items:center;margin-bottom:8px;font-size:13px;color:#c9d1d9;cursor:pointer;gap:10px}
.c-row input[type=checkbox]{width:16px;height:16px;accent-color:#1f6feb;cursor:pointer;flex-shrink:0}
</style>
</head><body>
<div class='hdr'>
  <h1>&#127815; Guava Home <div class='dot' id='wd'></div></h1>
  <div class='nav'>
    <button class='nb a' id='t1' onclick='sw(1)'>Dash</button>
    <button class='nb' id='t2' onclick='sw(2)'>Config</button>
  </div>
</div>

<div id='tab1' class='tab a'>
  <div class='ota-box' id='otaBox'>&#9889; OTA Update in progress — do not power off</div>
  <div class='g2'>
    <div class='c'><div class='u'>Temp / Humidity</div><div class='v'><span id='tV'>--</span>&deg;C <span id='hV'>--</span>%</div></div>
    <div class='c'><div class='u'>Distance / Motion</div><div class='v'><span id='dV'>--</span>cm <span id='mvV' class='warn'></span></div></div>
  </div>
  <div class='g3'>
    <div class='c'><div class='u'>Light Level</div><div class='v' id='ldrV'>--</div></div>
    <div class='c'><div class='u'>Soil Moisture</div><div class='v' id='solV'>--</div></div>
    <div class='c'><div class='u'>Gas (MQ-2)</div><div class='v' id='gasV'>--</div></div>
  </div>
  <div class='g2'>
    <div class='c'><div class='u'>Fan</div><div class='v' id='fanV'>--</div></div>
    <div class='c'><div class='u'>Light</div><div class='v' id='lgtV'>--</div></div>
  </div>
  <div class='g2'>
    <div class='c'><div class='u'>Pump</div><div class='v' id='pumV'>--</div></div>
    <div class='c'><div class='u'>Door</div><div class='v' id='doorV'>--</div></div>
  </div>
  <div class='pg'>
    <span>LCD Page:</span>
    <button class='nb' onclick="cmd('next_page')">&#8594; Next</button>
    <span id='pgV'></span>
  </div>
  <button class='btn g' onclick="cmd('unlock')">&#128275; Unlock Door (3s)</button>
  <div class='row'>
    <button class='btn' onclick="cmd('fan_on')">Fan ON</button>
    <button class='btn r' onclick="cmd('fan_off')">Fan OFF</button>
  </div>
  <div class='row'>
    <button class='btn' onclick="cmd('light_on')">Light ON</button>
    <button class='btn r' onclick="cmd('light_off')">Light OFF</button>
  </div>
  <div class='row'>
    <button class='btn b' onclick="cmd('pump_auto')">Pump Auto</button>
    <button class='btn r' onclick="cmd('pump_stop')">Pump Stop</button>
  </div>
  <div class='lbl' style='margin-bottom:5px'>Event Log</div>
  <div class='log' id='logV'>Loading...</div>
</div>

<div id='tab2' class='tab'>
  <h3>System Configuration</h3>
  <p class='lbl' style='margin-bottom:14px'>All settings saved to flash. Reboot to apply WiFi / Telegram / OTA changes.</p>

  <h4>&#128268; WiFi &amp; Telegram</h4>
  <label class='lbl'>WiFi SSID</label>
  <input class='inp' type='text' id='cS' placeholder='Your WiFi name'>
  <label class='lbl'>WiFi Password</label>
  <input class='inp' type='password' id='cP' placeholder='WiFi password'>
  <label class='lbl'>Telegram Bot Token</label>
  <input class='inp' type='text' id='cT' placeholder='1234567890:AAF...'>
  <label class='lbl'>Telegram Chat ID</label>
  <input class='inp' type='text' id='cC' placeholder='Your numeric chat ID'>

  <hr class='sep'>
  <h4>&#9889; OTA Firmware Update</h4>
  <label class='lbl'>OTA Password</label>
  <input class='inp' type='password' id='cO' placeholder='default: guavahome'>
  <p class='hint'>Arduino IDE: Tools → Port → Network → GuavaHome &nbsp;|&nbsp; <a href='/update' style='color:#58a6ff'>Browser upload (/update)</a></p>

  <hr class='sep'>
  <h4>&#128242; Telegram Automation</h4>
  <label class='lbl'>Auto-Status Interval (minutes, 0 = disabled)</label>
  <input class='inp' type='number' id='cTGI' placeholder='0' min='0' max='1440'>
  <p class='hint'>Automatically sends /status to Telegram at this interval. Set 0 to disable.</p>
  <label class='lbl'>Telegram Polling Interval (seconds)</label>
  <input class='inp' type='number' id='cTGP' placeholder='8' min='2' max='60'>
  <p class='hint'>How often to check Telegram for new commands. Lower = more responsive but higher CPU load.</p>

  <hr class='sep'>
  <h4>&#128268; RFID &amp; Door</h4>
  <label class='lbl'>Last Scanned UID (tap to copy)</label>
  <div class='luid' id='luidV' onclick='copyUID()' title='Tap to copy'>--</div>
  <label class='lbl'>Authorised User 1</label>
  <div class='uid-row'>
    <input class='inp uid' type='text' id='u1uid' placeholder='UID e.g. AB CD EF 12'>
    <input class='inp' type='text' id='u1name' placeholder='Name'>
  </div>
  <label class='lbl'>Authorised User 2</label>
  <div class='uid-row'>
    <input class='inp uid' type='text' id='u2uid' placeholder='UID'>
    <input class='inp' type='text' id='u2name' placeholder='Name'>
  </div>
  <label class='lbl'>Authorised User 3</label>
  <div class='uid-row'>
    <input class='inp uid' type='text' id='u3uid' placeholder='UID'>
    <input class='inp' type='text' id='u3name' placeholder='Name'>
  </div>
  <label class='c-row' style='margin-top:8px'>
    <input type='checkbox' id='cIRInv'>
    IR sensor inverted (tick if door logic is reversed — locks when open, stays open when closed)
  </label>
   <p class='hint'>Most IR obstacle modules: LOW = object detected (door closed). Tick this only if your sensor behaves the opposite way.</p>

  <hr class='sep'>
  <h4>&#127777; Climate Control & Lighting</h4>
  <label class='lbl'>Fan Auto-ON Temperature (&deg;C, 10-80)</label>
  <input class='inp' type='number' step='0.1' id='cTFON' placeholder='32.0' min='10' max='80'>
  <label class='lbl'>Auto-off delay after motion stops (seconds, 5–600)</label>
  <input class='inp' type='number' id='cLD' placeholder='30' min='5' max='600'>

  <hr class='sep'>
  <h4>&#128079; Clap Sensor</h4>
  <label class='lbl'>Clap Mode</label>
  <div style='display:flex;gap:14px;margin-bottom:12px'>
    <label style='display:flex;align-items:center;gap:8px;font-size:13px;color:#c9d1d9;cursor:pointer'>
      <input type='radio' name='clapMode' id='clapSingle' value='0' style='accent-color:#1f6feb'> Single clap
    </label>
    <label style='display:flex;align-items:center;gap:8px;font-size:13px;color:#c9d1d9;cursor:pointer'>
      <input type='radio' name='clapMode' id='clapDouble' value='1' style='accent-color:#1f6feb'> Double clap (storm-safe)
    </label>
  </div>
  <p class='hint' style='margin-top:-8px'>Double clap: two sharp peaks 200–700ms apart required. Rejects thunder (single massive peak or sustained rumble).</p>
  <label class='c-row'>
    <input type='checkbox' id='cClapMute'>
    Mute clap sensor (disable during storms / night)
  </label>
  <p class='hint'>Clap sensor completely ignored when muted. Toggle via Telegram: clap_mute / clap_unmute</p>

  <hr class='sep'>
  <h4>&#127793; Pump &amp; Garden</h4>
  <label class='lbl'>Pump ON Duration (seconds)</label>
  <input class='inp' type='number' id='cPON' placeholder='10' min='1' max='3600'>
  <label class='lbl'>Pump OFF Duration (seconds)</label>
  <input class='inp' type='number' id='cPOFF' placeholder='50' min='1' max='86400'>
  <label class='lbl'>Soil Dry Threshold (ADC value, 1000–4095)</label>
  <input class='inp' type='number' id='cSDT' placeholder='3000' min='1000' max='4095'>
  <p class='hint'>Higher = triggers alert at drier soil. Lower for plants needing more water. Current: <span id='curSDT'>3000</span></p>

  <button class='btn r' onclick='saveCfg()' style='margin-top:4px'>&#128190; Save Settings &amp; Reboot</button>
</div>

<script>
const PAGES=['Main','Sensors','Devices','Network'];
function sw(n){
  for(let i=1;i<=2;i++){
    document.getElementById('tab'+i).className='tab'+(i==n?' a':'');
    document.getElementById('t'+i).className='nb'+(i==n?' a':'');
  }
}

async function cmd(c){
  try{await fetch('/api/cmd?c='+encodeURIComponent(c));}catch(e){}
}

function copyUID(){
  const t=document.getElementById('luidV').textContent;
  if(t==='--')return;
  navigator.clipboard.writeText(t).then(()=>{
    const el=document.getElementById('luidV');
    el.style.background='#1a3a1a';
    setTimeout(()=>el.style.background='',700);
  });
}

async function saveCfg(){
  const data=new URLSearchParams({
    s:   document.getElementById('cS').value,
    p:   document.getElementById('cP').value,
    t:   document.getElementById('cT').value,
    c:   document.getElementById('cC').value,
    o:   document.getElementById('cO').value,
    u1u: document.getElementById('u1uid').value.toUpperCase().trim(),
    u1n: document.getElementById('u1name').value.trim(),
    u2u: document.getElementById('u2uid').value.toUpperCase().trim(),
    u2n: document.getElementById('u2name').value.trim(),
    u3u: document.getElementById('u3uid').value.toUpperCase().trim(),
    u3n: document.getElementById('u3name').value.trim(),
    ld:  document.getElementById('cLD').value,
    tgi: document.getElementById('cTGI').value,
    tgp: document.getElementById('cTGP').value,
    pon: document.getElementById('cPON').value,
    poff:document.getElementById('cPOFF').value,
    sdt: document.getElementById('cSDT').value,
    tfon:document.getElementById('cTFON').value,
    irdinv: document.getElementById('cIRInv').checked ? 1 : 0,
    clapmd: document.querySelector('input[name=clapMode]:checked')?.value || 0,
    clapmt: document.getElementById('cClapMute').checked ? 1 : 0
  });
  await fetch('/api/save',{method:'POST',body:data});
  alert('Saved! Rebooting...');
}

let cfgDone=false;
let dryThresh=3000;

function soilText(v){
  if(v>dryThresh) return v+' DRY';
  if(v<1000) return v+' WET';
  return v+' OK';
}
function soilCls(v){
  if(v>dryThresh) return 'v soil-dry';
  if(v<1000) return 'v soil-wet';
  return 'v soil-ok';
}

async function poll(){
  try{
    const r=await fetch('/api/state');
    const d=await r.json();

    if(d.sdt!==undefined){ dryThresh=d.sdt; document.getElementById('curSDT').textContent=d.sdt; }

    const wd=document.getElementById('wd');
    wd.className=d.ota?'dot ota':d.ap?'dot ap':'dot ok';
    document.getElementById('otaBox').style.display=d.ota?'block':'none';

    document.getElementById('tV').textContent=parseFloat(d.t||0).toFixed(1);
    document.getElementById('hV').textContent=Math.round(d.h||0);
    document.getElementById('dV').textContent=d.dst==999?'--':d.dst;
    document.getElementById('mvV').textContent=d.pir?'MOTION':'';

    const ldrEl=document.getElementById('ldrV');
    ldrEl.textContent=d.ldr<1000?'DARK':'OK';
    ldrEl.className='v '+(d.ldr<1000?'warn':'ok');
    
    const solEl=document.getElementById('solV');
    solEl.textContent=soilText(d.sol); solEl.className=soilCls(d.sol);

    const gasEl=document.getElementById('gasV');
    gasEl.textContent=d.gw?d.gas+' WARN':d.gas+' OK';
    gasEl.className='v '+(d.gw?'gas-warn':'gas-ok');

    document.getElementById('fanV').textContent=d.f?'ON':'OFF';
    document.getElementById('fanV').className='v '+(d.f?'on':'off');
    document.getElementById('lgtV').textContent=d.l?'ON':'OFF';
    document.getElementById('lgtV').className='v '+(d.l?'on':'off');
    document.getElementById('pumV').textContent=d.p?'ON':'OFF';
    document.getElementById('pumV').className='v '+(d.p?'on':'off');
    
    const doorEl=document.getElementById('doorV');
    if(d.d){
      doorEl.textContent=d.dir?'CLOSED':'OPEN';
      doorEl.className='v '+(d.dir?'door-unlocked':'door-open');
    } else {
      doorEl.textContent='LOCKED';
      doorEl.className='v door-locked';
    }

    document.getElementById('pgV').textContent=PAGES[d.pg||0];
    if(d.luid&&d.luid!=='') document.getElementById('luidV').textContent=d.luid;
    
    if(d.log){
      const lines=d.log.split('|').filter(x=>x.length).reverse();
      document.getElementById('logV').textContent=lines.join('\n');
    }

    if(!cfgDone&&d.tok!==undefined){
      cfgDone=true;
      document.getElementById('cT').value=d.tok||'';
      document.getElementById('cC').value=d.cid||'';
      document.getElementById('u1uid').value=d.u1u||'';
      document.getElementById('u1name').value=d.u1n||'';
      document.getElementById('u2uid').value=d.u2u||'';
      document.getElementById('u2name').value=d.u2n||'';
      document.getElementById('u3uid').value=d.u3u||'';
      document.getElementById('u3name').value=d.u3n||'';
      document.getElementById('cLD').value=d.ld||30;
      document.getElementById('cTGI').value=d.tgi||0;
      document.getElementById('cTGP').value=d.tgp||8;
      document.getElementById('cPON').value=d.pon||10;
      document.getElementById('cPOFF').value=d.poff||50;
      document.getElementById('cSDT').value=d.sdt||3000;
      if(d.tfon!==undefined) document.getElementById('cTFON').value=d.tfon;
      if(d.irdinv!==undefined) document.getElementById('cIRInv').checked=d.irdinv;
      if(d.clapmd!==undefined){ document.getElementById(d.clapmd?'clapDouble':'clapSingle').checked=true; }
      if(d.clapmt!==undefined) document.getElementById('cClapMute').checked=d.clapmt;
    }
  }catch(e){}
}
poll(); setInterval(poll,3000);
</script>
</body></html>)rawhtml";

const char MANIFEST_JSON[] PROGMEM = R"raw({
  "name":"Guava Home",
  "short_name":"GuavaHome",
  "start_url":"/",
  "display":"standalone",
  "background_color":"#0d1117",
  "theme_color":"#0d1117",
  "icons":[{"src":"/icon.png","sizes":"192x192","type":"image/png"}]
})raw";

const char OTA_HTML[] PROGMEM = R"raw(<!DOCTYPE html>
<html><head><meta charset='UTF-8'><title>OTA Update — Guava Home</title>
<style>body{font-family:Arial,sans-serif;background:#0d1117;color:#e6edf3;padding:30px}
h2{color:#fde68a}input[type=file]{margin:12px 0;display:block}
button{background:#1f6feb;color:#fff;border:0;padding:10px 22px;border-radius:6px;cursor:pointer}</style>
</head><body>
<h2>&#127815; Guava Home — OTA Firmware Update</h2>
<p>Select compiled .bin file and click Upload.</p>
<form method='POST' action='/ota_upload' enctype='multipart/form-data'>
  <input type='file' name='firmware' accept='.bin'>
  <button type='submit'>Upload Firmware</button>
</form>
</body></html>)raw";

// ═══════════════════════════════════════════════════════════════════════
// WEB HANDLERS
// ═══════════════════════════════════════════════════════════════════════
void handleRoot()     { webServer.send_P(200, "text/html", INDEX_HTML); }
void handleManifest() { webServer.send_P(200, "application/manifest+json", MANIFEST_JSON); }
void handleOTAPage()  { webServer.send_P(200, "text/html", OTA_HTML); }

void handleApiState() {
  String j = buildJSON();
  j.remove(j.length() - 1);
  j += ",\"tok\":\""  + cfg_tgToken + "\""
    +  ",\"cid\":\""  + cfg_chatID  + "\""
    +  ",\"u1u\":\""  + rfid_uid[0] + "\",\"u1n\":\"" + rfid_name[0] + "\""
    +  ",\"u2u\":\""  + rfid_uid[1] + "\",\"u2n\":\"" + rfid_name[1] + "\""
    +  ",\"u3u\":\""  + rfid_uid[2] + "\",\"u3n\":\"" + rfid_name[2] + "\""
    +  ",\"ld\":"     + String(cfg_lightDelay / 1000)
    +  ",\"tgi\":"    + String(cfg_tgInterval)
    +  ",\"tgp\":"    + String(cfg_tgPollMs / 1000)
    +  ",\"pon\":"    + String(cfg_pumpOnTime)
    +  ",\"poff\":"   + String(cfg_pumpOffTime)
    +  ",\"sdt\":"    + String(cfg_soilDryThresh)
    +  ",\"tfon\":"   + String(cfg_tempFanOn, 1)
    +  ",\"irdinv\":" + String(cfg_irDoorInvert ? "true" : "false")
    +  ",\"clapmd\":" + String(cfg_clapDouble   ? "true" : "false")
    +  ",\"clapmt\":" + String(cfg_clapMute     ? "true" : "false")
    +  "}";
  webServer.send(200, "application/json", j);
}

void handleApiCmd() {
  if (webServer.hasArg("c")) executeCmd(webServer.arg("c"), 1);
  webServer.send(200, "text/plain", "ok");
}

void handleApiSave() {
  if (webServer.hasArg("s") && webServer.arg("s").length() > 0) { prefs.putString("ssid", webServer.arg("s")); prefs.putString("pass", webServer.arg("p")); }
  if (webServer.hasArg("t") && webServer.arg("t").length() > 5) prefs.putString("tok",  webServer.arg("t"));
  if (webServer.hasArg("c") && webServer.arg("c").length() > 3) prefs.putString("chat", webServer.arg("c"));
  if (webServer.hasArg("o") && webServer.arg("o").length() >= 4) { cfg_otaPass = webServer.arg("o"); prefs.putString("otapw", cfg_otaPass); }
  
  for (int i = 0; i < 3; i++) {
    String ku = "u" + String(i+1) + "u";
    String kn = "u" + String(i+1) + "n";
    if (webServer.hasArg(ku)) { rfid_uid[i] = webServer.arg(ku); rfid_name[i] = webServer.arg(kn); prefs.putString(ku.c_str(), rfid_uid[i]); prefs.putString(kn.c_str(), rfid_name[i]); }
  }
  if (webServer.hasArg("ld")) { int s = webServer.arg("ld").toInt(); if (s >= 5 && s <= 600) { cfg_lightDelay = (unsigned long)s * 1000UL; prefs.putULong("ldly", cfg_lightDelay); } }
  if (webServer.hasArg("tgi")) { int v = webServer.arg("tgi").toInt(); cfg_tgInterval = max(0, v); prefs.putULong("tgi", cfg_tgInterval); }
  if (webServer.hasArg("tgp")) { int v = webServer.arg("tgp").toInt(); if (v >= 2 && v <= 60) { cfg_tgPollMs = (unsigned long)v * 1000UL; prefs.putULong("tgpms", cfg_tgPollMs); } }
  if (webServer.hasArg("pon"))  { int v=webServer.arg("pon").toInt();  if(v>=1)  { cfg_pumpOnTime=v;  prefs.putULong("pon",v); } }
  if (webServer.hasArg("poff")) { int v=webServer.arg("poff").toInt(); if(v>=1)  { cfg_pumpOffTime=v; prefs.putULong("poff",v); } }
  if (webServer.hasArg("sdt"))  { int v=webServer.arg("sdt").toInt();  if(v>=1000&&v<=4095) { cfg_soilDryThresh=v; prefs.putInt("sdt",v); } }
  if (webServer.hasArg("tfon")) { float v = webServer.arg("tfon").toFloat(); if (v >= 10.0 && v <= 80.0) { cfg_tempFanOn = v; prefs.putFloat("tfon", v); } }
  if (webServer.hasArg("irdinv")) { cfg_irDoorInvert = (webServer.arg("irdinv").toInt() == 1); prefs.putBool("irdinv", cfg_irDoorInvert); }
  if (webServer.hasArg("clapmd")) { cfg_clapDouble = (webServer.arg("clapmd").toInt() == 1); prefs.putBool("clapmd", cfg_clapDouble); tFirstClap = 0; }
  if (webServer.hasArg("clapmt")) { cfg_clapMute   = (webServer.arg("clapmt").toInt() == 1); prefs.putBool("clapmt", cfg_clapMute);   tFirstClap = 0; }

  webServer.send(200, "text/plain", "OK");
  logEvent("Config saved");
  delay(800); ESP.restart();
}

void handleOTAUpload() {
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    otaActive = true; Serial.println("[OTA] Browser upload: " + upload.filename);
    lcd.clear(); lcd.print("OTA Uploading...");
    lcd.setCursor(0,1); lcd.print("Do not power off");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { Serial.print("[OTA] begin() failed: "); Update.printError(Serial); }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) { Serial.print("[OTA] write() error: "); Update.printError(Serial); }
    char buf[17]; snprintf(buf, 17, "%6u bytes    ", upload.totalSize); lcd.setCursor(0, 1); lcd.print(buf);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { Serial.printf("[OTA] Upload done: %u bytes\n", upload.totalSize); } 
    else { Serial.print("[OTA] end() failed: "); Update.printError(Serial); }
  }
}

// ═══════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200); Serial.println("\n=== Guava Home V1.5.1 Booting ===");

  digitalWrite(PIN_RELAY1, HIGH); digitalWrite(PIN_RELAY2, HIGH);
  digitalWrite(PIN_RELAY3, HIGH); digitalWrite(PIN_RELAY4, HIGH);
  digitalWrite(PIN_BUZZER, LOW);  digitalWrite(PIN_LED,    LOW);
  digitalWrite(PIN_TRIG,   LOW);

  pinMode(PIN_RELAY1, OUTPUT); pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_RELAY3, OUTPUT); pinMode(PIN_RELAY4, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT); pinMode(PIN_LED,    OUTPUT);
  pinMode(PIN_TRIG,   OUTPUT); pinMode(PIN_ECHO,   INPUT);
  pinMode(PIN_PIR,    INPUT);  pinMode(PIN_BTN1,   INPUT_PULLUP);
  pinMode(PIN_BTN2,   INPUT);  pinMode(PIN_MQ2,    INPUT);
  pinMode(PIN_DOOR_IR, INPUT_PULLUP);
  pinMode(PIN_CLAP, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLAP), clapISR, FALLING);

  Wire.begin(21, 22);
  lcd.init(); lcd.backlight(); lcd.clear();
  lcd.print("Guava Home"); lcd.setCursor(0,1); lcd.print("V1.5.1 Booting...");

  dht.begin(); SPI.begin(); rfid.PCD_Init();
  Serial.println("[HW] LCD, DHT11, RFID, KY-037 ready");

  // ── LOAD CONFIG FROM FLASH ─────────────────────────────────────────────
  prefs.begin("gh", false);
  cfg_ssid         = prefs.getString("ssid",  "");
  cfg_pass         = prefs.getString("pass",  "");
  cfg_tgToken      = prefs.getString("tok",   "");
  cfg_chatID       = prefs.getString("chat",  "");
  cfg_otaPass      = prefs.getString("otapw", "guavahome");
  cfg_lightDelay   = prefs.getULong("ldly",   30000UL);
  cfg_tgInterval   = prefs.getULong("tgi",    0);
  cfg_tgPollMs     = prefs.getULong("tgpms",  8000UL);
  cfg_pumpOnTime   = prefs.getULong("pon",    10);
  cfg_pumpOffTime  = prefs.getULong("poff",   50);
  cfg_soilDryThresh= prefs.getInt("sdt",      3000);
  cfg_tempFanOn    = prefs.getFloat("tfon",   32.0f);
  cfg_irDoorInvert = prefs.getBool("irdinv",  false);
  cfg_clapDouble   = prefs.getBool("clapmd",  false);
  cfg_clapMute     = prefs.getBool("clapmt",  false);
  
  for (int i = 0; i < 3; i++) {
    rfid_uid[i]  = prefs.getString(("u" + String(i+1) + "u").c_str(), "");
    rfid_name[i] = prefs.getString(("u" + String(i+1) + "n").c_str(), "");
  }
  Serial.println("[CFG] SSID: " + (cfg_ssid.length() > 0 ? cfg_ssid : "not set"));
  pump.phaseStart = millis();

  // ── WIFI ───────────────────────────────────────────────────────────────
  WiFi.mode(WIFI_STA); WiFi.setSleep(false);
  if (cfg_ssid.length() > 0) {
    Serial.print("[WiFi] Connecting: " + cfg_ssid);
    WiFi.begin(cfg_ssid.c_str(), cfg_pass.c_str());
    int att = 0;
    while (WiFi.status() != WL_CONNECTED && att < 30) { delay(500); Serial.print("."); att++; }
    Serial.println();
  }

  if (WiFi.status() == WL_CONNECTED) {
    String ip = WiFi.localIP().toString();
    Serial.println("[WiFi] Connected. IP: " + ip);
    configTime(19800, 0, "pool.ntp.org");
    tlsClient.setInsecure(); tlsClient.setTimeout(5);
    tgBot = new UniversalTelegramBot(cfg_tgToken, tlsClient);
    
    ArduinoOTA.setHostname("GuavaHome"); ArduinoOTA.setPassword(cfg_otaPass.c_str());
    ArduinoOTA.onStart([]() { otaActive = true; lcd.clear(); lcd.print("OTA Updating... "); lcd.setCursor(0,1); lcd.print("Do not power off"); });
    ArduinoOTA.onEnd([]() { lcd.clear(); lcd.print("OTA Done!       "); lcd.setCursor(0,1); lcd.print("Rebooting...    "); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { int pct = progress / (total / 100); char buf[17]; snprintf(buf, 17, "Progress: %3d%%  ", pct); lcd.setCursor(0, 1); lcd.print(buf); });
    ArduinoOTA.onError([](ota_error_t error) { otaActive = false; lcd.clear(); lcd.print("OTA FAILED      "); });
    ArduinoOTA.begin();

    lcd.clear(); lcd.print("WiFi OK");
    lcd.setCursor(0,1); lcd.print(ip.c_str()); delay(2000);
    tgSend("Guava Home V1.5.1 Online\nDashboard: http://" + ip);
    tTGStatus = millis();
  } else {
    apMode = true; apStart = millis();
    WiFi.mode(WIFI_AP); WiFi.softAP("Guava_Setup");
    lcd.clear(); lcd.print("Setup: Guava"); lcd.setCursor(0,1); lcd.print("192.168.4.1");
  }

  // ── WEB SERVER ─────────────────────────────────────────────────────────
  webServer.on("/", handleRoot);
  webServer.on("/manifest.json", handleManifest);
  webServer.on("/update", handleOTAPage);
  webServer.on("/ota_upload", HTTP_POST, []() {
      if (Update.hasError()) {
        otaActive = false; lcd.clear(); lcd.print("OTA FAILED      "); lcd.setCursor(0,1); lcd.print("Check .bin file ");
        webServer.send(500, "text/html", "<html><body style='background:#0d1117;color:#f85149;font-family:Arial;padding:30px'><h2>&#10060; Update FAILED</h2><p>Wrong .bin or corrupt file.</p><p><a href='/update' style='color:#58a6ff'>Try again</a></p></body></html>");
      } else {
        lcd.clear(); lcd.print("OTA Done!       "); lcd.setCursor(0,1); lcd.print("Rebooting...    ");
        webServer.send(200, "text/html", "<html><body style='background:#0d1117;color:#4ade80;font-family:Arial;padding:30px'><h2>&#10003; Update complete!</h2><p>Rebooting — reconnect in 10 seconds.</p><script>setTimeout(()=>location.href='/',12000)</script></body></html>");
        delay(2000); ESP.restart();
      }
    }, handleOTAUpload);
  webServer.on("/api/state", handleApiState);
  webServer.on("/api/cmd", handleApiCmd);
  webServer.on("/api/save", HTTP_POST, handleApiSave);
  webServer.begin();
  
  pirWarmup = true; beep(2);
  lcd.clear(); lcd.print(apMode ? "Setup Mode" : "Guava Home");
  lcd.setCursor(0, 1); lcd.print(apMode ? "192.168.4.1" : WiFi.localIP().toString().c_str());
  tLCDWake = millis();
}

// ═══════════════════════════════════════════════════════════════════════
// LOOP
// ═══════════════════════════════════════════════════════════════════════
void loop() {
  unsigned long now = millis();
  if (!apMode && WiFi.status() == WL_CONNECTED) ArduinoOTA.handle();
  webServer.handleClient();
  if (apMode && (now - apStart > AP_TIMEOUT_MS)) ESP.restart();
  if (otaActive) return;
  if (pirWarmup && now > PIR_WARMUP_MS) { pirWarmup = false; logEvent("PIR ready"); }

  // ── HOTEL DOOR LOGIC ──
  if (dev.door) {
    bool rawIR = (digitalRead(PIN_DOOR_IR) == LOW);
    sens.doorIR = cfg_irDoorInvert ? !rawIR : rawIR;
    if (!sens.doorIR) { tDoor = now; } 
    else if (now - tDoor >= cfg_doorRelockMs) { lockDoor(); }
  }

  // ── CLAP SENSOR — double-clap state machine ──
  noInterrupts();
  uint8_t pulses = clapPulses;
  clapPulses = 0;
  interrupts();

  if (pulses > 0 && !cfg_clapMute && (now - tLastRelayFire > RELAY_SNAP_IGNORE_MS)) {
    if (!cfg_clapDouble) {
      // ── SINGLE MODE: any validated pulse triggers ──
      clapCount++;
      bool ns = !dev.light; setLight(ns); lightByPIR = false; tMotionEnd = 0;
      logEvent(String("Clap #") + clapCount + " — Light " + (ns ? "ON" : "OFF"));
    } else {
      // ── DOUBLE MODE: require two pulses 200–700ms apart ──
      if (tFirstClap == 0) {
        tFirstClap = now;
      } else {
        unsigned long gap = now - tFirstClap;
        if (gap >= DOUBLE_CLAP_MIN_MS && gap <= DOUBLE_CLAP_MAX_MS) {
          tFirstClap = 0;
          clapCount++;
          bool ns = !dev.light; setLight(ns); lightByPIR = false; tMotionEnd = 0;
          logEvent(String("DblClap #") + clapCount + " — Light " + (ns ? "ON" : "OFF"));
        } else {
          tFirstClap = now;
        }
      }
    }
  }
  // Double-clap wait-state timeout: reset if no second clap within 800ms
  if (tFirstClap > 0 && (now - tFirstClap > DOUBLE_CLAP_TIMEOUT)) { tFirstClap = 0; }

  // ── EXIT BUTTON ──
  bool btn1Now = digitalRead(PIN_BTN1);
  if (btn1Now == LOW && btn1LastState == HIGH && (now - tBtn1 > BTN_DEBOUNCE_MS)) { tBtn1 = now; unlockDoor(); }
  btn1LastState = btn1Now;

  // ── LCD PAGE BUTTON ──
  bool btn2Now = digitalRead(PIN_BTN2);
  if (btn2Now == HIGH && btn2LastState == LOW && (now - tBtn2 > BTN_DEBOUNCE_MS)) {
    tBtn2 = now; lcdPage = (LCDPage)(((int)lcdPage + 1) % PAGE_COUNT);
    lcdScrollPos = 0; lcd.clear(); logEvent("LCD → page " + String((int)lcdPage));
  }
  btn2LastState = btn2Now;

  // ── RFID SCAN ──
  if (!dev.door && rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = getUID(rfid.uid);
    String name = lookupRFID(uid); lastScannedUID = uid;
    if (name.length() > 0) { logEvent("RFID: " + name); tgSend("Access: " + name + "\nUID: " + uid); unlockDoor(); } 
    else { logEvent("RFID DENIED: " + uid); tgSend("DENIED\nUID: " + uid + "\nAdd in web Config if valid."); beep(3, 80); }
    rfid.PICC_HaltA(); rfid.PCD_StopCrypto1();
  }

  // ── FAST SENSORS ──
  if (now - tFast > SENSOR_FAST_MS) {
    tFast = now;
    sens.ldr = analogRead(PIN_LDR); sens.gas = analogRead(PIN_MQ2);
    sens.dist = getDistance(); sens.pir = pirWarmup ? false : (bool)digitalRead(PIN_PIR);
    bool rawIR = (digitalRead(PIN_DOOR_IR) == LOW); sens.doorIR = cfg_irDoorInvert ? !rawIR : rawIR;
    digitalWrite(PIN_LED, sens.pir ? HIGH : LOW);
    if (sens.dist > 0 && sens.dist <= LCD_WAKE_DIST_CM) { lcd.backlight(); tLCDWake = now; } 
    else if (now - tLCDWake > LCD_SLEEP_MS) { lcd.noBacklight(); }
  }

  // ── DHT11 ──
  if (now - tDHT > SENSOR_DHT_MS) {
    tDHT = now;
    float t = dht.readTemperature(), h = dht.readHumidity();
    if (!isnan(t) && !isnan(h) && t > -10 && t < 80) { sens.temp = t; sens.humidity = h; }
  }

  // ── SOIL ──
  if (now - tSoil > SENSOR_SOIL_MS) { tSoil = now; sens.soil = analogRead(PIN_SOIL); }

  // ── PUMP AUTO-CYCLE ──
  if (pump.enabled) {
    if (!pump.phaseOn) {
      if (now - pump.phaseStart >= (cfg_pumpOffTime * 1000UL)) {
        setPump(true); pump.phaseOn=true; pump.phaseStart=now; pump.cycleCount++;
        logEvent("Pump ON (cycle " + String(pump.cycleCount) + ")"); tgSend("Pump ON — cycle " + String(pump.cycleCount));
      }
    } else {
      if (now - pump.phaseStart >= (cfg_pumpOnTime * 1000UL)) {
        setPump(false); pump.phaseOn=false; pump.phaseStart=now; logEvent("Pump OFF — resting");
      }
    }
  }

  // ── AUTOMATION RULES ──
  if (!pirWarmup) {
    bool pirNow = sens.pir;
    if (pirNow && !pirLastState) {
      if (now - tPIRAlert > ALERT_COOL_MS) { tPIRAlert = now; logEvent("Motion detected"); tgSend("Motion detected"); }
      if (!dev.light && sens.ldr < THRESH_LDR_DARK) { setLight(true); lightByPIR = true; logEvent("Light ON (motion+dark)"); }
    }
    if (!pirNow && pirLastState) { tMotionEnd = now; }
    if (lightByPIR && dev.light && tMotionEnd > 0 && (now - tMotionEnd >= cfg_lightDelay)) {
      setLight(false); lightByPIR = false; tMotionEnd = 0; logEvent("Light OFF (auto " + String(cfg_lightDelay/1000) + "s)");
    }
    pirLastState = pirNow;
  }

  if (!isnan(sens.temp) && sens.temp > 0 && sens.temp >= cfg_tempFanOn && !dev.fan) {
    setFan(true); logEvent("Fan auto ON: " + String(sens.temp,1) + "C"); tgSend("Temp high: " + String(sens.temp,1) + "C — Fan ON");
  }
  if (sens.soil > cfg_soilDryThresh && (now - tSoilAlert > ALERT_COOL_MS)) {
    tSoilAlert = now; logEvent("Soil dry (" + String(sens.soil) + ")");
    beep(2, 80); tgSend("Plant needs water\nSoil ADC: " + String(sens.soil) + " (thresh:" + String(cfg_soilDryThresh) + ")");
  }
  if (sens.gas > THRESH_GAS_WARN && (now - tGasAlert > ALERT_COOL_MS)) {
    tGasAlert = now; logEvent("Gas/smoke: " + String(sens.gas));
    beep(4, 200); tgSend("GAS/SMOKE ALERT\nMQ-2 ADC: " + String(sens.gas) + "\nCheck immediately!");
  }
  if (sens.dist > 0 && sens.dist < THRESH_DIST_CM && (now - tDistAlert > ALERT_COOL_MS)) {
    tDistAlert = now; logEvent("Intrusion: " + String(sens.dist) + "cm");
    beep(3, 150); tgSend("INTRUSION: " + String(sens.dist) + "cm");
  }
  if (now - tLCD > LCD_UPDATE_MS) { tLCD = now; lcdShowPage(now); }

  // ── TELEGRAM POLLING ──
  if (!apMode && WiFi.status() == WL_CONNECTED && tgBot) {
    if (cfg_tgInterval > 0 && (now - tTGStatus >= (cfg_tgInterval * 60000UL))) { tTGStatus = now; executeCmd("status", 0); }
    if (now - tTG > cfg_tgPollMs) {
      tTG = now;
      int n = tgBot->getUpdates(tgBot->last_message_received + 1);
      for (int i = 0; i < n; i++) {
        if (tgBot->messages[i].chat_id == cfg_chatID) executeCmd(tgBot->messages[i].text, 0);
        else tgBot->sendMessage(tgBot->messages[i].chat_id, "Unauthorised.", "");
      }
    }
  }
}
