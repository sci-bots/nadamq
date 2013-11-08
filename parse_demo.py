import re
import numpy as np

from PacketParser import cPacketParser, compute_crc16


def main():
    args = parse_args()
    parser = cPacketParser()

    cre_command_byte = re.compile(r'0x[0-9a-fA-F]{2}')
    match = cre_command_byte.match(args.command_byte)
    if match is None:
        raise ValueError, 'Invalid command byte: %s' % args.command_byte
    command = chr(eval(args.command_byte))
    payload = args.payload
    crc = compute_crc16(payload)

    # Construct packet from:
    #   * Command octet
    #   * Payload data
    #   * CRC checksum
    packet_data = np.fromstring('~%s%s%s%s%s~' % (command, chr(len(payload)),
                                                payload, chr((crc >> 8) & 0x0FF),
                                                chr(crc & 0x0FF)), dtype='uint8')
    packet = parser.parse(packet_data)
    print 'command: %s' % hex(packet.command)
    print 'payload: "%s"' % packet.data()
    try:
        print 'CRC matched: %s' % hex(packet.crc)
    except RuntimeError:
        pass


def parse_args():
    """Parses arguments, returns (options, args)."""
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Create packet and parse using '
                            '`PacketParser`.')
    parser.add_argument(dest='command_byte')
    parser.add_argument(dest='payload')
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    main()
