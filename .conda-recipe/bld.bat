@echo off
REM Generate `setup.py` from `pavement.py` definition.
"%PYTHON%" -m paver generate_setup
setlocal enableextensions
md "%PREFIX%"\Library\include\Arduino\NadaMQ
endlocal

REM **Workaround** `conda build` runs a copy of `setup.py` named
REM `conda-build-script.py` with the recipe directory as the only argument.
REM This causes paver to fail, since the recipe directory is not a valid paver
REM task name.
REM
REM We can work around this by wrapping the original contents of `setup.py` in
REM an `if` block to only execute during package installation.
"%PYTHON%" -c "from __future__ import print_function; input_ = open('setup.py', 'r'); data = input_.read(); input_.close(); output_ = open('setup.py', 'w'); output_.write('\n'.join(['import sys', 'import path_helpers as ph', '''if ph.path(sys.argv[0]).name == 'conda-build-script.py':''', '    sys.argv.pop()', 'else:', '\n'.join([('    ' + d) for d in data.splitlines()])])); output_.close(); print(open('setup.py', 'r').read())"

"%PYTHON%" version.py
"%PYTHON%" -m paver sdist

copy nadamq\src\Arduino\packet_handler\output_buffer.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\Arduino\packet_handler\packet_handler.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\Arduino\packet_handler\NadaMQ.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\BufferAllocator.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\Packet.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketAllocator.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketHandler.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketParser.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketSocket.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketSocketEvents.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketStream.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\PacketWriter.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\SimpleCommand.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\StreamPacketParser.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\crc-16.c "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\crc-16.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\crc_common.cpp "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\crc_common.h "%PREFIX%"\Library\include\Arduino\NadaMQ
copy nadamq\src\packet_actions.cpp "%PREFIX%"\Library\include\Arduino\NadaMQ


REM Install source directory as Python package.
cd dist
for /f %%i in ('dir /B *.tar.gz') do set DIST_FILE=%%i
"%PYTHON%" -m pip install --no-cache %DIST_FILE%
if errorlevel 1 exit 1
