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
#include "TarjanSolver.h"
#include "TarjanCore.h"

TarjanSolver::TarjanSolver(const Graph& allNodes): mGraph(allNodes)
{
  isSolved = false;
}

TarjanSolver::~TarjanSolver()
{
}

bool TarjanSolver::solve()
{
  if (!isSolved)
  {
    // Try to solve in here
    if (!convertToCoreNodes_())
    {
      LOG_ERROR("Cannot convert to core data structure");
      return false;
    }

    TarjanCore coreSolver(mAllTarjanNodes);
    isSolved = coreSolver.solve();

    if (!isSolved)
    {
      LOG_ERROR("Cannot solve using TarjanCore");
    }
    else
    {
      mTarjanSolution = coreSolver.getSolution();
    }
  }

  return isSolved;
}

const set<set<string> >& TarjanSolver::getSolution()
{
  static auto retVal = mSolution;
  if (!isSolved)
  {
    LOG_ERROR("Solver must call solve() before calling " << __FUNCTION__);
    return retVal;

  }

  if (!convertFromCoreNodes_())
  {
    retVal.clear();
    LOG_ERROR("Cannot convert solution from core data structure");
  }
  else
  {
    retVal = mSolution;
  }

  return retVal;
}

bool TarjanSolver::convertToCoreNodes_()
{
  // Create tarjan graph map
  map<string, shared_ptr<TarjanNode> > allTarjanNodeMap; // map[id] = tarjannode
  for (const Node & node : mGraph)
  {
    allTarjanNodeMap[node.id] = std::make_shared<TarjanNode>(node.id);
  }

  // For tolerance, add tarjan node that exists from child nodes
  // but not exist in node map
  for (const Node & node : mGraph)
  {
    for (const string& childId : node.childNodes)
    {
      if (allTarjanNodeMap.end() == allTarjanNodeMap.find(childId))
      {
        LOG_DEBUG("Created non-existent node " << childId);
        allTarjanNodeMap[childId] = std::make_shared<TarjanNode>(childId);
      }
    }
  }

  // Update tarjan graph map
  for (const Node & node : mGraph)
  {
    for (const string& childId : node.childNodes)
    {
      if (allTarjanNodeMap.find(childId) == allTarjanNodeMap.end())
      {
        LOG_ERROR("Cannot find id " << childId << " in input map");
        return false;
      }
      else
      {
        allTarjanNodeMap[node.id]->childNodes.insert(allTarjanNodeMap[childId]);
      }
    }
  }

  // Now transfer allTarjanNodeMap to mAllTarjanNodes
  for (auto entry : allTarjanNodeMap)
  {
    mAllTarjanNodes.insert(entry.second);
  }

  return true;
}

bool TarjanSolver::convertFromCoreNodes_()
{
  mSolution.clear();
  for (auto oneSolutionSet : mTarjanSolution)
  {
    set<string> oneSolution;
    for (const auto& tarjanNode : oneSolutionSet)
    {
      oneSolution.insert(tarjanNode->id);
    }

    if (oneSolution.size() > 0)
    {
      mSolution.insert(oneSolution);
    }
    else
    {
      LOG_ERROR("Got empty solution from core algorithm");
      return false;
    }
  }
  return true;
}
