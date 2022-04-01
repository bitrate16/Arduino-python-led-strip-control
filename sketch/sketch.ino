// This source contains Arduino based drivet for LED Strip control via UARD interface
//
//   Arduino python led strip control
//   Copyright (C) 2022 bitrate16 (pegasko) bitrate16@gmail.com
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <FastLED.h>

////////////////////////////////////////////////////////////////////////////////
// Configuration
////////////////////////////////////////////////////////////////////////////////

// PIN & Strip definitions
#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    60
#define BRIGHTNESS  255
#define BAUDRATE   	200000

// LED Array
CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

// Single structure used to transfer response
struct Response {
	uint8_t packetId = 0; // Used to identify frames
}; Response response;

// Single structure used to transfer one LED update request
struct Request {
	uint8_t packetId = 0; // Used to identify frames
	uint8_t begin    = 0; // Begin interval for LED values update
	uint8_t size     = 0; // Interval size
}; Request request;

// Single request block used to transfer LED color
struct RequestColorBlock {
	uint8_t r, g, b;
}; RequestColorBlock requestColorBlock;

////////////////////////////////////////////////////////////////////////////////
// Lifecycle
////////////////////////////////////////////////////////////////////////////////

inline void init_com() {
	Serial.begin(BAUDRATE);
}

void read_blocking(uint8_t* buf, uint8_t len) {
	while (Serial.available() < len);
	for (uint8_t i = 0; i < len; ++i)
		buf[i] = Serial.read();
}

void write_blocking(uint8_t* buf, uint8_t len) {
	Serial.write(buf, len);
}

void setup() {
	
	// 100 ms recovery delay
	delay(100);

	// Tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

	// Set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
	
	// XXX: Attiny85 has not enough memory for startup sequence
	// Clear strip
	for (uint8_t i = 0; i < NUM_LEDS; ++i)
		leds[i] = 0;
	FastLED.show();
	
	// Set up Serial connection
	init_com();
}
  
void loop() {
		
	// Read request
	read_blocking((uint8_t*) (&request), sizeof(Request));
	
	// Read blocks
	for (uint8_t i = request.begin; i < request.begin + request.size; ++i) {
		
		// Read color
		
		read_blocking((uint8_t*) (&requestColorBlock), sizeof(RequestColorBlock));
		
		// Apply color
		leds[i].r = requestColorBlock.r;
		leds[i].g = requestColorBlock.g;
		leds[i].b = requestColorBlock.b;
	}
	FastLED.show();
	
	// Respond with packet id
	response.packetId = request.packetId;
	write_blocking((uint8_t*) (&response), sizeof(Response));
}
