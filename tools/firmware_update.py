from comm.serial_manager import SerialManager
from comm.message_parser import Message
import argparse

def main():
  """ Parses arguments and starts a firmware update """
  parser = argparse.ArgumentParser(
    prog = "FirmwareUpdate",
    description = "Starts a firmware update")
  parser.add_argument('-f', '--file', help='Path to a *.bin file for firmware upgrade')
  parser.add_argument('-p', '--port')

  args = parser.parse_args()
  serial = SerialManager(port=args.port)

  while True:
    msg = serial.get_message()
    if msg != None:
      

  return 0

if __name == '__main__':
  sys.exit(main())
