""" Contains classes to parse messages """

class Message:
  """ A structure that contains message data """

  def __init__(self):
    self.status = 0 
    self.length = 0
    self.data = bytearray()

  def status(self):
    return self.status

  def length(self):
    return self.length

  def data(self):
    return self.data

class MessageParser:
  """ Contains a state machine that processes incoming bytes into messages """
  STATE_PARSE_HEADER = 0
  STATE_PARSE_MESSAGE = 1
  STATE_PARSE_COMPLETE = 2

  def __init__(self, message_queue):
    self.state = MessageParser.STATE_PARSE_HEADER
    self.queue = message_queue
    self.parse_len = 0
    self.message = Message()

  def parse(self, incoming_bytes):
    while(len(incoming_bytes) > 0):
      if self.state == MessageParser.STATE_PARSE_HEADER:
          self.message.status = incoming_bytes[0]
          incoming_bytes = incoming_bytes[1:]
          self.message.length = int.from_bytes(incoming_bytes[0:2], byteorder='little', signed=False)
          self.parse_len = self.message.length
          incoming_bytes = incoming_bytes[2:]
          self.state = STATE_PARSE_MESSAGE
          self.notify("Parsed header status: {:x}, len: {}".format(self.message.status, self.message.length))
      elif self.state == MessageParser.STATE_PARSE_MESSAGE:
        if self.parse_len < len(incoming_bytes):
          # Grab bytes up to message size, then restart parsing on remainder
          self.message.data.extend(incoming_bytes[0:self.parse_len])
          incoming_bytes = incoming_bytes[self.parse_len:]
          self.queue.push(self.message)
          self.message = Message()
          self.parse_len = 0
          self.state = MessageParser.STATE_PARSE_HEADER
          self.parse(incoming_bytes)
        else:
          # Consume remainder of bytes
          self.message.data.extend(incoming_bytes)
          self.parse_len -= len(incoming_bytes)
        incoming_bytes.clear()
      else:
        self.state = MessageParser.STATE_PARSE_HEADER

  def notify(self, msg):
    print(msg)
    


