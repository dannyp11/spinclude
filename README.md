# spinclude - A Tarjan Algorithm Implementation for Finding Circular Included Header Files in C/C++ Projects

[![Build Status](https://travis-ci.org/dannyp11/spinclude.svg?branch=master)](https://travis-ci.org/dannyp11/spinclude)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Circular included headers are dangerous that they can cause linking error while compiling. This tool will help detect those files on the whole project without compiling.

#### Features

 - Supports generating & reading project configuration
 - Detects excluding system headers
 - Google unit test
 - Verbose mode for more detailed report


#### Requirements

 * C++11 compiler
 * Linux system (tested on Ubuntu 16 & Fedora 25)
 

#### Usage

```
spinclude [options] [dir1 dir2...]

if [dir1 dir2...] is blank, use current directory as project dir
  Option:
    -c {cfg file}   use project config file
    -g              generate config file Project.cfg
    -v              verbose mode
```

##### Sample outputs
Run on [spinclude](https://github.com/dannyp11/spinclude):
```
Config data:
--------------------------------------------------------------------------------
Project dir: 
  /home/user/Projects/spinclude
Exclude dir: 
- /usr/include
- /usr/include/linux
- /usr/lib/include
Exclude file: 
-- stdio.h
-- stdlib.h
--------------------------------------------------------------------------------
[WARNING] Found duplicates for 12 header basenames
[WARNING] Tossed out 11 nonexisted included header files
[WARNING] Warning code 12 while getting input headers
Processed 59 header files
================================================================================
++ Found 1 circle(s):
   "1file1.hpp" "1file2.hpp" "file1.hpp" 
================================================================================
```

Run on [Google protobuf](https://github.com/google/protobuf):
```
Config data:
--------------------------------------------------------------------------------
Project dir: 
  /home/user/Projects/protobuf
Exclude dir: 
- /usr/include
- /usr/include/linux
- /usr/lib/include
Exclude file: 
-- stdio.h
-- stdlib.h
--------------------------------------------------------------------------------
[WARNING] Found duplicates for 7 header basenames
[WARNING] Tossed out 41 nonexisted included header files
[WARNING] Warning code 12 while getting input headers
Processed 302 header files
================================================================================
-- No circle found
================================================================================
```

### Tarjan util
Also, there packed a utility for solving simple input graph, called tarjan-util
```
Usage: ./tarjan-util [options] [file1 file2...]
  Option:
    -v  verbose mode
```

Sample file content:
```
  a b c
  b d
  d e
  e b a
```
 That means the graph is a can go to b & c, b can go to d, and so on

### Stuffs that helped create this project:

 - Tarjan algorithm: [Wikipedia article](https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm), [Tutorial](https://www.youtube.com/watch?v=ju9Yk7OOEb8)
 - Coding style: [mcastit - multicast test tool](https://github.com/dannyp11/mcastit)
