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
#include "ProjectParser.h"

class ProjParserTest: public ::testing::Test
{
protected:
  void SetUp()
  {
    mAssetDirPath = "test/asset";
    mNoHeaderDir = mAssetDirPath + "/no-header";
    mHasHeaderDir = mAssetDirPath + "/has-header-no-include";
    mHasHeaderWithIncludeDir = mAssetDirPath + "/has-header-with-include";
  }

  void TearDown()
  {
    Common::setDebugMode(false);
  }

  string mAssetDirPath, mNoHeaderDir, mHasHeaderDir, mHasHeaderWithIncludeDir;
};

TEST_F(ProjParserTest, testNoHeaderDir)
{
  set<string> allDirs = {mNoHeaderDir};
  set<string> excludeFiles = {};
  Graph graph, detailGraph;
  ProjectParser::HeaderLocationMap locationMap;
  ASSERT_EQ(2, ProjectParser::parse(allDirs, excludeFiles, graph, detailGraph, locationMap));
}

TEST_F(ProjParserTest, testHasHeaderDir)
{
  set<string> allDirs = {mHasHeaderDir};
  set<string> excludeFiles = {};
  Graph graph, detailGraph;
  ProjectParser::HeaderLocationMap locationMap;

  ASSERT_EQ(0, ProjectParser::parse(allDirs, excludeFiles, graph, detailGraph, locationMap));
  ASSERT_EQ(6, graph.size());
}

TEST_F(ProjParserTest, testHasHeaderDirs)
{
  set<string> allDirs = {mNoHeaderDir, mHasHeaderDir, mHasHeaderDir};
  set<string> excludeFiles = {};
  Graph graph, detailGraph;
  ProjectParser::HeaderLocationMap locationMap;

  ASSERT_EQ(0, ProjectParser::parse(allDirs, excludeFiles, graph, detailGraph, locationMap));
  ASSERT_EQ(6, graph.size());
}

TEST_F(ProjParserTest, testHasHeaderWithIncludeDir)
{
  set<string> allDirs = {mHasHeaderWithIncludeDir};
  set<string> excludeFiles = {"stdio.h"};
  Graph graph, detailGraph;
  ProjectParser::HeaderLocationMap locationMap;

  ASSERT_EQ(0, ProjectParser::parse(allDirs, excludeFiles, graph, detailGraph, locationMap));
  ASSERT_EQ(6, graph.size());
}
