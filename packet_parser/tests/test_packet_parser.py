from PacketParser import cPacketParser, get_packet_data


def parse_demo(command_byte, payload, crc_checksum=False):
    packet_data = get_packet_data(command_byte, payload, crc_checksum)
    parser = cPacketParser()
    packet = parser.parse(packet_data)
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


def test_parse_pass():
    parse_demo('0x80', 'hello')
    parse_demo('0xFF', 'hello', crc_checksum=True)


def test_parse_fail():
    try:
        # This test should fail because the command is not within the range
        # `[0x80..0xFF]`.
        parse_demo('0x00', 'hello')
    except RuntimeError, e:
        assert('Error parsing packet:' in str(e))
    else:
        raise RuntimeError

    try:
        # This test should fail because the command is not within the range
        # `[0x80..0xFF]`.
        parse_demo('0x79', 'blah')
    except RuntimeError, e:
        assert('Error parsing packet:' in str(e))
    else:
        raise RuntimeError


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
