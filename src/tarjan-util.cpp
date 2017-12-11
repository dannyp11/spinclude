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

static void usage(int /*argc*/, char * argv[])
{
  cout << "Usage: " << argv[0] << " [options] [file1 file2...]" << endl << endl

      << "  Utility to excercise Tarjan algorithm" << endl
      << "    inputs are files that has each line contains " << endl
      << "    node childnode1 childnode2 ...." << endl << endl

      << " Sample file content:" << endl
      << "  a b c" << endl
      << "  b d" << endl
      << "  d e" << endl
      << "  e b a" << endl << endl

      << " That means the graph is" << endl
      << "  a can go to b & c" << endl
      << "  b can go to d" << endl
      << "  and so on" << endl << endl

      << "  Option:" << endl
      << "    -v  verbose mode" << endl
      << "    -h  This message, (version " __DATE__ << " " << __TIME__ << ")" << endl << endl;

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

int main(int argc, char** argv)
{
  set<string> inputFiles;

  /**
   * Getopt parser
   */
  int command = -1;
  while ((command = getopt(argc, argv, "Dvh")) != -1)
  {
    switch (command)
    {
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
  for (int i = optind; i < argc; ++i)
  {
    inputFiles.insert(argv[i]);
  }

  if (inputFiles.empty())
  {
    LOG_ERROR("Please specify at least 1 input file");
    safeExit(1);
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

  // Process each file
  for (auto filePath : inputFiles)
  {
    Common::printSeparator(2);
    cout << "Processing " << filePath << endl;
    if (!Common::isFileExist(filePath))
    {
      LOG_ERROR(filePath << " not exist");
      continue;
    }

    // Parse each line of file to graph
    Graph graph;
    {
      std::ifstream file(filePath);
      string line;
      while (std::getline(file, line))
      {
        // Get node id
        std::istringstream ss(line);
        string id;
        if (!(ss >> id))
        {
          // empty line
          continue;
        }
        else
        {
          Node node(id);

          // now get child ids
          while(ss>>id)
          {
            node.childNodes.insert(id);
          }

          graph.insert(node);
        }
      }

      file.close();
    }


    // Now spawn the mighty solver ----------------------------------------
    TarjanSolver solver(graph);
    if (!solver.solve())
    {
      LOG_ERROR("Cannot solve!");
      safeExit(3);
    }
    else
    {
      cout << "Found " << graph.size() << " nodes" << endl;
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
    Common::printSeparator();
    if (solution.empty())
    {
      cout << "-- No circle found" << endl;
    }
    else
    {
      cout << "++ Found " << solution.size() << " circle(s):" << endl << endl;
      for (const auto & oneSet : solution)
      {
        cout << "   ";
        for (const string & header : oneSet)
        {
          cout << "\"" << header << "\" ";
        }
        cout << endl;
      }
    }
    // --------------------------------------------------------------------
  }

  return 0;
}

