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
#include "ProjectParser.h"
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <algorithm>

static const set<string> HEADER_EXTENSIONS = {".h", ".hpp"};

bool is_dir(const string& path)
{
  struct stat sb;
  if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
  {
    return true;
  }
  return false;
}

bool is_header_file(const string& path, bool checkForExist = true)
{
  if (checkForExist)
  {
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    if (!S_ISREG(path_stat.st_mode))
    {
      // not a file
      return false;
    }
  }

  for (const string& ext : HEADER_EXTENSIONS)
  {
    if (ext.size() > path.size())
    {
      continue;
    }
    else if (std::equal(ext.rbegin(), ext.rend(), path.rbegin()))
    {
      // path ends with ext
      return true;
    }
  }

  return false;
}

void process_header_file(const string& filePath, const set<string>& excludedFiles,
    Graph& output)
{
  static char file_path[PATH_MAX];
  strcpy(file_path, filePath.c_str());
  Node fileNode(basename(file_path));

  // Only the first 2000 lines of file is process for performance
  int lineNum = 0;
  FILE* file = fopen(filePath.c_str(), "r"); /* should check the result */
  char line[1024];

  while (fgets(line, sizeof(line), file) && lineNum++ < 2000)
  {
    // First trim all spaces
    string lineStr = line;
    lineStr.erase(remove_if(lineStr.begin(), lineStr.end(), isspace), lineStr.end());

    // now if there's // at beginning of line, dont do it
    static const string comment = "//";
    if (0 == strncmp(lineStr.c_str(), comment.c_str(), comment.size()))
    {
      continue;
    }

    // here means we can check for #include< or #include"
    static const set<string> includeSig = {"#include<", "#include\""};
    for (const string & sig : includeSig)
    {
      if (0 != strncmp(lineStr.c_str(), sig.c_str(), sig.size()))
      {
        continue;
      }

      // Here means it's include msg, let's get the included header
      const char openBracket = sig[sig.size() - 1];
      const char closeBracket = (openBracket == '<')? '>' : openBracket;
      const size_t foundPos = lineStr.find(closeBracket, sig.size());
      if (foundPos == string::npos)
      {
        // Error parsing
        LOG_DEBUG("Error parsing line <" << line << ">");
        continue;
      }

      string includedHeader = lineStr.substr(sig.size(), foundPos - sig.size());
      static char includedHeaderPath[PATH_MAX];
      strcpy(includedHeaderPath, includedHeader.c_str());

      includedHeader = basename(includedHeaderPath);
      if (excludedFiles.end() == excludedFiles.find(includedHeader))
      {
        // Only add if not found in excluded set
        // Also, don't put files that don't have .h or .hpp
        // in because it's clearly system include files
        if (is_header_file(includedHeader, false))
        {
          fileNode.childNodes.insert(includedHeader);
          LOG_DEBUG("Found included file " << includedHeader);
        }
      }
    }
  }
  fclose(file);

  output.insert(fileNode);
}

/**
 * Recursively find in dirPath for eligible header files
 * @return same as ProjectParser::parse
 */
int parse_one_dir(const string& dirPath, const set<string>& excludedFiles,
    Graph& output)
{
  int retVal = 0;
  // check for existence
  struct stat sb;
  if (false == (stat(dirPath.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)))
  {
    return 1;
  }

  // Iterate thru dir
  DIR *d;
  struct dirent *dir;
  d = opendir(dirPath.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      const string itemName = dir->d_name;
      const string itemPath = dirPath + "/" + itemName;
      if (itemName == "." || itemName == "..")
      {
        continue;
      }

      if (is_dir(itemPath))
      {
        LOG_DEBUG("Found dir " << itemPath);
        const int parseVal = parse_one_dir(itemPath, excludedFiles, output);
        if (parseVal < 0)
        {
          retVal = parseVal;
          break;
        }
        else if (parseVal > 0)
        {
          retVal = parseVal;
          continue;
        }
      }
      else if (is_header_file(itemPath))
      {
        LOG_DEBUG("Found header " << itemPath);
        process_header_file(itemPath, excludedFiles, output);
      }
      else
      {
        continue;
      }

    }
    closedir(d);
  }

  return retVal;
}

int ProjectParser::parse(const set<string>& parseDirs, const set<string>& excludedFiles,
    Graph& output)
{
  int retVal = 0;
  output.clear();
  for (const string& dirName: parseDirs)
  {
    int helperRetval = parse_one_dir(dirName, excludedFiles, output);
    if (0 > helperRetval)
    {
      // Only stop if we hit critical error
      retVal = helperRetval;
      break;
    }
    else if (0 < helperRetval)
    {
      retVal = helperRetval;
      continue;
    }
  }

  return retVal;
}

int ProjectParser::parse(const set<string>& parseDirs, Graph& output)
{
  set<string> emptyExcludedFiles;
  return parse(parseDirs, emptyExcludedFiles, output);
}

int ProjectParser::parse(const string& parseDir, Graph& output)
{
  set<string> oneDir;
  oneDir.insert(parseDir);
  return parse(oneDir, output);
}
