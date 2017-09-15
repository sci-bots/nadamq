from nadamq.NadaMq import cPacket, parse_from_string, PACKET_TYPES
import nose.tools as nt


def test_parse_data():
    a = cPacket(iuid=1234, type_=PACKET_TYPES.DATA, data='hello')
    b = parse_from_string(a.tostring())

    nt.eq_(a.type_, b.type_)
    nt.eq_(a.iuid, b.iuid)
    nt.eq_(a.crc, b.crc)
    nt.eq_(a.data(), b.data())


def test_parse_ack():
    a = cPacket(iuid=1010, type_=PACKET_TYPES.ACK)
    b = parse_from_string(a.tostring())

    nt.eq_(a.type_, b.type_)
    nt.eq_(a.iuid, b.iuid)


def test_parse_nack():
    a = cPacket(iuid=4321, type_=PACKET_TYPES.NACK)
    b = parse_from_string(a.tostring())

    # TODO Should parse fail with `NACK` type?  It currently does.
    nt.assert_false(b)


def test_parse_id_response():
    '''
    .. versionadded:: 0.13
    '''
    a = cPacket(iuid=1234, type_=PACKET_TYPES.ID_RESPONSE,
                data='{"id": "my device name"}')
    b = parse_from_string(a.tostring())

    nt.eq_(a.type_, b.type_)
    nt.eq_(a.iuid, b.iuid)
    nt.eq_(a.crc, b.crc)
    # nt.eq_(a.data(), b.data())


def test_parse_id_request():
    '''
    .. versionadded:: 0.13
    '''
    a = cPacket(iuid=1234, type_=PACKET_TYPES.ID_REQUEST)
    b = parse_from_string(a.tostring())

    nt.eq_(a.type_, b.type_)
    nt.eq_(a.iuid, b.iuid)
