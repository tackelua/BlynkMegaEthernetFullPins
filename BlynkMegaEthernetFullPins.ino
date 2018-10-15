// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
	Name:       FullPins.ino
	Created:	11-Oct-18 09:30:06
	Author:     GITH\tacke
	Version:	0.2.1
*/


//#define BLYNK_PRINT Serial


#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>


char auth[] = "df141a844691408f8d668a4b9d1e7f87";
char host[] = "gith.cf";
uint16_t port = 8442;


//#define DEBUG(x);		Terminal.println(x);Terminal.flush();

unsigned long RELAY_ON_DURATION = 300;	//ms
#define R_ON	LOW
#define R_OFF	HIGH

char* C_ONLINE = "#00ff00";				//GREEN
char* C_OFFLINE = "#020202";			//BLACK
char* C_RELAY_ON = "#f70202";			//RED


#define W5100_CS  10
#define SDCARD_CS 4

#define NUM_OF_RELAYS	16

//------------------------------------------------------
bool LED_S[NUM_OF_RELAYS + 1];		   //blynk app
byte LED_P[NUM_OF_RELAYS + 1];		   //blynk app
bool BUTTON_S[NUM_OF_RELAYS + 1];	   //blynk app
unsigned long TIME_PUSH[NUM_OF_RELAYS + 1];
bool RELAY_S[NUM_OF_RELAYS + 1];	   //hardware board
byte RELAY_P[NUM_OF_RELAYS + 1];	   //hardware board
unsigned long TIME_RELAY_ON[NUM_OF_RELAYS + 1];
//------------------------------------------------------

void init_relay_pins() {
	RELAY_P[1] = 0;
	RELAY_P[2] = 1;
	RELAY_P[3] = 2;
	RELAY_P[4] = 3;
	RELAY_P[5] = 4;
	RELAY_P[6] = 5;
	RELAY_P[7] = 6;
	RELAY_P[8] = 7;

	RELAY_P[9] = 8;
	RELAY_P[10] = 9;
	RELAY_P[11] = A0;
	RELAY_P[12] = A1;
	RELAY_P[13] = A2;
	RELAY_P[14] = A3;
	RELAY_P[15] = A4;
	RELAY_P[16] = A5;

	//RELAY_P[1] = A0;
	//RELAY_P[2] = A1;
	//RELAY_P[3] = A2;
	//RELAY_P[4] = A3;
	//RELAY_P[5] = A4;
	//RELAY_P[6] = A5;
	//RELAY_P[7] = A6;
	//RELAY_P[8] = A7;

	//RELAY_P[9] = A8;
	//RELAY_P[10] = A9;
	//RELAY_P[11] = A10;
	//RELAY_P[12] = A11;
	//RELAY_P[13] = A12;
	//RELAY_P[14] = A13;
	//RELAY_P[15] = A14;
	//RELAY_P[16] = A15;

	//RELAY_P[17] = 14;
	//RELAY_P[18] = 15;
	//RELAY_P[19] = 16;
	//RELAY_P[20] = 17;
	//RELAY_P[21] = 18;
	//RELAY_P[22] = 19;
	//RELAY_P[23] = 20;
	//RELAY_P[24] = 21;

	//RELAY_P[25] = 22;
	//RELAY_P[26] = 23;
	//RELAY_P[27] = 24;
	//RELAY_P[28] = 25;
	//RELAY_P[29] = 26;
	//RELAY_P[30] = 27;
	//RELAY_P[31] = 28;
	//RELAY_P[32] = 29;

	//RELAY_P[33] = 30;
	//RELAY_P[34] = 31;
	//RELAY_P[35] = 32;
	//RELAY_P[36] = 33;
	//RELAY_P[37] = 34;
	//RELAY_P[38] = 35;
	//RELAY_P[39] = 36;
	//RELAY_P[40] = 37;

	//RELAY_P[41] = 38;
	//RELAY_P[42] = 39;
	//RELAY_P[43] = 40;
	//RELAY_P[44] = 41;
	//RELAY_P[45] = 42;
	//RELAY_P[46] = 43;
	//RELAY_P[47] = 44;
	//RELAY_P[48] = 45;

	for (byte i = 1; i <= NUM_OF_RELAYS; i++) {
		pinMode(RELAY_P[i], OUTPUT);
		RELAY_S[i] = R_OFF;
		digitalWrite(RELAY_P[i], RELAY_S[i]);
	}
}


//=============================================================================================

WidgetLED LED(0);

#ifdef DEBUG
WidgetTerminal Terminal(V0);
#else
#define DEBUG(x);
#endif // !DEBUG

