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
#ifndef SRC_CONFIGFILE_H_
#define SRC_CONFIGFILE_H_

#include "Common.h"

struct ConfigData
{
  set<string> projDirs;
  set<string> excludedDirs;
  set<string> excludedFiles;

  ConfigData();
  void dump(FILE* fd) const;
};

/**
 * Project cfg file parser
 */
class ConfigFile
{
public:
  ConfigFile(const string& cfgFilePath);
  virtual ~ConfigFile();

  /**
   * Parse data in cfg file
   * @return true on success
   */
  bool parse();

  /**
   * Get parsed data
   */
  const ConfigData& data() const;

private:
  /**
   * populate mParsedRawData
   */
  bool parseRawData_();

  /**
   * get data from mParsedRawData
   */
  const set<string>& getFromRawData(const string& key,
                                    const set<string>& defaultVals);

private:
  bool mParseSuccess;
  string mCfgFilePath;
  ConfigData mData;

  map<string, set<string>> mParsedRawData; // map[cfg item] = set<values>
};

#endif /* SRC_CONFIGFILE_H_ */
