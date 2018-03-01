@echo off
REM Generate `setup.py` from `pavement.py` definition.
"%PYTHON%" -m paver generate_setup
setlocal enableextensions
md "%PREFIX%"\Library\include\Arduino\NadaMQ
endlocal

REM Generate `packet_actions.cpp` from ragel state machine source.
"%PYTHON%" -m paver copy_packet_actions
REM Install source directory as Python package.
"%PYTHON%" setup.py install --single-version-externally-managed --record record.txt
if errorlevel 1 exit 1

REM Run build tests
"%PYTHON%" -m paver build_ext --inplace
nosetests -v nadamq\tests
if errorlevel 1 exit 1

REM Copy Arduino library files to Conda Arduino library.
copy "%SP_DIR%"\nadamq\src\Arduino\packet_handler\output_buffer.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\Arduino\packet_handler\packet_handler.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\Arduino\packet_handler\NadaMQ.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\BufferAllocator.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\Packet.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketAllocator.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketHandler.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketParser.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketSocket.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketSocketEvents.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketStream.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\PacketWriter.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\SimpleCommand.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\StreamPacketParser.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\crc-16.cpp "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\crc-16.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\crc_common.cpp "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\crc_common.h "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
copy "%SP_DIR%"\nadamq\src\packet_actions.cpp "%PREFIX%"\Library\include\Arduino\NadaMQ
if errorlevel 1 exit 1
