from nose.tools import eq_
from nadamq.NadaMq import cPacketParser, compute_crc16, PACKET_TYPES


def test_parse_data():
    parser = cPacketParser()
    packet = parser.parse(create_data_packet_bytes(97, 'hello'))
    eq_(packet.type_, PACKET_TYPES.DATA)
    eq_(packet.iuid, 97)
    eq_(packet.crc, compute_crc16('hello'))
    eq_(packet.data(), 'hello')

    packet = parser.parse(create_data_packet_bytes(10, 'world'))
    eq_(packet.type_, PACKET_TYPES.DATA)
    eq_(packet.iuid, 10)
    eq_(packet.crc, compute_crc16('world'))
    eq_(packet.data(), 'world')


def test_parse_ack():
    parser = cPacketParser()
    packet = parser.parse(create_ack_packet_bytes(1231))
    eq_(packet.type_, PACKET_TYPES.ACK)
    eq_(packet.iuid, 1231)


def test_parse_nack():
    parser = cPacketParser()
    packet = parser.parse(create_nack_packet_bytes(4328))
    eq_(packet.type_, PACKET_TYPES.NACK)
    eq_(packet.iuid, 4328)

    packet = parser.parse(create_nack_packet_bytes(128, max_packet_length=13))
    eq_(packet.type_, PACKET_TYPES.NACK)
    eq_(packet.iuid, 128)
