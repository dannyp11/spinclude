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
#include <libgen.h>
#include <string.h>
#include <algorithm>
#include <fstream>

static const set<string> HEADER_EXTENSIONS = {".h", ".hpp"};

bool is_header_file(const string& path, bool checkForExist = true)
{
  if (checkForExist && !Common::isFileExist(path))
  {
    return false;
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
  const string fileName = Common::getBaseName(filePath);
  Node fileNode(fileName);

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
      if (excludedFiles.end() == excludedFiles.find(includedHeader))
      {
        // Only add if not found in excluded set
        // Also, don't put files that don't have .h or .hpp
        // in because it's clearly system include files
        if (is_header_file(includedHeader, false))
        {
          includedHeader = Common::getBaseName(includedHeader);
          fileNode.childNodes.insert(includedHeader);
          LOG_DEBUG(fileName << " -> " << includedHeader);
        }
      }
    }
  }
  fclose(file);

  // Finally update output graph
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
  if (!Common::isDirExist(dirPath))
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

      if (Common::isDirExist(itemPath))
      {
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
        if (excludedFiles.end() == excludedFiles.find(Common::getBaseName(itemPath)))
        {
          process_header_file(itemPath, excludedFiles, output);
        }
        else
        {
          continue;
        }
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
  Graph tmpOutput;
  for (const string& dirName: parseDirs)
  {
    int helperRetval = parse_one_dir(dirName, excludedFiles, tmpOutput);
    if (0 > helperRetval)
    {
      // Only stop if we hit critical error
      retVal |= helperRetval;
      break;
    }
    else if (0 < helperRetval)
    {
      retVal = helperRetval;
      continue;
    }
  }

  // Check for non existence included file
  map<string, Node> idMap; // map[id] = node
  for (const auto & node : tmpOutput)
  {
    idMap.insert(std::make_pair(node.id, node));
  }

  for (auto node: tmpOutput)
  {
    for (const string& childId : node.childNodes)
    {
      if (idMap.end() == idMap.find(childId))
      {
        LOG_WARN("Can't find " << childId
            << " for " << node.id
            << ", tossing it out");

        node.childNodes.erase(childId);
      }
    }

    output.insert(node);
  }

  if (output.empty())
  {
    retVal |= 2;
  }

  return retVal;
}

int generateHeaderList_helper(const string& dirPath, set<string>& headerFiles)
{
  int retVal = 0;

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

      if (Common::isDirExist(itemPath))
      {
        retVal += generateHeaderList_helper(itemPath, headerFiles);
      }
      else if (is_header_file(itemPath))
      {
        headerFiles.insert(itemPath);
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

int ProjectParser::generateHeaderList(const set<string>& dirs, set<string>& headerFiles)
{
  int retVal = 0;
  headerFiles.clear();

  for (const string& dirPath : dirs)
  {
    set<string> fullPathHeaders = {};
    if (Common::isDirExist(dirPath))
    {
      retVal += generateHeaderList_helper(dirPath, fullPathHeaders);
    }

    // Make header files relative path
    for (string header : fullPathHeaders)
    {
      if (header.size() < dirPath.size())
      {
        continue;
      }
      else if (0 == strncmp(header.c_str(), dirPath.c_str(), dirPath.size()))
      {
        // Remove dirPath in header
        header = header.substr(dirPath.size(), header.size() - dirPath.size());

        // Remove leading / in header
        while (!header.empty() && header[0] == '/')
        {
          header = header.substr(1, header.size() - 1);
        }
      }

      // Finally update output
      headerFiles.insert(header);
    }
  }

  return retVal;
}