void relay_on(byte i) {
	DEBUG("Relay " + String(i) + " ON");
	RELAY_S[i] = R_ON;
	digitalWrite(RELAY_P[i], RELAY_S[i]);
	TIME_RELAY_ON[i] = millis();
	LED.setVPin(LED_P[i]);
	LED.setColor(C_RELAY_ON);
}
void relay_off(byte i) {
	DEBUG("Relay " + String(i) + " OFF");
	RELAY_S[i] = R_OFF;
	digitalWrite(RELAY_P[i], RELAY_S[i]);
	LED.setVPin(LED_P[i]);
	char* color = LED_S[i] ? C_ONLINE : C_OFFLINE;
	LED.setColor(color);
}

void relay_auto_off(unsigned long t) {
	for (byte i = 1; i <= NUM_OF_RELAYS; i++) {
		if (RELAY_S[i] == R_ON && (millis() - TIME_RELAY_ON[i] > t)) {
			relay_off(i);
		}
	}
}

void wait(unsigned long ms) {
	unsigned long t = millis();
	while (millis() - t < ms) {
		Blynk.run();
	}
}


#define BLYNK_PRESS(x);		BLYNK_WRITE(x) {												  \
								int stt = param.asInt();									  \
								DEBUG(String(x) + ":" + String(stt));						  \
								if (!BUTTON_S[x] && stt) {									  \
									TIME_PUSH[x] = millis();								  \
									DEBUG("TIME_PUSH: " + String(TIME_PUSH[x]));			  \
								}															  \
								if (BUTTON_S[x] && !stt) {									  \
									unsigned long push_duration = millis() - TIME_PUSH[x];	  \
									DEBUG("push_duration: " + String(push_duration));		  \
									if (push_duration > 150 && push_duration < 3000) {		  \
										LED_S[x] = !LED_S[x];								  \
										relay_on(x);										  \
									}														  \
								}															  \
								BUTTON_S[x] = stt;											  \
							}																  


BLYNK_PRESS(1);
BLYNK_PRESS(2);
BLYNK_PRESS(3);
BLYNK_PRESS(4);
BLYNK_PRESS(5);
BLYNK_PRESS(6);
BLYNK_PRESS(7);
BLYNK_PRESS(8);
BLYNK_PRESS(9);
BLYNK_PRESS(10);
BLYNK_PRESS(11);
BLYNK_PRESS(12);
BLYNK_PRESS(13);
BLYNK_PRESS(14);
BLYNK_PRESS(15);
BLYNK_PRESS(16);
//BLYNK_PRESS(17);
//BLYNK_PRESS(18);
//BLYNK_PRESS(19);
//BLYNK_PRESS(20);
//BLYNK_PRESS(21);
//BLYNK_PRESS(22);
//BLYNK_PRESS(23);
//BLYNK_PRESS(24);
//BLYNK_PRESS(25);
//BLYNK_PRESS(26);
//BLYNK_PRESS(27);
//BLYNK_PRESS(28);
//BLYNK_PRESS(29);
//BLYNK_PRESS(30);
//BLYNK_PRESS(31);
//BLYNK_PRESS(32);
//BLYNK_PRESS(33);
//BLYNK_PRESS(34);
//BLYNK_PRESS(35);
//BLYNK_PRESS(36);
//BLYNK_PRESS(37);
//BLYNK_PRESS(38);
//BLYNK_PRESS(39);
//BLYNK_PRESS(40);
//BLYNK_PRESS(41);
//BLYNK_PRESS(42);
//BLYNK_PRESS(43);
//BLYNK_PRESS(44);
//BLYNK_PRESS(45);
//BLYNK_PRESS(46);
//BLYNK_PRESS(47);
//BLYNK_PRESS(48);


BLYNK_CONNECTED() {
	LED.setVPin(LED_P[0]);
	for (byte i = 1; i <= NUM_OF_RELAYS; i++) {
		LED.setVPin(LED_P[i]);
		LED.setColor(C_OFFLINE);
		LED.on();
	}
	if (NUM_OF_RELAYS < 48) {
		for (byte i = NUM_OF_RELAYS + 1; i <= 48; i++) {
			LED.setVPin(LED_P[i]);
			LED.setColor(C_OFFLINE);
			LED.off();
		}
	}
	for (byte i = 1; i <= NUM_OF_RELAYS; i++) {
		relay_off(i);
	}
}

void setup()
{
	//Serial.begin(115200);

	for (byte i = 1; i <= NUM_OF_RELAYS; i++) {
		LED_S[i] = false;
		LED_P[i] = i + 49; //const for LED V
		BUTTON_S[i] = false;
	}

	init_relay_pins();

	pinMode(SDCARD_CS, OUTPUT);
	digitalWrite(SDCARD_CS, HIGH);

	Blynk.begin(auth, host, port);
}

void loop()
{
	Blynk.run();
	relay_auto_off(RELAY_ON_DURATION);
}
