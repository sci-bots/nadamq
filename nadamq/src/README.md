Packet parser
=============

The packet parsing code in `packet_actions.cpp` is generated using
the files `packet_actions.rl` and `packet.rl`, and the [`ragel`][1]
code-generator.

To generate the contents of `packet_actions.cpp` using `ragel`, run the
following command:

    ragel -o packet_actions.cpp packet_actions.rl


## Compiling for speed or for reduced binary size ##

Note that `ragel` can be instructed to generate one of several different
implementations of the C++ code, including:

    -T0                  Table driven FSM (default)
    -T1                  Faster table driven FSM
    -F0                  Flat table driven FSM
    -F1                  Faster flat table-driven FSM
    -G0                  Goto-driven FSM
    -G1                  Faster goto-driven FSM
    -G2                  Really fast goto-driven FSM
    -P<N>                N-Way Split really fast goto-driven FSM

The listed implementations provide trade-offs between parsing speed and code
size.
