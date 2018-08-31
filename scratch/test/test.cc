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
#include "libMA.h"
#include "libadd.h"
#include <mclmcrrt.h>
#include <mclcppclass.h>
#include <matrix.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

int mkpath(std::string s,mode_t mode=0755)
{
    size_t pre=0,pos;
    std::string dir;
    int mdret;

    if(s[s.size()-1]!='/'){
        // force trailing / so we can handle everything in loop
        s+='/';
    }

    while((pos=s.find_first_of('/',pre))!=std::string::npos){
        dir=s.substr(0,pos++);
        pre=pos;
        if(dir.size()==0) continue; // if leading / first time is 0 length
        if((mdret=::mkdir(dir.c_str(),mode)) && errno!=EEXIST){
            return mdret;
        }
    }
    return mdret;
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

#if 0
  mwArray *haha;

  if( !libaddInitialize())
  {
    std::cout << "Could not initialize libadd!" << std::endl;
    return -1;
  }

//  double a = 1;
//  double b = 4;
  double c;

  mwArray mwA(1, 1, mxDOUBLE_CLASS);
  mwArray mwB(1, 1, mxDOUBLE_CLASS);
  mwArray mwC(mxDOUBLE_CLASS);
  mwArray mwD(2, 2, mxDOUBLE_CLASS);

  mwA(1, 1) = 2;
  mwB(1, 1) = 4;

  mwD(1, 1) = 1;
  mwD(1, 2) = 2;
  mwD(2, 1) = 3;
  mwD(2, 2) = 4;

  add(1, mwC, mwA, mwB);

  c = (double)mwC.Get(1, 1);

  cout<<"The sum is: "<<c<<endl;

  cout<<"mwD(1, 1): "<< mwD(1, 1) <<endl;
  cout<<"mwD(1, 2): "<< mwD(1, 2) <<endl;
  cout<<"mwD(2, 1): "<< mwD(2, 1) <<endl;
  cout<<"mwD(2, 2): "<< mwD(2, 2) <<endl;

  libaddTerminate();

  if( !libMAInitialize())
  {
      std::cout << "Could not initialize libMA!" << std::endl;
      return -1;
  }

  uint32_t dim1 = 3;
  uint32_t dim2 = 2;
  uint32_t dim3 = 2;
  mwSize dims[3] = {dim1, dim2, dim3};
  mwArray mwT(3, dims, mxDOUBLE_CLASS);
  double data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  mwT.SetData(data, 12);

  std::cout << "dims="<<mwT.GetDimensions()<<std::endl;

  for (uint32_t i = 1; i <= dim1; i++)
    {
      cout << i << ":" << endl;
      for (uint32_t j = 1; j <= dim2; j++)
	{
	  for (uint32_t k = 1; k <= dim3; k++)
	    {
	      cout << " " << mwT(i, j, k);
	    }
	  cout << endl;
	}
    }
//  mwArray haha;
  haha = new mwArray(3, 2, mxDOUBLE_CLASS);
  for (uint32_t i = 1; i <= dim1; i++)
    {
      for (uint32_t j = 1; j <= dim2; j++)
	{
	  (*haha)(i, j) = mwT(i, j, 1);
	}
      cout << endl;
    }

  cout << "haha:" << endl;
  for (uint32_t i = 1; i <= dim1; i++)
    {
      for (uint32_t j = 1; j <= dim2; j++)
	{
	  cout << " " << (*haha)(i, j);
	}
      cout << endl;
    }

  delete haha;

  libMATerminate();

  mclTerminateApplication();
#endif

  std::string m_workspaceRootDir = "../workspace";
  std::string m_workspaceDir = m_workspaceRootDir + "/vanet-cs-vfc";
  std::string m_outputDir = m_workspaceDir + "/result/";
  if (access(m_outputDir.c_str(), 0) == -1)
    {
      mkpath(m_outputDir);
    }

  std::ostringstream oss;
  oss << m_outputDir << "test" << "-out.txt";
//  std::ofstream ofs; ///< output stream
//  ofs.open (oss.str(), ios::app);
//  streambuf* coutbackup;
//  coutbackup = std::cout.rdbuf(ofs.rdbuf());

  std::cout << "1globalDbSize: \t " << std::endl;
  std::cout << "TimeSpent: \t\t " << std::endl;
  std::cout << "receive_count: \t " << std::endl;
  std::cout << "SubmittedReqs: \t " << std::endl;
  std::cout << "SatisfiedReqs: \t " << std::endl;
  std::cout << "BroadcastPkts: \t " << std::endl;
  std::cout << "CumulativeDelay: " << std::endl;
//  ofs.close ();
//
//  std::cout.rdbuf(coutbackup);
}
