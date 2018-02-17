# nadamq

Low overhead packets with checksum validation.

API includes `nadamq.NadaMq.cPacket` type and `nadamq.NadaMq.cPacketParser`.

See [`nadamq/tests`](nadamq/tests) for example usage.


Build
=====

Install `conda-build`:

    conda install "conda-build>=3.4.2"

Build Python 2.7 and Python 3 packages:

    conda build .conda-recipe -m .conda-recipe/variants.yaml


Install
=======

Install the latest release from the [`sci-bots` Conda channel][sci-bots] using:

    conda install -c sci-bots nadamq


[sci-bots]: https://anaconda.org/sci-bots
