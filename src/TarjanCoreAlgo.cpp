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

// This file implements the solving of TarjanCore
// We move solve() to here so it's all about solving the algo

static bool compare_denom(const shared_ptr<TarjanNode>& a,const shared_ptr<TarjanNode>& b)
{
  return a->denom > b->denom;
}

bool TarjanCore::solve()
{
  if (!isSolved)
  {
    // Init all vars
    mSolution.clear();

    // Refresh parent nodes
    generateParentNodes_();

    // Generate mFirstIterList
    generateFirstIterList_();

    // Generate solution set
    generateSolutionSet_();

    isSolved = mSolution.size() > 0;
  }

  return isSolved;
}

void TarjanCore::generateParentNodes_()
{
  // clear all parent set in mAllNodes
  for (auto & node: mAllNodes)
  {
    node->parentNodes.clear();
  }

  // now go thru all child nodes of 1 node and update their parent set
  for (auto & node: mAllNodes)
  {
    for (auto & childNode: node->childNodes)
    {
      childNode->parentNodes.insert(node);
    }
  }
}

void TarjanCore::generateFirstIterList_()
{
  mFirstIterList.clear();
  mNomIndex = 0;

  // First update all node nom & denom
  for (auto node : mAllNodes)
  {
    updateNodeFirstIter_(node);
  }

  // Now put all nodes on iterlist and sort by denom descending
  mFirstIterList = vector<shared_ptr<TarjanNode> >(mAllNodes.size());
  std::partial_sort_copy(mAllNodes.begin(), mAllNodes.end(),
      mFirstIterList.begin(), mFirstIterList.end(), compare_denom);
}

int TarjanCore::nextIndex_()
{
  return ++mNomIndex;
}

void TarjanCore::updateNodeFirstIter_(shared_ptr<TarjanNode>& node)
{
  // This mean node is visited
  if (node->hasNom() && node->hasDenom())
  {
    return;
  }

  node->nom = nextIndex_();
  for (auto child : node->childNodes)
  {
    if (!child->hasNom())
    {
      updateNodeFirstIter_(child);
    }
  }

  // update denom of node here
  node->denom = nextIndex_();
}

void TarjanCore::generateSolutionSet_()
{
  // If id exists here, we won't process it in firstIterList
  set<string> visitedIds;
  for (auto node: mFirstIterList)
  {
    // Now check if node has any eligible parent
    TarjanGraph oneSolution;
    generateSolutionSetParentHelper_(oneSolution, node, visitedIds);
    if (oneSolution.size() > 0)
    {
      mSolution.insert(oneSolution);
    }
  }
}

void TarjanCore::generateSolutionSetParentHelper_(TarjanGraph& solutionSet,
                                                  shared_ptr<TarjanNode>& node,
                                                  set<string>& visitedIds)
{
  if (visitedIds.find(node->id) != visitedIds.end())
  {
    return;
  }

  // Recursively add to solution set if found parent
  solutionSet.insert(node);
  visitedIds.insert(node->id);

  // Go thru all parents and add to solution set if eligible
  for (auto parent : node->parentNodes)
  {
    generateSolutionSetParentHelper_(solutionSet, parent, visitedIds);
  }

}
