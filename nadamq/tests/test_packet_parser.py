from nose.tools import eq_
from nadamq.NadaMq import (cPacket, parse_from_string, compute_crc16,
                           PACKET_TYPES)


def test_parse_data():
    a = cPacket(iuid=1234, type_=PACKET_TYPES.DATA, data='hello')
    b = parse_from_string(a.tostring())

    eq_(a.type_, b.type_)
    eq_(a.iuid, b.iuid)
    eq_(a.crc, b.crc)
    eq_(a.data(), b.data())


def test_parse_ack():
    a = cPacket(iuid=1010, type_=PACKET_TYPES.ACK)
    b = parse_from_string(a.tostring())

    eq_(a.type_, b.type_)
    eq_(a.iuid, b.iuid)


def test_parse_nack():
    a = cPacket(iuid=4321, type_=PACKET_TYPES.NACK)
    b = parse_from_string(a.tostring())

    eq_(a.type_, b.type_)
    eq_(a.iuid, b.iuid)
