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
#include <string.h>

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
    Graph& output, Graph& detailOutput)
{
  const string fileName = Common::getBaseName(filePath);
  Node fileNode(fileName);
  Node fileRealNode(filePath);

  // Only the first 2000 lines of file is process for performance
  int lineNum = 0;
  FILE* file = fopen(filePath.c_str(), "r");
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
          fileRealNode.childNodes.insert(includedHeader);
        }
      }
    }
  }
  fclose(file);

  // Let's update detail output first before combining duplicates
  detailOutput.insert(fileRealNode);

  // Finally update output graph
  // we will combine child list if duplicate is found
  // this may create false positive in detecting circle
  auto outputNodeIt = output.find(fileNode);
  if (outputNodeIt != output.end())
  {
    fileNode.childNodes.insert(outputNodeIt->childNodes.begin(),
                               outputNodeIt->childNodes.end());
    output.erase(outputNodeIt);
  }
  output.insert(fileNode);

  // Only print node that has child in verbose mode
  if (Common::isDebugMode())
  {
    LOG_DEBUG(fileNode);
  }
  else if (!fileNode.childNodes.empty())
  {
    LOG_DEBUG(fileNode);
  }
}

/**
 * Recursively find in dirPath for eligible header files
 * @return same as ProjectParser::parse
 */
int parse_one_dir(const string& dirPath, const set<string>& excludedFiles,
    Graph& output, ProjectParser::HeaderLocationMap& headerFullPathMap, Graph& detailOutput)
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
        const int parseVal = parse_one_dir(itemPath, excludedFiles, output,
                                            headerFullPathMap, detailOutput);
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
        const string headerBasename = Common::getBaseName(itemPath);
        if (excludedFiles.end() == excludedFiles.find(headerBasename))
        {
          headerFullPathMap[headerBasename].insert(itemPath);
          process_header_file(itemPath, excludedFiles, output, detailOutput);
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
    Graph& output, Graph& detailOutput, HeaderLocationMap& outputLocationMap)
{
  int retVal = 0;
  output.clear();
  outputLocationMap.clear();
  detailOutput.clear();
  Graph tmpOutput;

  // This map keeps track of duplicate items
  // which may cause unwanted result since spinclude
  //  requires all basename of header files in project dirs are unique
  for (const string& dirName: parseDirs)
  {
    // Report
    Common::printSeparator(2, true);
    LOG_DEBUG("Parsing " << Common::getRealPath(dirName));
    Common::printSeparator(2, true);

    int helperRetval = parse_one_dir(dirName, excludedFiles, tmpOutput, outputLocationMap, detailOutput);
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

  // Check for duplicate basename
  int totalDupBasename = 0;
  for (const auto& nameSet : outputLocationMap)
  {
    if (nameSet.second.size() > 1)
    {
      ++totalDupBasename;

      LOG_DEBUG("Found duplicate for " << nameSet.first << ": ");
      for (auto fullPath : nameSet.second)
      {
        LOG_DEBUG("   " << Common::getDirName(fullPath));
      }
    }
  }

  if (totalDupBasename > 0)
  {
    retVal |= 8;
    LOG_WARN("Found duplicates for " << totalDupBasename << " header basenames");
  }

  // Check for non existence included file
  map<string, Node> idMap; // map[id] = node
  for (const auto & node : tmpOutput)
  {
    idMap.insert(std::make_pair(node.id, node));
  }

  HeaderLocationMap tossedOutMap;
  for (auto node: tmpOutput)
  {
    for (const string& childId : node.childNodes)
    {
      if (idMap.end() == idMap.find(childId))
      {
        tossedOutMap[childId].insert(node.id);
        node.childNodes.erase(childId);
      }
    }

    output.insert(node);
  }

  if (!tossedOutMap.empty())
  {
    // Report what's tossed out
    std::stringstream stm;
    for (const auto& item : tossedOutMap)
    {
      stm.str("");
      stm << "Tossed out " << item.first << " <-- (";

      for (const string & tossParent : item.second)
      {
        stm << " " << tossParent;
      }
      stm << " )";
      LOG_DEBUG(stm.str());
    }

    retVal |= 4;
    LOG_WARN("Tossed out " << tossedOutMap.size() << " nonexisted included header files");
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
      if (itemName.empty() || itemName[0] == '.')// don't process '.' '..' or hidden items
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

