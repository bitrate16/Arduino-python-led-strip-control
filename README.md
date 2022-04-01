# Arduino LED strip controlle with python interface

Python interface to drive your LED strips.

### > What is it?

This project aims to bring python-arduino LED strip communication interface. using this interface you can implement any kind of animation for LED strips without buying LED drivers

### > Why is it?

This project made as small weekend code rest primary for my semi-dead led strip dticked under the table. I believe once it will shine in the rhytm of playing mysic, byt pyaudio can not capture output devices

### > It?

Yes, hello>?

## How to use

For details, view attached notebook.ipynb for 3 demos and source of communicator.py that wraps communication with arduino.

## How it works?

Communication is built on simple request-response protocol where host (python) transmits packet with led update information and additional packets used to define color of each updated LED.

Host packet consists of the following:
* packetId - for identification in asynchronous read/write mode (not implemented)
* begin - index of LED to start with
* size - amount of LEDs to be updated

Additinal LED packet contains following:
* R - red color byte
* G - green color byte
* B - blue color byte

Response packet contains only one field:
* packetId - ID of processed packet

## How to use / configure?

Main configuration is specified in sketch.ino:
```C++
#define DATA_PIN    4      # LED strip attachment PIN
#define LED_TYPE    WS2811 # LED strip type
#define COLOR_ORDER GRB    # LED strip color sequence type
#define NUM_LEDS    60     # Number of LEDs in strip
#define BRIGHTNESS  255    # Brightness value
#define BAUDRATE   	200000 # Communication port speed as baud rate
```

Suggested tweaks:
* Change DATA_PIN for your needs
* Change LED type to match your LED type
* Configure number of LEDS (both - sketch and your code), mismatch will lead to memory access violation and microcontroller reboot / death
* Baud rate (both - sketch and your code) to change data transmission speed
  * Note that standard 9600 baud will give you about 2 frames per second, but 115200 is already enough
  * Go crazy, configure 400000 baud rate

## Example: Random flickering burst animation

```python
import communicate
import random
import time

# Config
SPAWN_DELAY  = 0.1
SPAWN_AMOUNT = 2
FADE_DECAY   = 10
FPS          = 30.0
NUM_LEDS     = 60

# Logic
invFPS = 1.0 / FPS
leds = [ [ 0, 0, 0 ] for i in range(NUM_LEDS) ]

state = communicate.connect_waiting(baudrate=200000, port='COM4')
time.sleep(2)
timestamp = time.time()

try:
	while True:
		if timestamp + SPAWN_DELAY < time.time():
			for i in range(SPAWN_AMOUNT):
				leds[random.randint(0, NUM_LEDS - 1)] = [ random.randint(0, 255), random.randint(0, 255), random.randint(0, 255) ]
			timestamp = time.time()
		
		communicate.send_led_state(state, leds, 0)
		time.sleep(invFPS)
		
		for l in leds:
			l[0] = max(0, l[0] - FADE_DECAY)
			l[1] = max(0, l[1] - FADE_DECAY)
			l[2] = max(0, l[2] - FADE_DECAY)
except:
	communicate.close(state)
```

## Example: Ping-Pong dot animation

```python
import communicate
import time

# Config
FPS      = 60.0
NUM_LEDS = 60
COLOR    = [ 0, 255, 0 ]

# Logic
invFPS = 1.0 / FPS
leds = [ [ 0, 0, 0 ] for i in range(60) ]
zero = [ 0, 0, 0 ]

state = communicate.connect_waiting(baudrate=200000, port='COM4')
time.sleep(2)
timestamp = time.time()

try:
	while True:
		for i in range(NUM_LEDS):
			leds[i] = COLOR
			communicate.send_led_state(state, leds, 0)
			time.sleep(invFPS)
			leds[i] = zero
		
		for i in reversed(range(NUM_LEDS)):
			leds[i] = COLOR
			communicate.send_led_state(state, leds, 0)
			time.sleep(invFPS)
			leds[i] = zero
except:
	communicate.close(state)
```

## Example: Random colors
```python
import communicate
import random
import time

# Config
FPS      = 60.0
NUM_LEDS = 60

# Logic
invFPS = 1.0 / FPS

state = communicate.connect_waiting(baudrate=200000, port='COM4')
time.sleep(2)
timestamp = time.time()

def random_list():
	return [ [ random.randint(0, 255), random.randint(0, 255), random.randint(0, 255) ] for i in range(NUM_LEDS) ]

try:
	while True:
		communicate.send_led_state(state, random_list(), 0)
		time.sleep(invFPS)
except:
	communicate.close(state)
```

## TODO:
* Implement pyAudio capture to render music driven visualizer
* Encapsulate communicate.State to singleton Opject-Oriented pattern
* * Maybe make a module (we need more modules)

## Credits

* [FastLed library](https://github.com/FastLED/FastLED)
* [pySerial](https://github.com/pyserial/pyserial)