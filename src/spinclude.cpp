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

#include "DataStructure.h"
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
      << "    -E {dir path}   exclude header in dir path while solving" << endl
      << "    -e {header}     exclude header while solving" << endl

      << "    -c {cfg file}   use project config file" << endl
      << "    -g              generate config file " << DEFAULT_CFG_FILE << endl
      << "    -h              This message, (version " __DATE__ << " " << __TIME__ << ")" << endl << endl;

  exit(1);
}

static void cleanup()
{
}

static void safeExit(int errCode = 0)
{
  cleanup();
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
  std::ofstream defCfgFile(DEFAULT_CFG_FILE.c_str(), std::ofstream::binary);
  defCfgFile.write((char*)DefaultProjectDescription_cfg, DefaultProjectDescription_cfg_len);
  defCfgFile.close();
  cout << "Generated " << DEFAULT_CFG_FILE << endl;
  safeExit(defCfgFile.fail());
}

int main(int argc, char** argv)
{
  string cfgFile;
  ConfigData cfgData;

  /**
   * Getopt parser
   */
  int command = -1;
  while ((command = getopt(argc, argv, "gDh")) != -1)
  {
    switch (command)
    {
    case 'g':
      exportDefaultCfgFile();
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

  // Register sig handlers
  signal(SIGINT, sigHandler);
  signal(SIGHUP, sigHandler);
  signal(SIGQUIT, sigHandler);
  signal(SIGSEGV, errorHandler);
  signal(SIGABRT, errorHandler);
  signal(SIGILL, errorHandler);
  signal(SIGFPE, errorHandler);
  signal(SIGPIPE, errorHandler);



  return 0;
}

