#include "gtest/gtest.h"
#include "TarjanCore.h"

class CoreAlgoTest: public ::testing::Test
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

TEST_F(CoreAlgoTest, Test2Nodes1Circle)
{
  // Create data graph of a cycle of 2 nodes
  // 1<->2
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");

  node1->childNodes.insert(node2);
  node2->childNodes.insert(node1);

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(1, solution.size());
}

TEST_F(CoreAlgoTest, Test2Nodes2Circle)
{
  // Create data graph of 2 nodes
  // 1 2
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(2, solution.size());
}

TEST_F(CoreAlgoTest, Test3Nodes2Circle)
{
  // Create data graph of 3 nodes
  // 1<->2 -> 3
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");
  auto node3 = std::make_shared<TarjanNode>("3");

  node1->childNodes.insert(node2);
  node2->childNodes.insert(node1);
  node2->childNodes.insert(node3);

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);
  graph.insert(node3);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(2, solution.size());
}

TEST_F(CoreAlgoTest, Test3Nodes1Circle)
{
  // Create data graph of 3 nodes
  // 1->2 -> 3 ->1
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");
  auto node3 = std::make_shared<TarjanNode>("3");

  node1->childNodes.insert(node2);
  node2->childNodes.insert(node3);
  node3->childNodes.insert(node1);

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);
  graph.insert(node3);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(1, solution.size());
}

TEST_F(CoreAlgoTest, Test4Nodes2Circle)
{
  // Create data graph of 3 nodes
  // 1<->4  5<->7
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("4");
  auto node3 = std::make_shared<TarjanNode>("5");
  auto node4 = std::make_shared<TarjanNode>("7");

  node1->childNodes.insert(node2);
  node2->childNodes.insert(node1);
  node3->childNodes.insert(node4);
  node4->childNodes.insert(node3);

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);
  graph.insert(node3);
  graph.insert(node4);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(2, solution.size());
}

TEST_F(CoreAlgoTest, Test5Nodes3Circle)
{
  // Create data graph of 3 nodes
  // 0->1->2->1 0->3->4
  auto node0 = std::make_shared<TarjanNode>("0");
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");
  auto node3 = std::make_shared<TarjanNode>("3");
  auto node4 = std::make_shared<TarjanNode>("4");

  node0->childNodes.insert(node1);
  node1->childNodes.insert(node2);
  node2->childNodes.insert(node0);
  node0->childNodes.insert(node3);
  node3->childNodes.insert(node4);

  TarjanGraph graph;
  graph.insert(node1);
  graph.insert(node2);
  graph.insert(node3);
  graph.insert(node0);
  graph.insert(node4);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(3, solution.size());
}

TEST_F(CoreAlgoTest, Test4Nodes2Circle2)
{
  // Create data graph of 3 nodes
  // 1<->2->3<->4
  auto node1 = std::make_shared<TarjanNode>("1");
  auto node2 = std::make_shared<TarjanNode>("2");
  auto node3 = std::make_shared<TarjanNode>("3");
  auto node4 = std::make_shared<TarjanNode>("4");

  node1->childNodes.insert(node2);
  node2->childNodes.insert(node1);
  node2->childNodes.insert(node3);

  node3->childNodes.insert(node4);
  node4->childNodes.insert(node3);

  TarjanGraph graph;
  graph.insert(node2);
  graph.insert(node1);
  graph.insert(node3);
  graph.insert(node4);

  TarjanCore coreSolver(graph);
  coreSolver.dumpInfo(stdout);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(2, solution.size());
}

TEST_F(CoreAlgoTest, TestComprehensive)
{
  auto nodea = std::make_shared<TarjanNode>("a");
  auto nodeb = std::make_shared<TarjanNode>("b");
  auto nodec = std::make_shared<TarjanNode>("c");
  auto noded = std::make_shared<TarjanNode>("d");
  auto nodee = std::make_shared<TarjanNode>("e");
  auto nodef = std::make_shared<TarjanNode>("f");
  auto nodeg = std::make_shared<TarjanNode>("g");
  auto nodeh = std::make_shared<TarjanNode>("h");

  nodea->childNodes.insert(nodeb);
  nodea->childNodes.insert(nodef);

  nodeb->childNodes.insert(nodec);
  nodeb->childNodes.insert(nodef);

  nodec->childNodes.insert(noded);
  nodec->childNodes.insert(nodeg);

  nodee->childNodes.insert(nodea);

  nodef->childNodes.insert(nodee);
  nodef->childNodes.insert(nodeg);

  nodeg->childNodes.insert(nodec);

  nodeh->childNodes.insert(nodeg);

  TarjanGraph graph;
  graph.insert(nodea);
  graph.insert(nodeb);
  graph.insert(nodec);
  graph.insert(noded);
  graph.insert(nodee);
  graph.insert(nodef);
  graph.insert(nodeg);
  graph.insert(nodeh);

  TarjanCore coreSolver(graph);
  coreSolver.dumpInfo(stdout);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(4, solution.size());
}

TEST_F(CoreAlgoTest, TestComprehensive2)
{
  auto nodea = std::make_shared<TarjanNode>("a");
  auto nodeb = std::make_shared<TarjanNode>("b");
  auto nodec = std::make_shared<TarjanNode>("c");
  auto noded = std::make_shared<TarjanNode>("d");
  auto nodee = std::make_shared<TarjanNode>("e");
  auto nodef = std::make_shared<TarjanNode>("f");
  auto nodeg = std::make_shared<TarjanNode>("g");
  auto nodeh = std::make_shared<TarjanNode>("h");

  nodea->childNodes.insert(nodeb);

  nodeb->childNodes.insert(nodee);
  nodeb->childNodes.insert(nodec);
  nodeb->childNodes.insert(nodef);

  nodec->childNodes.insert(noded);
  nodec->childNodes.insert(nodeg);

  noded->childNodes.insert(nodec);
  noded->childNodes.insert(nodeh);

  nodee->childNodes.insert(nodea);
  nodee->childNodes.insert(nodef);

  nodef->childNodes.insert(nodeg);

  nodeg->childNodes.insert(nodef);
  nodeg->childNodes.insert(nodeh);

  nodeh->childNodes.insert(nodeh);

  TarjanGraph graph;
  graph.insert(nodea);
  graph.insert(nodeb);
  graph.insert(nodec);
  graph.insert(noded);
  graph.insert(nodee);
  graph.insert(nodef);
  graph.insert(nodeg);
  graph.insert(nodeh);

  TarjanCore coreSolver(graph);
  EXPECT_TRUE(coreSolver.solve());

  auto solution = coreSolver.getSolution();
  EXPECT_EQ(4, solution.size());
}
