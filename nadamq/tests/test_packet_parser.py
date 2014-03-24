from nose.tools import ok_, eq_
from nadamq.NadaMq import (cPacketParser, get_packet_data, PACKET_NAME_BY_TYPE,
                           PACKET_TYPES)


def parse_demo(interface_unique_id, packet_type, payload):
    packet_data = get_packet_data(interface_unique_id, packet_type, payload)
    parser = cPacketParser()
    packet = parser.parse(packet_data)
    print 'packet_type: %s' % PACKET_NAME_BY_TYPE[packet.type_]
    print 'command: %s' % hex(packet.iuid)
    print 'payload: "%s"' % packet.data()
    print 'CRC matched: %s' % hex(packet.crc)
    return packet


def main():
    args = parse_args()

    parse_demo(args.command_byte, args.payload, args.crc)


def test_parse_request():
    packet = parse_demo(97, PACKET_TYPES.REQUEST, 'hello')
    eq_(packet.type_, PACKET_TYPES.REQUEST)
    eq_(packet.iuid, 97)
    eq_(packet.data(), 'hello')

    packet = parse_demo(10, PACKET_TYPES.REQUEST, 'world')
    eq_(packet.type_, PACKET_TYPES.REQUEST)
    eq_(packet.iuid, 10)
    eq_(packet.data(), 'world')


def test_parse_response():
    packet = parse_demo(71, PACKET_TYPES.RESPONSE, 'foo')
    eq_(packet.type_, PACKET_TYPES.RESPONSE)
    eq_(packet.iuid, 71)
    eq_(packet.data(), 'foo')

    packet = parse_demo(32, PACKET_TYPES.RESPONSE, 'bar')
    eq_(packet.type_, PACKET_TYPES.RESPONSE)
    eq_(packet.iuid, 32)
    eq_(packet.data(), 'bar')


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
