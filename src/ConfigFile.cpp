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
#include "ConfigFile.h"
#include <fstream>
#include <algorithm>

ConfigFile::ConfigFile(const string& cfgFilePath): mCfgFilePath(cfgFilePath)
{
  mParseSuccess = false;
}

ConfigFile::~ConfigFile()
{
  // TODO Auto-generated destructor stub
}

bool ConfigFile::parse()
{
  if (!Common::isFileExist(mCfgFilePath))
  {
    return false;
  }

  mParseSuccess = parseRawData_();

  if (mParseSuccess)
  {
    const ConfigData defaultData;
    mData.projDirs = getFromRawData("PROJECT_DIRS", defaultData.projDirs);
    mData.excludedDirs = getFromRawData("EXCLUDE_DIRS", defaultData.excludedDirs);
    mData.excludedFiles = getFromRawData("EXCLUDE_FILES", defaultData.excludedFiles);
  }

  return mParseSuccess;
}

const ConfigData& ConfigFile::data() const
{
  return mData;
}

bool ConfigFile::parseRawData_()
{
  std::ifstream cfgFile(mCfgFilePath);
  string line;
  int lineNumber = 0;
  while (std::getline(cfgFile, line))
  {
    // First rm all spaces
    ++lineNumber;
    line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

    // rm everything after the first #
    auto foundCmt = line.find('#');
    if (foundCmt != string::npos)
    {
      line = line.substr(0, foundCmt);
    }

    // Ignore empty line
    if (line.empty())
    {
      continue;
    }

    // Make sure there's = in line
    auto foundEq = line.find('=');
    if (foundEq == string::npos)
    {
      LOG_ERROR("Error Parsing " << mCfgFilePath << " line " << lineNumber
          << " : no '=' found");
      return false;
    }

    // Now get key and values
    string key = line.substr(0, foundEq-1);
    string values = line.substr(foundEq+1, line.size() - foundEq -1);

    // Split up values to a set of values
    set<string> valueSet;
    if (values.size() > 0)
    {
      std::istringstream valueSS(values);
      string valToken;
      while (std::getline(valueSS, valToken, ','))
      {
        valueSet.insert(valToken);
      }
    }

    // Finally add to map
    mParsedRawData[key] = valueSet;
  }

  return true;
}

const set<string>& ConfigFile::getFromRawData(const string& key, const set<string>& defaultVals)
{
  if (mParsedRawData.end() != mParsedRawData.find(key))
  {
    return mParsedRawData[key];
  }
  return defaultVals;
}

ConfigData::ConfigData()
{
  // Populate default values
  projDirs = {"dir1", "dir2"};
  excludedFiles = {"stdio.h", "stdlib.h"};
  excludedDirs = {"/usr/include", "/usr/lib/include", "/usr/include/linux"};
}

void ConfigData::dump(FILE* fd) const
{
  if (!projDirs.empty())
  {
    fprintf(fd, "Project dir: \n");
    for (const string& dir:projDirs)
    {
      fprintf(fd, "  %s\n", Common::getRealPath(dir).c_str());
    }
    fprintf(fd, "\n");
  }
  if (!excludedDirs.empty())
  {
    fprintf(fd, "Exclude dir: \n");
    for (const string& dir : excludedDirs)
    {
      fprintf(fd, "- %s\n", dir.c_str());
    }
    fprintf(fd, "\n");
  }
  if (!excludedFiles.empty())
  {
    fprintf(fd, "Exclude file: \n");
    for (const string& file : excludedFiles)
    {
      fprintf(fd, "-- %s\n", file.c_str());
    }
  }
}
