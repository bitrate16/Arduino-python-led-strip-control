# This source contains Arduino driver communicator written in python
#
#   Arduino python led strip control
#   Copyright (C) 2022 bitrate16 bitrate16@gmail.com
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.

import typing
import time
import serial

class State:
	packetId = 0
	devince = None

def connect(baudrate=115200, port='COM1'):
	"""
	Connect to Digispark Attiny85 and return current state.
	
	Returns current state.
	"""
	
	state = State()
	
	state.packetId = 0
	
	state.device = serial.Serial()
	state.device.port = port
	state.device.baudrate = baudrate
	state.device.open()
	
	# Object oriented
	state.close = close
	state.write_byte = write_byte
	state.send_led_state = send_led_state
	state.connect_waiting = connect_waiting
	
	if state.device:
		return state
	
	return None

def close(state):
	state.device.close()

def write_byte(state, b):
	state.device.write(b.to_bytes(1, 'big'))

def send_led_state(state, led_state: typing.List[typing.List[int]], start: int=0, size_mode=0):
	"""
	Send LED state to the board.
	Input led_state is a list containing lists with 3 color R, G, B components.
	start defines starting LED index.
	size_mode defines usage of uint16_t & uint32_t (mode = 1) or usage of uint8_t (mode = 0)
	
	Returns ID of response. 
	"""
	
	# Transmit Packet ID
	state.packetId += 1
	state.packetId = state.packetId & 0xFF
	write_byte(state, state.packetId & 0xFF)
	
	# Transmit interval begin
	write_byte(state, start & 0xFF)
	
	# Transmit interval size
	write_byte(state, len(led_state) & 0xFF)
	
	state.device.flush()
	
	# Transmit data blocks
	for i in range(len(led_state)):
		if len(led_state[i]) != 3:
			write_byte(state, 0)
			write_byte(state, 0)
			write_byte(state, 0)
		else:	
			write_byte(state, led_state[i][0] & 0xFF)
			write_byte(state, led_state[i][1] & 0xFF)
			write_byte(state, led_state[i][2] & 0xFF)
	
		state.device.flush()
	
	# Receive response
	packetId = state.device.read(1)
	
	return packetId

def connect_waiting(baudrate=115200, port='COM1', sleep=0.1, log=False):
	"""
	Connect and wait for port to be available for connection
	"""
	
	import traceback
	
	while True:
		try:
			return connect(baudrate=baudrate, port=port)
		except:
			if log:
				traceback.print_exc()
			time.sleep(sleep)