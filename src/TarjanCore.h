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
#ifndef SRC_TARJANCORE_H_
#define SRC_TARJANCORE_H_

#include "Common.h"

/**
 * This is the algorithm node
 */
struct TarjanNode
{
  string id;
  set<shared_ptr<TarjanNode> > childNodes;
  set<shared_ptr<TarjanNode> > parentNodes;
  int nom, denom; // denominator & common denominator of the node

  TarjanNode(const string& _id): id(_id), nom(0), denom(0) {}

  const string& str() const;
  bool hasNom() const { return nom>0; }
  bool hasDenom() const { return denom>0; }

private:
  friend std::ostream & operator<<(std::ostream &os, const TarjanNode& node);
};

typedef set<shared_ptr<TarjanNode> > TarjanGraph;

/**
 * Core solving class of Tarjan algorithm
 */
class TarjanCore
{
public:
  TarjanCore(const set<shared_ptr<TarjanNode> >& allNodes);
  virtual ~TarjanCore();

  /**
   * Try to solve
   * @return true on success
   */
  bool solve();

  /**
   * Return solution if solved successfully
   */
  const set<TarjanGraph>& getSolution();

  /**
   * Print all member data for debugging
   * @param fd - file fd for printing to
   */
  void dumpInfo(FILE* fd) const;

private:
  bool isSolved;
  TarjanGraph mAllNodes;

//
// Algorithm related vars & function
//
private:
  set<TarjanGraph> mSolution; // contains solution
  vector<shared_ptr<TarjanNode> > mFirstIterList; // list contains nodes order after first graph searching
  int mNomIndex; // nom/denom counter

private:
  void generateParentNodes_();
  void generateFirstIterList_();
  void generateSolutionSet_();
  static void generateSolutionSetParentHelper_(TarjanGraph& solutionSet,
                                              shared_ptr<TarjanNode>& node,
                                              set<string>& visitedIds);
  void updateNodeFirstIter_(shared_ptr<TarjanNode>& node);
  int nextIndex_();
};

#endif /* SRC_TARJANCORE_H_ */
