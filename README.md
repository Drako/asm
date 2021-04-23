# libasm

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A little embeddable x86_64 assembler I intend to use for JIT compilation.

## Generation

As I'm too lazy to write code for all the instructions manually, I created a Python 3 script grabbing instruction infos
from
[Félix Cloutier's online x86 reference](https://www.felixcloutier.com/x86/).

Should the reference get updated, one can regenerate the JSON files in *spec*
like this:
```shell
cd specs
python3 sync.py
```
