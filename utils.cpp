/*
 * utils.cpp
 *
 *  Created on: 11 Apr 2018
 *      Author: andre.louw
 */

#include "utils.h"
#include "Proxies.h"

void flashLED(uint8_t times, uint16_t interval) {
	uint8_t k;
	for (k = 0; k < times; k++) {
		digitalWrite(LED_BUILTIN, 1);
		delay(interval);
		digitalWrite(LED_BUILTIN, 0);
		delay(interval);
	}
}

int print2digits(char *buffer, int offset, int number) {
	static const char nybble_chars[] = "0123456789";
	if (number >= 0 && number < 10) {
		buffer[offset++] = '0';
		buffer[offset++] = nybble_chars[number];
	} else {
		buffer[offset++] = nybble_chars[number/10];
		buffer[offset++] = nybble_chars[number-((number/10)*10)];
	}
	return offset;
}

Split::Split(String str, char delim)
: start(0)
, content(str)
, delimiter(delim) {
}

String Split::stringToken() {
	String value;
	if (start < 0 || start >= (int16_t)content.length())
		return value;
	int sep = content.indexOf(delimiter, start);
	if (sep > 0) {
		value = content.substring(start, sep);
		start = sep+1;
	} else
		start = -1;
	return value;
}

float_t Split::floatToken() {
	float_t value = 0.0;
	if (start < 0 || start >= (int16_t)content.length())
		return value;
	int sep = content.indexOf(delimiter, start);
	if (sep > 0) {
		value = content.substring(start, sep).toFloat();
		start = sep+1;
	} else
		start = -1;
	return value;
}

int16_t Split::intToken() {
	int16_t value = 0;
	if (start < 0 || start >= (int16_t)content.length())
		return value;
	int sep = content.indexOf(delimiter, start);
	if (sep > 0) {
		value = content.substring(start, sep).toInt();
		start = sep+1;
	} else
		start = -1;
	return value;
}

void APICallTXResponse(ZBTxStatusResponse& tx, uintptr_t data) {
	TXResponseHandlers *handlers = (TXResponseHandlers *)data;
	if (tx.getDeliveryStatus() == SUCCESS) {
		if (handlers->success != 0)
			(handlers->success)(0);
	} else {
#if (SERIAL_DEBUG)
		Serial.print("ZBTxError: ");
		Serial.println(tx.getDeliveryStatus(), 16);
#endif
		if (handlers->error != 0)
			(handlers->error)(0);
	}
}

RemoteAPI::RemoteAPI(XBeeWithCallbacks *target, XBeeAddress64 addr64) {
	this->target = target;
	this->addr64 = addr64;
}
void RemoteAPI::transmitStateFromProxy(APIMethod method, Proxy *payload, TXSuccessResponse success, TXErrorResponse error) {
	ZBTxRequest request = ZBTxRequest(addr64, (uint8_t*)payload->buffer((uint8_t)method), payload->bufferSize());
	TXResponseHandlers *handlers = new TXResponseHandlers;
	handlers->success = success;
	handlers->error = error;
	target->onZBTxStatusResponse(APICallTXResponse, (uintptr_t)handlers);
	target->send(request);
}
void RemoteAPI::transmitStateWithPayload(APIMethod method, String payload, TXSuccessResponse success, TXErrorResponse error) {
	String str;
	str.append((char)method);
	str.append(payload);
	ZBTxRequest request = ZBTxRequest(addr64, (uint8_t*)const_cast<char*>(str.c_str()), str.length());
	TXResponseHandlers *handlers = new TXResponseHandlers;
	handlers->success = success;
	handlers->error = error;
	target->onZBTxStatusResponse(APICallTXResponse, (uintptr_t)handlers);
	target->send(request);
}
void RemoteAPI::transmitState(APIMethod method, TXSuccessResponse success, TXErrorResponse error) {
	ZBTxRequest request = ZBTxRequest(addr64, (uint8_t*)&method, 1);
	TXResponseHandlers *handlers = new TXResponseHandlers;
	handlers->success = success;
	handlers->error = error;
	target->onZBTxStatusResponse(APICallTXResponse, (uintptr_t)handlers);
	target->send(request);
}
