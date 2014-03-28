/* TODO
 * ====
 *
 * Use [`Connection` state chart][1] as example for implementing packet socket
 * state-machine.
 *
 * [1]: http://www.zedshaw.com/essays/ragel_state_charts.html */
%%{
    machine packet_socket_fsm;

    # alphtype unsigned char;
    alphtype char;

    ack_packet          = 'a';
    data_packet         = 'd';
    data_too_large      = 'L';
    done                = 'D';
    initialize          = 'i';
    nack_packet         = 'n';
    packet_found        = 'Y';
    packet_incomplete   = 'c';
    packet_not_found    = 'N';
    packet_ready        = 'r';
    packet_sent         = 's';
    parse_error         = 'e';
    queued_ok           = 'q';
    queue_full          = 'f';
    rx_packet_queued    = 'I';
    rx_stream_available = 'A';
    tx_packet_queued    = 'O';

    PacketSocket = (
        start: (
            initialize @initialize -> Idle
        ),

        Idle: (
            rx_stream_available @read_stream -> ParsingStream |
            rx_packet_queued @recv -> Servicing |
            tx_packet_queued @send -> Sending |
            done @cleanup -> final
        ),

        ParsingStream: (
            # When reading from the stream, we finished a packet, so update the
            # incoming packet queue.
            packet_ready @update_rx_queue -> Idle |
            # The packet payload is too large for our packet buffers.
            data_too_large @data_too_large -> ServicingError |
            # Our incoming packet queue is full.
            queue_full @queue_full -> ServicingError |
            # There was an error parsing from the stream.
            parse_error @parse_error -> ServicingError |
            # Packet is not complete yet, so return to `Idle` state.
            packet_incomplete -> Idle
        ),

        Servicing: (
            ack_packet @handle_ack -> ProcessAck |
            nack_packet @handle_nack -> ProcessNack |
            data_packet @handle_data -> ProcessData |
            packet_not_found -> Idle
        ),

        ServicingError: (
            # The packet payload is too large for our packet buffers.
            data_too_large @queue_nack_data_too_large -> Idle |
            # Our incoming packet queue is full.
            queue_full @queue_nack_queue_full -> Idle |
            # There was a general error parsing from the stream.
            parse_error @queue_nack_parse_error -> Idle
        ),

        Sending: (
            # A packet was sent successfully.
            packet_sent @sent -> Idle |
            packet_not_found -> Idle
        ),

        ProcessAck: (
            # Packet that was acknowledged is still on output queue.  Since the
            # receiver has acknowledged receipt, we can pop it off the queue.
            packet_found @pop_packet -> Idle |
            # Packet that was acknowledged was not found, so there is nothing
            # to do.
            packet_not_found -> Idle
        ),

        ProcessNack: (
            # The receiver reported a parse error _(e.g., checksum mismatch)_.
            #  - Queue the packet to be resent _(if possible)_.
            parse_error @requeue_packet -> Idle |

            # The receiver reported that it could not receive the packet,
            # because there was not enough buffer space available at the time
            # of receipt.
            #  - Queue the packet to be resent _(if possible)_.
            #
            # TODO
            # ====
            #
            # We might want to add a delay before trying to resend. Perhaps we
            # can add delays to packets in the transmission queue?
            #  - Probably overkill for now...
            queue_full @requeue_packet -> Idle |

            # The receiver reported that the packet was too large to fit in its
            # buffer.
            #  - Split the packet into smaller packets and queue the resulting
            #    packets for sending.
            data_too_large @split_and_queue_packet -> Idle
        ),

        ProcessData: (
            # The data packet was successfully added to the data queue.
            queued_ok @queue_ack -> Idle
        )
    );

    #main := (PacketSocket ${ printf("\n"); })*;
#    main := (PacketSocket $!{ fgoto Idle; } )*;
    main := (PacketSocket )*;
}%%
