import sys
import re
from cStringIO import StringIO

from path_helpers import path


def generate_ragel_events_header(input_path, output):
    print >> output, '''\
#ifndef ___PACKET_SOCKET_EVENTS__HPP___
#define ___PACKET_SOCKET_EVENTS__HPP___

#include <string>

inline std::string event_label(uint8_t event) {
  switch (event) {
'''

    with open('packet_socket_fsm.rl', 'rb') as input_file:
        cre_event = re.compile(r"^\s+(?P<label>\w+).*(?P<char>'.');",
                               re.MULTILINE)
        for label, char in cre_event.findall(input_file.read()):
            print >> output, '''    case %s: return "%s";''' % (char, label)

    print >> output, '''
    default: return "<unknown event>";
  }
}

#endif  // #ifndef ___PACKET_SOCKET_EVENTS__HPP___
'''
    return output


def parse_args(argv=None):
    """Parses arguments, returns (options, args)."""
    from argparse import ArgumentParser

    if argv is None:
        argv = sys.argv[1:]

    parser = ArgumentParser(description='Generate a C++ header containing '
                            'labels for event characters from a Ragel FSM '
                            'definition.')
    parser.add_argument('-f', '--force_overwrite', action='store_true',
                        default=False)
    parser.add_argument('-o', '--output_path', type=path, default=None)
    parser.add_argument(dest='input_file', type=path)
    args = parser.parse_args(argv)
    return args


if __name__ == '__main__':
    args = parse_args()
    if args.output_path is None:
        output = StringIO()
    else:
        if args.output_path.isfile() and not args.force_overwrite:
            # The output-path exists, but `overwrite` was not enabled, so raise
            # exception.
            raise IOError('Output path `%s` already exists.  Specify '
                          '`overwrite=True` to force overwrite.' %
                          args.output_path)
        output = args.output_path.open('wb')
    try:
        generate_ragel_events_header(args.input_file, output)
    finally:
        if args.output_path is not None:
            output.close()
