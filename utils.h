/*
 * utils.h
 *
 *  Created on: 12 Apr 2018
 *      Author: andre.louw
 */

#ifndef LIBRARIES_CHALET_UTILS_H_
#define LIBRARIES_CHALET_UTILS_H_

#include <Arduino.h>
#include <XBee.h>

#define DebugSerial Serial
#define SERIAL_DEBUG 1

void flashLED(uint8_t times, uint16_t interval);
void debugLog(const char s[], bool println=true);

class Proxy;

class Split {
public:
	Split(String str, char delim);
	String stringToken();
	float_t floatToken();
	int16_t intToken();
private:
	int16_t start;
	String content;
	char delimiter;
};

typedef enum {
	SCREEN_REGISTER 		= 0x01,
	FLOW_UPDATED			= 0x02,
	TANK_EMPTIED			= 0x03,
	TANK_FILLED				= 0x04,
	ADJUST_TANK_VOLUME		= 0x05,
	SETUP_CALIBRATION		= 0x06,
	START_CALIBRATION		= 0x07,
	STOP_CALIBRATION		= 0x08,
	CANCEL_CALIBRATION		= 0x09,
	DONE_CALIBRATION		= 0x0A,
	STORE_CALIBRATION		= 0x0B,
	SCREEN_REGISTERED 		= 0x0C,
	POWER_UPDATED	 		= 0x0D,
	BATTERY_FILLED	 		= 0x0E,
	ADJUST_BATTERY_CAPACITY	= 0x0F,
	ADJUST_COULOMB_EFFIC	= 0x10,
	ADJUST_PEUKERT_CONSTANT	= 0x11,
} APIMethod;

typedef void (*TXSuccessResponse)(uint8_t *);
typedef void (*TXErrorResponse)(uint8_t *);

typedef struct {
	TXSuccessResponse success;
	TXErrorResponse error;
} TXResponseHandlers;

class RemoteAPI {
public:
	RemoteAPI(XBeeWithCallbacks *target, XBeeAddress64 addr64);
	void transmitStateFromProxy(APIMethod method, Proxy *payload, TXSuccessResponse success=0, TXErrorResponse error=0);
	void transmitStateWithPayload(APIMethod method, String payload, TXSuccessResponse success=0, TXErrorResponse error=0);
	void transmitState(APIMethod method, TXSuccessResponse func=0, TXErrorResponse error=0);

	XBeeWithCallbacks *target;
	XBeeAddress64 addr64;
};

class MovingAvgLastN {
public:
	MovingAvgLastN() {
		maxTotal = 20;
		avg = 0.0;
		head = 0;
		total = 0;
	}
	MovingAvgLastN *add(float_t num) {
		float_t prevSum = (float_t)total*avg;
		if (total == maxTotal) {
			prevSum -= lastN[head];
			total--;
		}
		head = (head+1)%maxTotal;
		int emptyPos = (maxTotal+head-1)%maxTotal;
		lastN[emptyPos] = num;
		float_t newSum = prevSum+num;
		total++;
		avg = newSum/(float_t)total;
		return this;
	}
	float_t average() {
		return avg;
	}
	void setAverage(float_t average) {
		head = 0;
		total = 0;
		add(average);
	}
private:
	int maxTotal;
	int total;
	float_t lastN[20];
	float_t avg;
	int head;
};

#endif /* LIBRARIES_CHALET_UTILS_H_ */
