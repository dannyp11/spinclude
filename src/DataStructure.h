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
#ifndef SPINCLUDE_DATASTRUCTURE_H_
#define SPINCLUDE_DATASTRUCTURE_H_

#include "Common.h"

/**
 * This is the user input
 */
struct Node
{
  string id; // unique id for the node, can be name or number, etc.
  set<string> childNodes; // set of other node ids that this node can go to

  Node(string _id): id(_id) {}
  const string& toString() const;

private:
  friend std::ostream & operator<<(std::ostream &os, const Node& node);
  friend bool operator<(const Node &lhs, const Node &rhs);
};

/**
 * User should provide this set to the algorithm
 */
typedef set<Node> Graph;

#endif /* SPINCLUDE_DATASTRUCTURE_H_ */
