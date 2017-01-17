# Generate `setup.py` from `pavement.py` definition.
"${PYTHON}" -m paver generate_setup
mkdir -p "${PREFIX}"/include/Arduino/NadaMQ

# **Workaround** `conda build` runs a copy of `setup.py` named
# `conda-build-script.py` with the recipe directory as the only argument.
# This causes paver to fail, since the recipe directory is not a valid paver
# task name.
#
# We can work around this by wrapping the original contents of `setup.py` in
# an `if` block to only execute during package installation.
"${PYTHON}" -c "from __future__ import print_function; input_ = open('setup.py', 'r'); data = input_.read(); input_.close(); output_ = open('setup.py', 'w'); output_.write('\n'.join(['import sys', 'import path_helpers as ph', '''if ph.path(sys.argv[0]).name == 'conda-build-script.py':''', '    sys.argv.pop()', 'else:', '\n'.join([('    ' + d) for d in data.splitlines()])])); output_.close(); print(open('setup.py', 'r').read())"

"${PYTHON}" version.py
# Generate `packet_actions.cpp` from ragel state machine source.
"${PYTHON}" -m paver copy_packet_actions
# Install source directory as Python package.
"${PYTHON}" -m pip install --no-cache .
rc=$?; if [[ $rc != 0  ]]; then exit $rc; fi

# Copy Arduino library files to Conda Arduino library.
cp -ra "${SP_DIR}"/nadamq/src/Arduino/packet_handler/output_buffer.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/Arduino/packet_handler/packet_handler.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/Arduino/packet_handler/NadaMQ.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/BufferAllocator.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/Packet.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketAllocator.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketHandler.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketParser.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketSocket.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketSocketEvents.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketStream.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/PacketWriter.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/SimpleCommand.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/StreamPacketParser.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/crc-16.c "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/crc-16.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/crc_common.cpp "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/crc_common.h "${PREFIX}"/include/Arduino/NadaMQ
cp -ra "${SP_DIR}"/nadamq/src/packet_actions.cpp "${PREFIX}"/include/Arduino/NadaMQ
rc=$?; if [[ $rc != 0  ]]; then exit $rc; fi
