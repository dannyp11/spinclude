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
#include "Common.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

static bool g_isDebugMode = false;
static bool g_isVerboseMode = false;

void Common::setDebugMode(bool enable)
{
  g_isDebugMode = enable;
}

bool Common::isDebugMode()
{
  return g_isDebugMode;
}

void Common::setVerboseMode(bool enable)
{
  g_isVerboseMode = enable;
}

bool Common::isVerboseMode()
{
  return g_isVerboseMode;
}

bool Common::isDirExist(const string& dirPath)
{
  struct stat sb;
  return stat(dirPath.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
}

bool Common::isFileExist(const string& filePath)
{
  struct stat path_stat;
  stat(filePath.c_str(), &path_stat);
  return S_ISREG(path_stat.st_mode);
}

const string& Common::getBaseName(const string& path)
{
  static string retVal;
  static char file_path[PATH_MAX];
  strcpy(file_path, path.c_str());
  retVal = basename(file_path);
  return retVal;
}

const string& Common::getRealPath(const string& path)
{
  static string retVal;
  static char file_path[PATH_MAX];
  if (nullptr != realpath(path.c_str(), file_path))
  {
    retVal = file_path;
  }
  else
  {
    LOG_WARN("Realpath for " << path << ": " << strerror(errno));
    retVal = path;
  }
  return retVal;
}

const string& Common::getDirName(const string& path)
{
  static string retVal;
  static char file_path[PATH_MAX];
  strcpy(file_path, path.c_str());
  retVal = dirname(file_path);
  return retVal;
}
