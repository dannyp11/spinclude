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

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <fstream>

#include "TarjanSolver.h"
#include "ProjectParser.h"
#include "ConfigFile.h"

#include "_default_proj_cfg.h"

const string DEFAULT_CFG_FILE = "Project.cfg";

static void usage(int /*argc*/, char * argv[])
{
  cout << "Usage: " << argv[0] << " [options] [dir1 dir2...]" << endl << endl

      << "  Tool to detect circling included headers in a project" << endl
      << "    if [dir1 dir2...] is blank, use current pwd as project dir" << endl << endl

      << "  Option:" << endl
      << "    -c {cfg file}   use project config file" << endl
      << "    -g              generate config file " << DEFAULT_CFG_FILE << endl
      << "    -v              verbose mode" << endl
      << "    -h              This message, (version " __DATE__ << " " << __TIME__ << ")" << endl << endl;

  exit(1);
}

static void safeExit(int errCode = 0)
{
  exit(errCode);
}

static void sigHandler(int signo)
{
  cout << " Caught signal " << signo << endl;
  safeExit(0);
}

static void errorHandler(int signo)
{
  cout << " Caught error signal " << signo << endl;
  void *array[10];
  size_t size = backtrace(array, 10);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, size, STDERR_FILENO);

  safeExit(signo);
}

static void exportDefaultCfgFile()
{
  if (Common::isFileExist(DEFAULT_CFG_FILE))
  {
    LOG_WARN(DEFAULT_CFG_FILE << " already exists");
    safeExit(1);
  }

  std::ofstream defCfgFile(DEFAULT_CFG_FILE.c_str(), std::ofstream::binary);
  defCfgFile.write((char*)DefaultProjectDescription_cfg, DefaultProjectDescription_cfg_len);
  defCfgFile.close();
  cout << "Generated " << DEFAULT_CFG_FILE << endl;
  safeExit(defCfgFile.fail());
}

int main(int argc, char** argv)
{
  string cfgFilePath;
  ConfigData cfgData;
  cfgData.projDirs.clear();

  /**
   * Getopt parser
   */
  int command = -1;
  while ((command = getopt(argc, argv, "c:gDvh")) != -1)
  {
    switch (command)
    {
    case 'c':
      cfgFilePath = optarg;
      break;
    case 'g':
      exportDefaultCfgFile();
      break;
    case 'v':
      Common::setVerboseMode(true);
      break;
    case 'D':
      cout << "Debug mode ON" << endl;
      Common::setDebugMode(true);
      break;
    case 'h':
    default:
      usage(argc, argv);
      break;
    }
  }

  // Get proj dirs
  if (optind == argc)
  {
    cfgData.projDirs.insert(".");
  }
  else
  {
    for (int i = optind; i < argc; ++i)
    {
      cfgData.projDirs.insert(argv[i]);
    }
  }

  // Register sig handlers
  signal(SIGINT, sigHandler);
  signal(SIGHUP, sigHandler);
  signal(SIGQUIT, sigHandler);
  signal(SIGSEGV, errorHandler);
  signal(SIGABRT, errorHandler);
  signal(SIGILL, errorHandler);
  signal(SIGFPE, errorHandler);
  signal(SIGPIPE, errorHandler);

  // Now see if cfgFile is specified, if so use it instead of inputs
  if (!cfgFilePath.empty())
  {
    ConfigFile cfgFile(cfgFilePath);
    if (!cfgFile.parse())
    {
      LOG_ERROR("Error parsing " << cfgFilePath << ", exiting...");
      safeExit(1);
    }

    cfgData = cfgFile.data();
  }

  // Report cfg data
  cout << "Config data:\n";
  Common::printSeparator();
  cfgData.dump(stdout);
  Common::printSeparator();

  // Get all excluded header files
  set<string> allExcludedFiles;
  if (0 != ProjectParser::generateHeaderList(cfgData.excludedDirs, allExcludedFiles))
  {
    LOG_ERROR("Error generating excluded files from excluded dirs, ignoring these dirs");
    allExcludedFiles.clear();
  }
  allExcludedFiles.insert(cfgData.excludedFiles.begin(), cfgData.excludedFiles.end());
  LOG_DEBUG("Excluding " << allExcludedFiles.size() << " headers");

  // Get all target header files
  Graph headerFileGraph;
  ProjectParser::HeaderLocationMap headerPathMap;
  int parseCode = ProjectParser::parse(cfgData.projDirs, allExcludedFiles,
                                       headerFileGraph, headerPathMap);
  if (0 > parseCode)
  {
    LOG_ERROR("Critical error code " << parseCode << " while getting input headers");
    safeExit(2);
  }
  else if (parseCode > 0)
  {
    LOG_DEBUG("Warning code " << parseCode << " while getting input headers");
  }

  // Now spawn the mighty solver ----------------------------------------
  TarjanSolver solver(headerFileGraph);
  if (!solver.solve())
  {
    LOG_ERROR("Cannot solve!");
    safeExit(3);
  }
  else
  {
    cout << "Processed " << headerFileGraph.size() << " header files" << endl;
  }

  // Don't use 1 element solution set
  auto solution = solver.getSolution();
  for (const auto& oneSet : solution)
  {
    if (oneSet.size() <=1)
    {
      solution.erase(oneSet);
    }
  }
  // --------------------------------------------------------------------

  // Report result -------------------------------------------------------
  Common::printSeparator(2);
  if (solution.empty())
  {
    cout << "-- No circle found" << endl;
  }
  else
  {
    cout << "++ Found " << solution.size() << " circle(s):" << endl << endl;
    for (const auto & oneSet : solution)
    {
      Common::printSeparator(1, true);
      cout << "   ";
      for (const string & header : oneSet)
      {
        cout << "\"" << header << "\" ";
      }
      cout << endl;

      // Report detailed path
      for (const auto header : oneSet)
      {
        const auto headerPathSetIt = headerPathMap.find(header);
        if (Common::isVerboseMode()
            && headerPathMap.end() != headerPathSetIt
            && !headerPathSetIt->second.empty())
        {
          for (const auto path: headerPathSetIt->second)
          {
            if (path == *(headerPathSetIt->second.begin()))
            {
              std::cerr << "   -- ";
            }
            else
            {
              std::cerr << "      ";
            }
            std::cerr << path << endl;
          }
          std::cerr << endl;
        }
      }
    }
  }
  Common::printSeparator(2);
  // --------------------------------------------------------------------

  return 0;
}

