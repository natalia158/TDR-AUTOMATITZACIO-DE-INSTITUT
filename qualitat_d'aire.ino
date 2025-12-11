#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WiFi.h"
#include "ABlocks_DHT.h"
#include <DFRobot_ENS160.h>
#include "ABlocks_Button.h"
#include <HTTPClient.h>
#include <base64.h>

double Temperatura;
double Humetat;
double CO2;
String s_URL2;
boolean b_CanviManual;
Adafruit_SSD1306 oled_1(128,64, &Wire,-1);
bool oled_1_autoshow=true;
String espwifi_ssid="";
String espwifi_pass="";
unsigned long espwifi_reconnect_last_time = 0;
DHT dht26(26,DHT11);
DFRobot_ENS160_I2C sensor_ens160(&Wire, 0x53);
Button button_debounced_18(18,50);

HTTPClient http_client;
unsigned long task_time_ms=0;

void espwifi_check() {
    if (WiFi.status() != WL_CONNECTED) {
        if( (millis() - espwifi_reconnect_last_time) >= 5000) {
            espwifi_reconnect_last_time = millis();
            WiFi.reconnect();
        }
    }
}

void espwifi_setup(String _ssid, String _pass){
	WiFi.mode(WIFI_STA);
	espwifi_ssid=_ssid;
	espwifi_pass=_pass;
	WiFi.begin(_ssid.c_str(),_pass.c_str());
	unsigned long wc = millis();
	while ( (WiFi.status() != WL_CONNECTED) && ( (millis()-wc) < 10000) ) delay(250);
}

double fnc_ens160_data(int _type, double _temp, double _hum)
{
	double v=0.0;
	sensor_ens160.setTempAndHum(_temp,_hum);
	if(_type==0)v=sensor_ens160.getECO2();
	if(_type==1)v=(sensor_ens160.getECO2()*1.8);
	if(_type==2)v=sensor_ens160.getTVOC();
	if(_type==3)v=sensor_ens160.getAQI();
	if(_type==4)v=sensor_ens160.getENS160Status();
	return v;
}

String fnc_http_client_get(String _url, String _user, String _pass,bool _response, bool _redirect){
	String resp="";
	int respcode;
	if(_redirect){
		http_client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
	}
	else{
		http_client.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
	}
	http_client.begin(_url.c_str());
	if(_user!="" || _pass!=""){
		String auth = base64::encode(_user + ":" + _pass);
		http_client.addHeader("Authorization", "Basic " + auth);
	}
	respcode=http_client.GET();
	if(_response){
		if(respcode>0){
			resp=http_client.getString();
		}else{
			resp="HTTP-GET-ERROR: "+String(respcode);
		}
	}
	http_client.end();
	return resp;
}

String fnc_http_client_post(String _url, String _data, String _user, String _pass, String _contenttype, bool _response, bool _redirect){
	String resp="";
	int respcode;
	if(_redirect){
		http_client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
	}
	else{
		http_client.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
	}
	http_client.begin(_url.c_str());
	if(_user!="" || _pass!=""){
		String auth = base64::encode(_user + ":" + _pass);
		http_client.addHeader("Authorization", "Basic " + auth);
	}
	http_client.addHeader("Content-Type",_contenttype);
	respcode=http_client.POST(_data);
	if(_response){
		if(respcode>0){
			resp=http_client.getString();
		}else{
			resp="HTTP-POST-ERROR: "+String(respcode);
		}
	}
	return resp;
}

void setup()
{
  	pinMode(27, OUTPUT);
	pinMode(26, INPUT);
	pinMode(18, INPUT);

	Serial.begin(115200);
	Serial.flush();
	while(Serial.available()>0)Serial.read();

	dht26.begin();
	sensor_ens160.begin();
	sensor_ens160.setPWRMode(ENS160_STANDARD_MODE);

	oled_1.begin(SSD1306_SWITCHCAPVCC,0x3C);
	espwifi_setup(String("Galaxy A33 5G EA00"),String("dbco8108"));
	delay(3000);
	digitalWrite(27,LOW);
	b_CanviManual = false;

}


void loop()
{
	yield();

	espwifi_check();

  	Temperatura = dht26.readTemperature();
  	Humetat = dht26.readHumidity();
  	CO2 = fnc_ens160_data(0,Temperatura,Humetat);
  	oled_1.clearDisplay();
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.drawLine(64,0,64,64,WHITE);
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.drawLine(0,22,128,22,WHITE);
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.setTextSize(2);
  	oled_1.setTextColor(WHITE);
  	oled_1.setCursor(30,5);
  	oled_1.print(String("T"));
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.setTextSize(2);
  	oled_1.setTextColor(WHITE);
  	oled_1.setCursor(94,5);
  	oled_1.print(String("H"));
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.setTextSize(1);
  	oled_1.setTextColor(WHITE);
  	oled_1.setCursor(15,40);
  	oled_1.print(String(Temperatura)+String("C"));
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.setTextSize(1);
  	oled_1.setTextColor(WHITE);
  	oled_1.setCursor(79,40);
  	oled_1.print(String(Humetat)+String("%"));
  	if(oled_1_autoshow)oled_1.display();
  	oled_1.display();
  	if (button_debounced_18.pressed()) {
  		b_CanviManual = (!b_CanviManual);
  	}

  	if ((((((Temperatura >= 28) && (!b_CanviManual)) || ((!((Temperatura >= 28))) && b_CanviManual))) == 0)) {
  		digitalWrite(27,HIGH);
  	}
  	else {
  		digitalWrite(27,LOW);
  	}

  	if((millis()-task_time_ms)>=30000){
  		task_time_ms=millis();
  		s_URL2 = String("https://docs.google.com/forms/d/1EiXeKlq41weK7dTjz_SnujM8-Ke2HUkRrdzMyb_vNW4/formResponse?usp=pp_url&entry.958814289=")+String(Temperatura)+String("&entry.524074273=")+String(Humetat)+String("&entry.225244453=")+String(CO2)+String("&submit=Submit");
  		fnc_http_client_get(s_URL2,String(""),String(""),false,true);
  	}
  	delay(2000);

}