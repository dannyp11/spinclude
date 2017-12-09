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
#include "gtest/gtest.h"
#include "TarjanSolver.h"

class SolverTest: public ::testing::Test
{
protected:
  void SetUp()
  {
  }

  void TearDown()
  {
    Common::setDebugMode(false);
  }
};

TEST_F(SolverTest, Test2Nodes1Circle)
{
  // Create data graph of a cycle of 2 nodes
  // 1<->2
  Node node1("1");
  Node node2("2");

  node1.childNodes.insert(node2.id);
  node2.childNodes.insert(node1.id);

  Graph graph;
  graph.insert(node1);
  graph.insert(node2);

  TarjanSolver solver(graph);
  ASSERT_TRUE(solver.solve());

  auto solution = solver.getSolution();
  EXPECT_EQ(1, solution.size());
}

TEST_F(SolverTest, Test2Nodes2Circle)
{
  // Create data graph of 2 nodes
  // 1 2
  Node node1("1");
  Node node2("2");

  Graph graph;
  graph.insert(node1);
  graph.insert(node2);

  TarjanSolver solver(graph);
  ASSERT_TRUE(solver.solve());

  auto solution = solver.getSolution();
  EXPECT_EQ(2, solution.size());
}

TEST_F(SolverTest, TestComprehensive)
{
  Node nodea("a");
  Node nodeb("b");
  Node nodec("c");
  Node noded("d");
  Node nodee("e");
  Node nodef("f");
  Node nodeg("g");
  Node nodeh("h");

  nodea.childNodes.insert(nodeb.id);
  nodea.childNodes.insert(nodef.id);

  nodeb.childNodes.insert(nodec.id);
  nodeb.childNodes.insert(nodef.id);

  nodec.childNodes.insert(noded.id);
  nodec.childNodes.insert(nodeg.id);

  nodee.childNodes.insert(nodea.id);

  nodef.childNodes.insert(nodee.id);
  nodef.childNodes.insert(nodeg.id);

  nodeg.childNodes.insert(nodec.id);

  nodeh.childNodes.insert(nodeg.id);

  Graph graph;
  graph.insert(nodea);
  graph.insert(nodeb);
  graph.insert(nodec);
  graph.insert(noded);
  graph.insert(nodee);
  graph.insert(nodef);
  graph.insert(nodeg);
  graph.insert(nodeh);

  TarjanSolver solver(graph);
  ASSERT_TRUE(solver.solve());

  auto solution = solver.getSolution();
  EXPECT_EQ(4, solution.size());
}

TEST_F(SolverTest, TestComprehensive2)
{
  Node nodea("a");
  Node nodeb("b");
  Node nodec("c");
  Node noded("d");
  Node nodee("e");
  Node nodef("f");
  Node nodeg("g");
  Node nodeh("h");

  nodea.childNodes.insert(nodeb.id);

  nodeb.childNodes.insert(nodee.id);
  nodeb.childNodes.insert(nodec.id);
  nodeb.childNodes.insert(nodef.id);

  nodec.childNodes.insert(noded.id);
  nodec.childNodes.insert(nodeg.id);

  noded.childNodes.insert(nodec.id);
  noded.childNodes.insert(nodeh.id);

  nodee.childNodes.insert(nodea.id);
  nodee.childNodes.insert(nodef.id);

  nodef.childNodes.insert(nodeg.id);

  nodeg.childNodes.insert(nodef.id);
  nodeg.childNodes.insert(nodeh.id);

  nodeh.childNodes.insert(nodeh.id);

  Graph graph;
  graph.insert(nodea);
  graph.insert(nodeb);
  graph.insert(nodec);
  graph.insert(noded);
  graph.insert(nodee);
  graph.insert(nodef);
  graph.insert(nodeg);
  graph.insert(nodeh);

  TarjanSolver solver(graph);
  ASSERT_TRUE(solver.solve());

  auto solution = solver.getSolution();
  EXPECT_EQ(4, solution.size());
}
