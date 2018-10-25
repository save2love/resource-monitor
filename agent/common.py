import os
import logging
import traceback
import struct
import math
import time
import psutil # pip install psutil
import serial # pip install pyserial
import config
from datetime import datetime

logging.basicConfig(filename=os.path.dirname(os.path.realpath(__file__)) + '\\log.log',level=logging.DEBUG)

def convert_size(size_bytes):
   if size_bytes == 0:
       return "0 b"
   size_name = ("b", "Kb", "Mb", "Gb", "Tb", "Pb", "Eb", "Zb", "Yb")
   i = int(math.floor(math.log(size_bytes, 1024)))
   p = math.pow(1024, i)
   s = round(size_bytes / p, 2)
   return "%s %s" % (s, size_name[i])

def sendNumericPacket(com, type, value, length):
	packet = bytearray()
	packet.append(ord('['))
	packet.append(type & 0xff)
	for i in range(0, length):
		packet.append((value >> ((length - i - 1) * 8)) & 0xff)
	packet.append(ord('\n'))
	com.write(packet)
   
def printHeader(start):
	if start:
		print("┌───────┬───────┬──────────┬──────────┬────────────┬────────────┬────────────┬────────────┐")
	else:
		print("├───────┼───────┼──────────┼──────────┼────────────┼────────────┼────────────┼────────────┤")
	print("│ CPU % │ MEM % │   USED   │   FREE   │ SENT b/sec │ RECV b/sec │ READ b/sec │ WRITE b/sec│")
	print("╞═══════╪═══════╪══════════╪══════════╪════════════╪════════════╪════════════╪════════════╡")


def mainLoop(console):
	try:
		if console:
			printHeader(True)
		
		com = serial.Serial(config.serial['port'], baudrate=115200, timeout=1)
		
		last_read = datetime.now()	# The last time of read sensors
		net_last_sent = 0		# Last sent bytes
		net_last_recv = 0
		has_data = False

		disk_last_read = 0;
		disk_last_write = 0;
		
		byte = 1
		lines = 0;	
		while True:
			cpu = psutil.cpu_percent(interval=1)
			mem = psutil.virtual_memory()
			disk = psutil.disk_io_counters(perdisk=False)
			net = psutil.net_io_counters() #pernic=True
			
			deltha_t = (datetime.now() - last_read).total_seconds();
			last_read = datetime.now()
			
			if has_data:
				# Calculate some metrics
				net_sent = (net.bytes_sent - net_last_sent) / deltha_t
				net_recv = (net.bytes_recv - net_last_recv) / deltha_t
				disk_read_bytes = (disk.read_bytes - disk_last_read) / deltha_t
				disk_write_bytes = (disk.write_bytes - disk_last_write) / deltha_t
				
				# Send metrics to Arduino
				sendNumericPacket(com, 1, int(round(cpu * 10)), 2)
				sendNumericPacket(com, 2, int(round(mem.percent * 10)), 2)
				sendNumericPacket(com, 3, int(mem.used / 1024.0), 4)
				sendNumericPacket(com, 4, int(mem.free / 1024.0), 4)
				sendNumericPacket(com, 5, int(net_sent / 1024.0), 4)
				sendNumericPacket(com, 6, int(net_recv / 1024.0), 4)
				sendNumericPacket(com, 7, int(disk_read_bytes / 1024.0), 4)
				sendNumericPacket(com, 8, int(disk_write_bytes / 1024.0), 4)

				# Print out to stdout
				if console:
					print("│"
					+ str(cpu).rjust(5, ' ') + "% │"
					+ str(mem.percent).rjust(5, ' ') + "% │ "
					+ convert_size(mem.used).rjust(8, ' ') + " │ "
					+ convert_size(mem.free).rjust(8, ' ') + " │ "
					+ convert_size(net_sent).rjust(10, ' ') + " │ "
					+ convert_size(net_recv).rjust(10, ' ') + " │ "
					+ convert_size(disk_read_bytes).rjust(10, ' ') + " │ "
					+ convert_size(disk_write_bytes).rjust(10, ' ') + " │")
			
			# Save previous values
			net_last_sent = net.bytes_sent
			net_last_recv = net.bytes_recv
			disk_last_read = disk.read_bytes
			disk_last_write = disk.write_bytes
			has_data = True

			lines += 1
			if lines > 20:
				printHeader(False)
				lines = 0

	except KeyboardInterrupt:
		pass
	except:
		logging.error("Unexpected error: " + traceback.format_exc())
		print("Unexpected error: " + traceback.format_exc())