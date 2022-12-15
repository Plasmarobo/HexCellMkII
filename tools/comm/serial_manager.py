import time
import serial
from Queue import Queue
from threading import Thread
from message_parser import MessageParser

def serial_read_thread(port, queue):
  parser = MessageParser(queue)
  while True:
    if port.inWaiting() > 0:
      buffer = bytearray()
      buffer.extend(port.read(port.inWaiting()))
      parser.parse(buffer)
      

def serial_write_thread(port, queue):
  while True:
    if not queue.empty():
      msg = queue.get()
      # Write header
      buffer = bytearray()
      buffer.append(msg.status.to_bytes(1, 'little'))
      buffer.append(msg.length.to_bytes(2, 'little'))
      port.write(buffer)
      # Write body
      if msg.length > 0:
        buffer = bytearray()
        port.write(msg.data[0:msg.length])
      queue.task_done()

class SerialManager:
  """ A class that manages serial IO """
  def __init__(self, **kwargs):
    self.current_port = None
    self.producer = None
    self.consumer = None
    self.running = True
    self.incoming_queue = None
    self.outgoing_queue = None
    self.start_serial_thread(kwargs)

  def start_serial_thread(self, **kwargs):
    if self.current_port != None:
      self.current_port.close()
    self.current_port = serial.Serial(
      port=kwargs.pop('port', '/dev/ttyUSB0'),
      baudrate=kwargs.pop('baud', 115200),
      parity=serial.PARITY_ODD,
      stopbits=serial.STOPBITS_ONE,
      bytesize=serial.EIGHTBITS,
      timeout= 1)
    self.incoming_queue = Queue(maxsize=0)
    self.outgoing_queue = Queue(maxsize=0)
    try:
      self.current_port.open()
    except Exception as e:
      print("Error opening port: " + str(e))
      exit()

    if self.current_port.isOpen():
      try:
        self.current_port.flushInput()
        self.current_port.flushOutput()
        self.producer = Thread(target=serial_read_thread, args=(self.current_port, self.incoming_queue))
        self.producer.setDaemon(True)
        self.selfproducer.start()
        self.consumer = Thread(target=serial_write_thread, args=(self.current_port, self.outgoing_queue))
        self.consumer.setDaemon(True)
        self.consumer.start()
      except Exception as e:
        print("Error starting serial thread: " + str(e))
        exit()

    def send_message(self, msg):
      self.outgoing_queue.append(msg)

    def get_message(self):
      if self.incoming_queue.empty():
        return None
      msg = self.incoming_queue.get()
      self.incoming_queue.task_done()
      return msg

  def stop_serial_thread(self):
    self.running = False
    self.consumer.join()
    self.producer.join()
