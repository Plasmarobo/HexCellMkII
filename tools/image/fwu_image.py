""" Consumes a binary file and produces a .fwu file """

from struct import *

#typedef struct __attribute__((packed, aligned(8)))
#{
#  uint32_t image_magic; // Determines image type
#  uint8_t  version_major;
#  uint8_t  version_minor;
#  uint8_t  version_patch;
#  uint8_t  hardware_revision;
#  uint32_t hardware_id; // Settable hardware address
#  uint32_t boot_address;
#} image_metadata_t;
#
#typedef struct __attribute__((packed, aligned(8)))
#{
#  image_metadata_t image_metadata;
#  uint32_t         chunk_size;
#  uint32_t         image_size;
#  uint32_t         crc;
#  uint32_t         flags;
#} firmware_update_metadata_t;

class ImageMetadata:
  """ Unpacks/packs image metadata from/to bytes """
  
  STRUCT_BYTE_FORMAT = '<IBBBBII'

  def __init__(self):
    self.image_magic = 0
    self.version_major = 0
    self.version_minor = 0
    self.version_patch = 0
    self.hardware_revision = 0
    self.hardware_id = 0
    self.boot_address = 0
  
  def from_bytes(self, raw_bytes):
    magic, major, minor, patch, hwr, hwi, ba = unpack(ImageMetadata.STRUCT_BYTE_FORMAT, raw_bytes)
    self.image_magic = magic
    self.version_major = major
    self.version_minor = minor
    self.version_patch = patch
    self.hardware_revision = hwr
    self.hardware_id = hwi
    self.boot_address = ba
    return self
  
  def to_bytes(self):
    return bytearray(pack(ImageMetadata.STRUCT_BYTE_FORMAT,
        int(self.image_magic),
        int(self.version_major),
        int(self.version_minor),
        int(self.version_patch),
        int(self.hardware_revision),
        int(self.hardware_id),
        int(self.boot_address)))

class FirmwareUpdateFactory:
  """ Factory for building firmware update factory """
  IMAGE_MAGIC_BOOTLOADER = 0xA9911C47
  IMAGE_MAGIC_APP = 0xB007104D
  IMAGE_MAGIC_INVALID = 0x00000000
  IMAGE_MAGIC_EMPTY = 0xFFFFFFFF
  IMAGE_MAGIC_METADATA = 0xFE7AD474

  FILE_BUFFER_SIZE = 0x1000

  def generate_fwu_file(self, input_filename, output_filename, version, boot_address, **kwargs):
    """ Writes out a fwu file """
    metadata = ImageMetadata()
    metadata.image_magic = kwargs.pop('image_magic', self.detect_image_type(input_filename))
    
    metadata.hardware_id = kwargs.pop('hardware_id', 0)
    metadata.hardware_revision = kwargs.pop('hardware_revision', 0)

    metadata.version_major = version.major
    metadata.version_minor = version.minor
    metadata.version_patch = version.patch

    metadata.boot_address = boot_address
    with open(input_filename, 'rb', FirmwareUpdateFactory.FILE_BUFFER_SIZE) as input_file:
      with open(output_filename, 'wb', FirmwareUpdateFactory.FILE_BUFFER_SIZE) as output_file:
        output_file.write(metadata.to_bytes())
        self.copy_bin_file(input_file, output_file)
        output_file.close()
        input_file.close()

  def detect_image_type(self, input_filename):
    """ Checks to see if image type can be guessed from filename """
    if input_filename.find("bootloader") != -1:
      return FirmwareUpdateFactory.IMAGE_MAGIC_APP
    elif input_filename.find("app") != -1:
      return FirmwareUpdateFactory.IMAGE_MAGIC_BOOTLOADER
    else:
      print("Could not deduce image type from filename {}".format(input_filename))
      return FirmwareUpdateFactory.IMAGE_MAGIC_INVALID

  def copy_bin_file(self, input_file, output_file):
    """ Chunked copy """
    for chunk in self.read_chunk(input_file):
      output_file.write(chunk)

  def read_chunk(self, input_file):
    """ Read and yield a chunk """
    while True:
      data = input_file.read(FirmwareUpdateFactory.FILE_BUFFER_SIZE)
      if not data:
        break
      yield data
