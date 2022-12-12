import argparse
from image.fwu_image import FirmwareUpdateFactory
from datetime import datetime
import semver
import sys

def main():
  """ Parses arguments and starts a firmware update """
  parser = argparse.ArgumentParser(
    prog = "FirmwareUpdate",
    description = "Starts a firmware update")
  parser.add_argument('-f', '--file', help='Path to a *.bin file for firmware upgrade', required=True)
  parser.add_argument('-v', '--version', help='A symantic version number in X.Y.Z format', required=True)
  parser.add_argument('-i', '--image_type', help='Type of image to generate, deduced from filename if not set', choices=['app', 'bootloader'])
  parser.add_argument('-b', '--boot_address', help='Boot address in hexidecimal', default="0xFFFFFFFF")
  parser.add_argument('-r', '--hardware_revision', help='Hardware revision override', required=True)
  parser.add_argument('-d', '--hardware_id', help='Hardware id override', default=0)
  parser.add_argument('-o', '--out', help='Output filename override')

  args = parser.parse_args()
  fwu_builder = FirmwareUpdateFactory()

  if not args.out:
    target_filename = "{}_{}_{}{}_{}.fwu".format(args.image_type, args.hardware_revision, args.version, args.boot_address, datetime.now())
  else:
    target_filename = args.out


  ver = semver.VersionInfo.parse(args.version)
  boot_address = int(args.boot_address, 16)

  fwu_builder.generate_fwu_file(args.file, target_filename, ver, boot_address, hardware_id=args.hardware_id, hardware_revision=args.hardware_revision)

  return 0

if __name__ == '__main__':
  sys.exit(main())
