
class FirmwareUpdateStateMachine:
  """ An FSM which conducts a firmware update """

  STATE_START = 0
  STATE_SENDING = 1
  STATE_VERIFY = 2
  STATE_COMPLETE = 3

  def __init__(self):
    self.state = STATE_START

  def trigger_update(self, fwu_file):
    

  def handle_message(self, message):
    if self.state == STATE_START
  