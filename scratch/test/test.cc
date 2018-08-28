/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/packet.h"
#include "graph.hpp"
#include "byte-buffer.h"
#include "stats.h"
#include "custom-type.h"
#include "libadd.h"
#include "libMA.h"
#include <mclmcrrt.h>
#include <mclcppclass.h>
#include <matrix.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

void test()
{
  cout << Now().GetDouble() << endl;
}

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Scratch Simulator");
  CommandLine cmd;
  cmd.Parse (argc, argv);

#if 0
  {
    Ptr<UniformRandomVariable> urn = CreateObject<UniformRandomVariable>();
    vector<VertexNode> vertices;
    uint32_t size = 5;
    for (uint32_t i = 0; i < size; i++)
      {
//	uint32_t j = (uint32_t)(urn->GetValue(0, size));
//	while (j == i) j = (uint32_t)(urn->GetValue(0, size));
//	VertexNode vn(i, j);
	VertexNode vn(i, i);
	vertices.push_back(vn);
      }

    GraphMatrix<VertexNode> graphM(vertices, vertices.size());
    EdgeNode<> en = {.type = EdgeType::NOT_SET, .weight = 1};
#if 0
    graphM.addEdge(vertices[0], vertices[2], en);
    graphM.addEdge(vertices[3], vertices[1], en);
    graphM.addEdge(vertices[0], vertices[1], en);
    graphM.addEdge(vertices[4], vertices[3], en);
    graphM.addEdge(vertices[0], vertices[3], en);
    graphM.addEdge(vertices[0], vertices[4], en);
#endif

#if 1
    graphM.addEdge(vertices[0], vertices[1], en);
    graphM.addEdge(vertices[0], vertices[2], en);
    graphM.addEdge(vertices[0], vertices[3], en);
    graphM.addEdge(vertices[0], vertices[4], en);
    graphM.addEdge(vertices[2], vertices[3], en);
    graphM.addEdge(vertices[2], vertices[4], en);
    graphM.addEdge(vertices[3], vertices[4], en);
#endif
    graphM.printEdge();

    vector<PAIR<VertexNode>> vec = graphM.sortAllVertexByDegree();
    for (PAIR<VertexNode> p : vec)
      {
	cout << p.first << ": " << p.second << endl;
      }
    cout << "------------" << endl;

    {
      clock_t startTime, endTime;
      startTime = clock();
      graphM.printCliques(graphM.getCliques(10));
      endTime = clock();
      cout << "sim time: " << Now().GetSeconds() << ", clock: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << endl;
      cout << "------------" << endl;
    }

    {
      clock_t startTime, endTime;
      startTime = clock();
      graphM.printCliques(graphM.getCliquesWithBA(10));
      endTime = clock();
      cout << "sim time: " << Now().GetSeconds() << ", clock: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << endl;
      cout << "------------" << endl;
    }
  }
#endif

//  std::set<uint32_t> haha;
//  std::set<uint32_t> hehe;
//  for (uint32_t i = 0; i < 10; i++)
//    {
//      haha.insert(i);
//    }
//  hehe.insert(2);
//  hehe.insert(4);
//  hehe.insert(6);
//
//  std::set<uint32_t>::iterator iter = haha.begin();
//  for (; iter != haha.end(); iter++)
//    {
//      cout << " " << *iter;
//    }
//  cout << endl;
//
//  std::set<uint32_t>::iterator iter2 = hehe.begin();
//  for (; iter2 != hehe.end(); iter2++)
//    {
//      haha.erase(*iter2);
//    }
//
//  iter = haha.begin();
//  for (; iter != haha.end(); iter++)
//    {
//      cout << " " << *iter;
//    }
//  cout << endl;


  if( !libaddInitialize())
  {
      std::cout << "Could not initialize libmyFunc!" << std::endl;
      return -1;
  }

  double a = 1;
  double b = 4;
  double c;

  mwArray mwA(1, 1, mxDOUBLE_CLASS);
  mwArray mwB(1, 1, mxDOUBLE_CLASS);
  mwArray mwC(1, 1, mxDOUBLE_CLASS);

  mwA.SetData(&a, 1);
  mwB.SetData(&b, 1);

  add(1, mwC, mwA, mwB);

  c = mwC.Get(1, 1);

  cout<<"The sum is: "<<c<<endl;
  libaddTerminate();

  if( !libMAInitialize())
  {
      std::cout << "Could not initialize libmyFunc!" << std::endl;
      return -1;
  }

//  MA();

  libMATerminate();

  mclTerminateApplication();
}
