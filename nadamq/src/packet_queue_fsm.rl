%%{
    machine packet_socket_fsm;

    # alphtype unsigned char;
    alphtype char;

    action handle_ack {}
    action handle_data {}
    action handle_nack {}
    action parse_data {}
    action pop_packet {}
    action push_packet {}
    action send_ack {}
    action send_nack {}
    action send_packet {}

    ack_packet            = 'A';
    alloc_queue           = 'Q';
    data_available        = 'a';
    data_packet           = 'D';
    done                  = 'd';
    listen                = 'L';
    nack_packet           = 'N';
    output_ready          = 'r';
    packet_complete       = 'C';
    packet_found          = 'f';
    packet_send           = 'S';
    queue_ready           = 'R';
    queue_full            = 'F';
    queue_space_available = 's';

    PacketSocket = (
        start: (
            alloc_queue -> AllocQueue
        ),

        AllocQueue: (
            queue_ready -> QueueReady
        ),

        QueueReady: (
            listen -> Listening
        ),

        Listening: (
            output_ready -> SendPacket |
            data_available -> ReadPacket |
            done -> final
        ),

        SendPacket: (
            # There is a packet in the output queue, ready to send.
            packet_send @send_packet -> Listening
        ),

        ReadPacket: (
            data_available @parse_data -> ReadPacket |
            packet_complete -> ProcessPacket
        ),

        ProcessPacket: (
            ack_packet @handle_ack -> ProcessAck |
            nack_packet @handle_nack -> Listening |
            data_packet @handle_data -> ProcessData
        ),

        ProcessAck: (
            # Packet that was acknowledged is still on output queue.  Since the
            # receiver has acknowledged receipt, we can pop it off the queue.
            packet_found @pop_packet -> Listening
        ),

        ProcessData: (
            # Packet that was acknowledged is still on output queue.  Since the
            # receiver has acknowledged receipt, we can pop it off the queue.
            queue_space_available @send_ack @push_packet -> Listening |
            queue_full @send_nack -> Listening
        )
    );

    main := (PacketSocket ${ printf("\n"); })*;
}%%
