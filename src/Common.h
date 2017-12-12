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
#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <iostream>
#include <set>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <algorithm>

using std::map;
using std::set;
using std::vector;
using std::string;
using std::shared_ptr;
using std::cout;
using std::endl;
using std::stringstream;

// print out error message to stderr
#define LOG_ERROR(msg) \
    do {if (!Common::isDebugMode()) std::cerr << "[ERROR] " << msg << std::endl;\
        else std::cerr << __FILE__ << ":" << __LINE__ << "-(" \
        <<__func__ << ") [ERROR] " << msg << std::endl;\
    } while(0)

#define LOG_WARN(msg) \
    do {if (!Common::isDebugMode()) std::cerr << "[WARNING] " << msg << std::endl;\
        else std::cerr << __FILE__ << ":" << __LINE__ << "-(" \
        <<__func__ << ") [WARNING] " << msg << std::endl;\
    } while(0)

#define LOG_DEBUG(msg) \
    do {if (Common::isDebugMode()) std::cerr << __FILE__ << ":" \
        << __LINE__ << "-(" <<__func__ << ") [debug] " << msg << std::endl;\
        else if (Common::isVerboseMode()) std::cout << "[verbose] " << msg << std::endl;\
    } while(0)

namespace Common
{
/**
 * Getter/setter for debug mode
 */
void setDebugMode(bool enable = true);
bool isDebugMode();

/**
 * Getter/setter for verbose mode
 * @param dirPath
 * @return
 */
void setVerboseMode(bool enable = true);
bool isVerboseMode();

/**
 * Check for existence of file/dir path
 */
bool isDirExist(const string& dirPath);
bool isFileExist(const string& filePath);

/**
 * Wrappers
 */
const string& getBaseName(const string& path);
const string& getDirName(const string& path);
const string& getRealPath(const string& path);

/**
 * Print out separator, levels can be 1,2
 */
void printSeparatorFd(unsigned level, FILE* fd);
void printSeparator(unsigned level = 1, bool isVerboseModeOnly = false);
}

#endif /* SRC_COMMON_H_ */
