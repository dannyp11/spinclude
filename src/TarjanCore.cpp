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
#include "TarjanCore.h"

TarjanCore::TarjanCore(const set<shared_ptr<TarjanNode> >& allNodes): mAllNodes(allNodes)
{
  isSolved = false;
  mNomIndex = 0;
}

TarjanCore::~TarjanCore()
{
  // TODO Auto-generated destructor stub
}

const set<TarjanGraph>& TarjanCore::getSolution()
{
  static set<TarjanGraph> retVal;
  if (!isSolved)
  {
    LOG_ERROR("Need to solve before getting solution");
  }
  else
  {
    retVal = mSolution;
  }

  return retVal;
}

const string& TarjanNode::str() const
{
  static string result;
  std::stringstream stm;
  stm << "\"" << id << "\" " << nom << ":" << denom;

  if (childNodes.size() > 0)
  {
    stm << " Child ( ";
    for (const auto& child : childNodes)
    {
      stm << "\"" << child->id << "\" ";
    }
    stm << ")";
  }

  if (parentNodes.size() > 0)
  {
    stm << " Parent ( ";
    for (const auto& parent : parentNodes)
    {
      stm << "\"" << parent->id << "\" ";
    }
    stm << ")";
  }

  result = stm.str();
  return result;
}

std::ostream & operator<<(std::ostream &os, const TarjanNode& node)
{
  return os << node.str();
}

void TarjanCore::dumpInfo(FILE* fd) const
{
  if (Common::isDebugMode())
  {
    // Only print in debug mode
    fprintf(fd, "Total nodes: %lu\n", mAllNodes.size());
    for (const auto & node : mAllNodes)
    {
      fprintf(fd, " %s\n", node->str().c_str());
    }

    if (mFirstIterList.size() > 0)
    {
      fprintf(fd, " First iter list: \n");
      for (const auto & node : mFirstIterList)
      {
        fprintf(fd, "-- %s\n", node->str().c_str());
      }
    }
  }

  if (mSolution.size() > 0)
  {
    fprintf(fd, "Solution: %lu circle(s)\n", mSolution.size());
    for (const auto & graph : mSolution)
    {
      for (const auto & node : graph)
      {
        fprintf(fd, " \"%s\"", node->id.c_str());
      }
      fprintf(fd, "\n");
    }
  }
}
