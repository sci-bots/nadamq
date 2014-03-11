from nose.tools import ok_, eq_
from PacketParser import (cPacketParser, get_packet_data, PACKET_NAME_BY_TYPE,
                          PACKET_TYPES)


def parse_demo(command_byte, payload, crc_checksum=False):
    packet_data = get_packet_data(command_byte, payload, crc_checksum)
    parser = cPacketParser()
    packet = parser.parse(packet_data)
    print 'packet_type: %s' % PACKET_NAME_BY_TYPE[packet.type_]
    print 'command: %s' % hex(packet.command)
    print 'payload: "%s"' % packet.data()
    if crc_checksum:
        try:
            print 'CRC matched: %s' % hex(packet.crc)
        except RuntimeError:
            pass
    return packet


def main():
    args = parse_args()

    parse_demo(args.command_byte, args.payload, args.crc)


def test_parse_request():
    packet = parse_demo('0x80', 'hello')
    eq_(packet.type_, PACKET_TYPES.REQUEST)
    eq_(packet.command, 0)
    eq_(packet.data(), 'hello')

    packet = parse_demo('0xFF', 'hello', crc_checksum=True)
    eq_(packet.type_, PACKET_TYPES.REQUEST)
    eq_(packet.command, 0x7F)
    eq_(packet.data(), 'hello')


def test_parse_response():
    packet = parse_demo('0x00', 'hello')
    eq_(packet.type_, PACKET_TYPES.RESPONSE)
    eq_(packet.command, 0x00)
    eq_(packet.data(), 'hello')

    packet = parse_demo('0x79', 'blah')
    eq_(packet.type_, PACKET_TYPES.RESPONSE)
    eq_(packet.command, 0x79)
    eq_(packet.data(), 'blah')


def parse_args():
    """Parses arguments, returns (options, args)."""
    from argparse import ArgumentParser
    parser = ArgumentParser(description='Create packet and parse using '
                            '`PacketParser`.')
    parser.add_argument(dest='command_byte')
    parser.add_argument(dest='payload')
    parser.add_argument('--crc', action='store_true', default=False)
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    main()
