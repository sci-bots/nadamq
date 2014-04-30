import re

from simple_pb2 import *


# Function to convert from camel-case to underscore separated.  Taken from
# [here][1].
#
# [1]: https://djangosnippets.org/snippets/585/
def camelcase_to_underscore(value):
    return re.sub('(((?<=[a-z])[A-Z])|([A-Z](?![A-Z]|$)))', '_\\1',
                  value).lower().strip('_')


# Collect the names of all request types.
request_camelcase = [k[:-len('Request')]
                     for k in locals().keys()
                     if k != 'CommandRequest' and k.endswith('Request')]


def get_request_from_camelcase(camelcase_request_name, **kwargs):
    lowercase = camelcase_to_underscore(camelcase_request_name)
    kwargs.update({lowercase: eval(camelcase_request_name + 'Request')(),
                   'type': CommandType.Value(lowercase.upper())})
    return CommandRequest(**kwargs)


__all__ = ['request_camelcase', 'get_request_from_camelcase',
           'camelcase_to_underscore', 'CommandResponse']
