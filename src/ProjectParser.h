/*
 * MIT License
 * 
 * Copyright (c) 2017 Dat
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef SRC_PROJECTPARSER_H_
#define SRC_PROJECTPARSER_H_

#include "DataStructure.h"

namespace ProjectParser
{
  /// map<header> = set<header path>
  typedef map<string,set<string> > HeaderLocationMap;

  /**
   * This is the C/C++ header parser
   * @param parseDirs     Input: set of dirs to search recursively
   * @param excludedFiles Input: set of excluded files
   * @param output        Output: Graph of all nodes of header files,
   *                              child nodes are included files
   * @param detailOutput  Output: Graph of all nodes of header files that
   *                              has unique path
   * @param outputLocationMap Output: relative path to header files in output
   *                          ideally set<header path> should have size 1
   * @return 0 on success, other err code are bitwise updated
   *         1 if 1 of parseDirs not exists
   *         2 if no headers in all dirs
   *         4 if one included file isn't found
   *         8 if found duplicate basename of headers
   *         <0 on critical error
   */
  int parse(const set<string>& parseDirs, const set<string>& excludedFiles,
      Graph& output, Graph& detailOutput, HeaderLocationMap& outputLocationMap);

  /**
   * Recursively get header files inside dirs
   * @param headerFiles - OUTPUT - relative path to dirs
   * @return 0 on success, <0 on error
   */
  int generateHeaderList(const set<string>& dirs, set<string>& headerFiles);
};

#endif /* SRC_PROJECTPARSER_H_ */
