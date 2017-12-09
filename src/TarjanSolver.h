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
#ifndef SRC_TARJANSOLVER_H_
#define SRC_TARJANSOLVER_H_

#include "DataStructure.h"

struct TarjanNode;
class TarjanCore;

/**
 * Front end of the core solver
 * External classes should use this class only, do not touch TarjanCore
 */
class TarjanSolver
{
public:
  TarjanSolver(const Graph& allNodes);
  virtual ~TarjanSolver();

  /**
   * Main algorithm, try to find circle in graph
   * @return true on success
   */
  bool solve();

  /**
   * If solved successfully, return a set of strongly connected graph
   */
  const set<set<string> >& getSolution();

private: // internal functions
  /**
   * Convert Graph to/from set<TarjanNode>
   * @return true on success
   */
  bool convertToCoreNodes_();
  bool convertFromCoreNodes_();

private: // internal facing vars
  set<shared_ptr<TarjanNode> > mAllTarjanNodes;
  set<set<shared_ptr<TarjanNode> > > mTarjanSolution;

private: // external facing vars
  set<set<string> > mSolution;
  Graph mGraph;
  bool isSolved;
};

#endif /* SRC_TARJANSOLVER_H_ */
