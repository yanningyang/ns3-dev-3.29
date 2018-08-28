/*
 * graph.hpp
 *
 *  Created on: Jul 24, 2018
 *      Author: Yang yanning <yang.ksn@gmail.com>
 */

#ifndef SCRATCH_VANET_CS_VFC_GRAPH_HPP_
#define SCRATCH_VANET_CS_VFC_GRAPH_HPP_

//#define NDEBUG
#include <cassert>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "custom-type.h"

using namespace std;

template <typename V>
using PAIR = pair<V, size_t>;

template <typename V>
struct CmpByValue
{
  bool operator()(const PAIR<V>& lhs, const PAIR<V>& rhs)
  {
    return lhs.second < rhs.second;
  }
};

struct VertexNode
{
  uint32_t	fogIndex;
  uint32_t	reqDataIndex;
  std::string	name;

  VertexNode()
  {
    this->fogIndex = 0;
    this->reqDataIndex = 0;
    this->name = "";
  }
  VertexNode(const uint32_t& _fogIndex, const uint32_t& _reqDataIndex)
      : fogIndex(_fogIndex)
      , reqDataIndex(_reqDataIndex)
      , name("")
  {
    genName();
  }

  void genName()
  {
    std::ostringstream oss;
    oss << fogIndex << "-" << reqDataIndex;
    this->name = oss.str();
  }

  friend ostream & operator << (ostream &os, VertexNode &vn)
    {
      os << vn.fogIndex << "-" << vn.reqDataIndex;
      return os;
    }
  bool operator == (const VertexNode &vn) const
    {
      return (this->fogIndex == vn.fogIndex) && (this->reqDataIndex == vn.reqDataIndex);
    }
};

template <typename W = uint32_t>
struct EdgeNode
{
  EdgeType	type;
  W		weight;
  EdgeNode()
  {
    this->type = EdgeType::NOT_SET;
    this->weight = 0;
  }
  EdgeNode(EdgeType _type, const W& _weight)
      : type(_type)
      , weight(_weight)
  {}

  friend ostream & operator << (ostream &os, EdgeNode &en)
    {
      os << en.weight;
      return os;
    }
};

struct MCP
{
  vector<uint32_t> x; //The connectivity between the vertex and the current clique, x[i]=1 indicates i is connected to current clique
  vector<uint32_t> bestx; //optimal solution
  size_t cnum; //vertex num of current clique
  size_t bestn; //vertex num of maximal clique
};

/**
 * adjacent matrix
 */
template <typename V>
class GraphMatrix
{
public:
  GraphMatrix();
  GraphMatrix(const vector<V>& _vertex, size_t _size, bool _isDirected = false);
  void printEdge();
  vector<V> getAdjVertex(const V& v);
  void delVertex(const V& v);
  void delVertex(const size_t& index);
  void addEdge(const V& v1, const V& v2, const EdgeNode<>& en);
  void addEdge(const size_t& v1, const size_t& v2, const EdgeNode<>& en);
  EdgeType getEdgeType(const V& v1, const V& v2);
  EdgeType getEdgeType(const size_t& v1, const size_t& v2);
  size_t getDegree(const V& v);
  vector<PAIR<V>> sortAllVertexByDegree();
  GraphMatrix<V> getComplement();
  vector<vector<V>> getCliques(uint32_t nClique);
  vector<vector<V>> getCliquesWithBA(uint32_t nClique);
  void backtrace(MCP &mcp, size_t i);
  void printClique(const vector<V>& clique);
  void printCliques(const vector<vector<V>>& clique);
  size_t getSize();
  bool isEmpty();
  bool existVertex(V &v);
  void updateIdxMap();

private:
  size_t getIndexOfVertex(const V& _vertex);

private:
  bool					isDirected;
  std::vector<V>			vertices;
  std::map<std::string, uint32_t>	vertex2IdxMap;
  std::vector<std::vector<EdgeNode<>>>	edge;
};

template<typename V>
GraphMatrix<V>::GraphMatrix()
{

}

template<typename V>
GraphMatrix<V>::GraphMatrix(const vector<V>& _vertex, size_t _size, bool _isDirected)
{
  this->vertices.resize(_size);
  this->edge.resize(_size);
  this->isDirected = _isDirected;

  for (size_t idx = 0; idx < _size; ++idx)
  {
    this->vertices[idx] = _vertex[idx];
    this->edge[idx].resize(_size);

    vertex2IdxMap.insert(make_pair(vertices[idx], idx));
  }
}

template<>
GraphMatrix<VertexNode>::GraphMatrix(const vector<VertexNode>& _vertex, size_t _size, bool _isDirected)
{
  this->vertices.resize(_size);
  this->edge.resize(_size);
  this->isDirected = _isDirected;

  for (size_t idx = 0; idx < _size; ++idx)
  {
    this->vertices[idx] = _vertex[idx];
    this->edge[idx].resize(_size);

    if (vertices[idx].name.empty())
      {
	vertices[idx].genName();
      }
    vertex2IdxMap.insert(make_pair(vertices[idx].name, idx));
  }
}

template<typename V>
vector<V> GraphMatrix<V>::getAdjVertex(const V& v)
{
  vector<V> adjVertex;
  size_t index = getIndexOfVertex(v);
  size_t size = edge[index].size();
  for (size_t i = 0; i < size; i++)
    {
      if (edge[index][i].weight != 0 && index != i) adjVertex.push_back(vertices[i]);
    }

  return adjVertex;
}

template<typename V>
void GraphMatrix<V>::delVertex(const V& v)
{
  size_t index = getIndexOfVertex(v);
  delVertex(index);
}

template<typename V>
void GraphMatrix<V>::delVertex(const size_t& index)
{
  vertices.erase(vertices.begin() + index);
  vertices.shrink_to_fit();

  edge.erase(edge.begin() + index);
  edge.shrink_to_fit();
  for (vector<EdgeNode<>> &vec : edge)
    {
      vec.erase(vec.begin() + index);
      vec.shrink_to_fit();
    }

  updateIdxMap();
}

template<typename V>
void GraphMatrix<V>::addEdge(const V& v1, const V& v2, const EdgeNode<>& en)
{
  size_t start = getIndexOfVertex(v1);
  size_t end = getIndexOfVertex(v2);
  addEdge(start, end, en);
}

template<typename V>
void GraphMatrix<V>::addEdge(const size_t& v1, const size_t& v2, const EdgeNode<>& en)
{
  assert(v1 != v2);

  edge[v1][v2] = en;
  if (!isDirected)
      edge[v2][v1] = en;
}

template<typename V>
EdgeType GraphMatrix<V>::getEdgeType(const V& v1, const V& v2)
{
  size_t start = getIndexOfVertex(v1);
  size_t end = getIndexOfVertex(v2);
  return getEdgeType(start, end);
}

template<typename V>
EdgeType GraphMatrix<V>::getEdgeType(const size_t& v1, const size_t& v2)
{
  assert(v1 != v2);
  return edge[v1][v2].type;
}

template<typename V>
void GraphMatrix<V>::printEdge()
{
  assert(!vertices.empty());

  size_t maxSize = 7;
//  if (std::is_same<V, std::string>())
//    {
//      for (V v : vertices)
//	{
//	  size_t length = ((std::string)v).length();
//	  if (length > maxSize) maxSize = length;
//	}
//      maxSize += 1;
//      std::cout << std::setw(maxSize) << " ";
//    }

  for (size_t idx = 0; idx < vertices.size(); ++idx)
    {
//      std::cout << std::setw(maxSize) <<  vertices[idx];
      std::cout << " " <<  vertices[idx];
    }

  std::cout << endl;

  for (size_t idx_row = 0; idx_row < edge.size(); ++idx_row)
  {
      std::cout << std::setw(maxSize) << vertices[idx_row];
      for (size_t idx_col = 0; idx_col < edge.size(); ++idx_col)
      {
//	  std::cout << std::setw(maxSize) << edge[idx_row][idx_col] ;
	  std::cout << " " << edge[idx_row][idx_col] ;
      }
      std::cout << endl;
  }
  std::cout << endl;
}

template<typename V>
size_t GraphMatrix<V>::getDegree(const V& v)
{
  size_t degree = 0;
  size_t index = getIndexOfVertex(v);
  for (EdgeNode<> en : edge[index])
    {
      if (en.weight != 0) degree++;
    }
  return degree;
}

template<typename V>
vector<PAIR<V>> GraphMatrix<V>::sortAllVertexByDegree()
{
  vector<PAIR<V>> result;
  for (V vertex : vertices)
    {
      PAIR<V> pair = make_pair(vertex, getDegree(vertex));
      result.push_back(pair);
    }
  sort(result.begin(), result.end(), CmpByValue<V>());
  return result;
}

template<typename V>
GraphMatrix<V> GraphMatrix<V>::getComplement()
{
  size_t size = vertices.size();
  GraphMatrix<V> complement(vertices, size);
  for(size_t i = 0; i < size; i++)
    {
      for(size_t j = 0; j < size; j++)
	{
	  if (edge[i][j].weight == 0 && i != j)
	    {
	      EdgeNode<> en = {.type = EdgeType::NOT_SET, .weight = 1};
	      complement.addEdge(i, j, en);
	    }
	}
    }
  return complement;
}

template<typename V>
vector<vector<V>> GraphMatrix<V>::getCliques(uint32_t nClique)
{
  GraphMatrix<V> graphCopy = *this;
  vector<vector<V>> cliques;

  for (uint32_t i = 0; i < nClique && !graphCopy.isEmpty(); i++)
    {
      vector<V> clique;
      GraphMatrix<V> complement = graphCopy.getComplement();
      vector<PAIR<V>> sortedVertex = complement.sortAllVertexByDegree();

      size_t size1 = sortedVertex.size();
      for (size_t i = 0; i < size1 && !complement.isEmpty(); i++)
	{
	  V v = sortedVertex[i].first;
	  if (!complement.existVertex(v)) continue;
	  clique.push_back(v);
	  vector<V> adjVertex = complement.getAdjVertex(v);
	  complement.delVertex(v);
	  for (V v : adjVertex)
	    {
	      complement.delVertex(v);
	    }
	}
      cliques.push_back(clique);

      for (V v : clique)
	{
	  graphCopy.delVertex(v);
	}
    }

  return cliques;
}

template<typename V>
vector<vector<V>> GraphMatrix<V>::getCliquesWithBA(uint32_t nClique)
{
  GraphMatrix<V> graphCopy = *this;
  vector<vector<V>> cliques;

  for (uint32_t i = 0; i < nClique && !graphCopy.isEmpty(); i++)
    {
      vector<V> clique;

      size_t size = graphCopy.getSize();
      MCP mcp;
      mcp.bestx.resize(size);
      mcp.x.resize(size);
      mcp.bestn=0;
      mcp.cnum=0;

      graphCopy.backtrace(mcp, 0);
      for (size_t j = 0; j < size; j++)
	{
	  if (mcp.bestx[j] == 1)
	    {
	      clique.push_back(vertices[j]);
	    }
	}

      cliques.push_back(clique);

      for (V v : clique)
	{
	  graphCopy.delVertex(v);
	}
    }

  return cliques;
}

template<typename V>
void GraphMatrix<V>::backtrace(MCP &mcp, size_t i)
{
  size_t size = getSize();
  if (i >= size)
    {
      for (size_t j = 0; j < size; j++)
	{
	  mcp.bestx[j] = mcp.x[j];
	}
      mcp.bestn =mcp.cnum;
      return;
    }
  uint32_t OK = 1;
  for (size_t j = 0; j <= i; j++)
    if (mcp.x[j] == 1 && edge[i][j].weight == 0)
      {
	OK = 0;
	break;
      }
  if (OK)
    {
      mcp.x[i] = 1;
      mcp.cnum++;
      backtrace(mcp, i + 1);
      mcp.x[i] = 0;
      mcp.cnum--;
    }
  if (mcp.cnum + size - (i + 1) > mcp.bestn)
    {
      mcp.x[i] = 0;
      backtrace(mcp, i + 1);
    }
}

template<typename V>
void GraphMatrix<V>::printClique(const vector<V>& clique)
{
  std::cout << "clique:";
  for (V v : clique)
    {
      std::cout << v << " ";
    }
  std::cout << endl;
}

template<typename V>
void GraphMatrix<V>::printCliques(const vector<vector<V>>& cliques)
{
  for (vector<V> clique : cliques)
    {
      printClique(clique);
    }
}

template<typename V>
size_t GraphMatrix<V>::getSize()
{
  return vertices.size();
}

template<typename V>
bool GraphMatrix<V>::isEmpty()
{
  return vertices.size() == 0;
}

template<typename V>
bool GraphMatrix<V>::existVertex(V &v)
{
  return vertex2IdxMap.count(v) != 0;
}

template<>
bool GraphMatrix<VertexNode>::existVertex(VertexNode &v)
{
  return vertex2IdxMap.count(v.name) != 0;
}

template<typename V>
size_t GraphMatrix<V>::getIndexOfVertex(const V& v)
{
  return vertex2IdxMap.at(v);
}

template<>
size_t GraphMatrix<VertexNode>::getIndexOfVertex(const VertexNode& v)
{
  return vertex2IdxMap.at(v.name);
}

template<typename V>
void GraphMatrix<V>::updateIdxMap()
{
  vertex2IdxMap.clear();
  uint32_t size = getSize();
  for (size_t i = 0; i < size; ++i)
  {
    vertex2IdxMap.insert(make_pair(vertices[i], i));
  }
}

template<>
void GraphMatrix<VertexNode>::updateIdxMap()
{
  vertex2IdxMap.clear();
  uint32_t size = getSize();
  for (size_t i = 0; i < size; ++i)
  {
    if (vertices[i].name.empty())
      {
	vertices[i].genName();
      }
    vertex2IdxMap.insert(make_pair(vertices[i].name, i));
  }
}

#if 0
/**
 * adjacency list
 */
template <typename E>
struct EdgeNode
{
  size_t	startIndex;
  size_t	endIndex;
  uint32_t	type;
  W		weight;
  EdgeNode<E>*	nextNode;
  EdgeNode(size_t start, size_t end, uint32_t type, const W& _weight)
      : startIndex(start)
      , endIndex(end)
      , type(type)
      , weight(_weight)
      , nextNode(nullptr)
  {}
};

template <typename V>
class GraphLink
{
public:
  typedef EdgeNode<E> node;

  GraphLink(const vector<V>& _vertex, size_t _size, bool _isDirected = false);
  void printEdge();

  void delVertex(const size_t& index);
  void addEdge(const V& v1, const V& v2, const uint32_t& type = 1, const W& weight = 1);
  void addEdge(const size_t& startIndex, const size_t& endIndex, const uint32_t& type = 1, const W& weight = 1);
  size_t getDegree(const V& v);
  vector<PAIR<V>> sortAllVertexByDegree();
  size_t minDegree();

  GraphMatrix<V> getAdjMatrix();
  GraphLink<V> getComplement();

  vector<GraphLink<V>> getCliques();

private:
  size_t getIndexOfVertex(const V& v);
  void __addEdge(size_t startIndex, size_t endIndex, const uint32_t& type, const W& weight);

private:
  bool         isDirected;
  vector<V>      vertices;
  vector<node*> linkTable;
};

// implementation
/*
*   public function
*/

template<typename V>
GraphLink<V>::GraphLink(const vector<V>& _vertex, size_t _size, bool _isDirected)
{
    // init data
    this->vertices.resize(_size);
    this->linkTable.resize(_size);
    this->isDirected = _isDirected;

    for (size_t i = 0; i < _size; i++)
    {
        this->vertices[i] = _vertex[i];
    }
}

template<typename V>
void GraphLink<V>::printEdge()
{
    for (size_t idx = 0; idx < vertices.size(); ++idx)
    {
        std::cout << vertices[idx] << ": ";

        node* pEdge = linkTable[idx];
        while (pEdge)
        {
            std::cout << pEdge->weight << "[" << vertices[pEdge->endIndex] << "]-->";
            pEdge = pEdge->nextNode;
        }
        std::cout << "NULL" << endl;
    }
    std::cout << endl;
}

template<typename V>
void GraphLink<V>::delVertex(const size_t& index)
{
  vertices.erase(vertices.begin() + index);
  vertices.shrink_to_fit();

  node* head = linkTable[index];
  if (head != nullptr)
    {
      node* tmp = nullptr;
      while (head)
	{
	  tmp = head;
	  head = head->nextNode;
	  delete tmp;
	}
    }
  linkTable.erase(linkTable.begin() + index);
  linkTable.shrink_to_fit();
}

template<typename V>
void GraphLink<V>::addEdge(const V& v1, const V& v2, const uint32_t& type, const W& weight)
{
    addEdge(getIndexOfVertex(v1), getIndexOfVertex(v2), type, weight);
}

template<typename V>
void GraphLink<V>::addEdge(const size_t& startIndex, const size_t& endIndex, const uint32_t& type, const W& weight)
{
    assert( startIndex!=endIndex);
    __addEdge(startIndex, endIndex, type, weight);
    if (!isDirected)
        __addEdge(endIndex, startIndex, type, weight);
}

template<typename V>
size_t GraphLink<V>::getDegree(const V& v)
{
  size_t degree = 0;
  node* en;
  size_t index = getIndexOfVertex(v);
  en = linkTable[index];
  while (en)
    {
      degree++;
      en = en->nextNode;
    }
  return degree;
}

template<typename V>
vector<PAIR<V>> GraphLink<V>::sortAllVertexByDegree()
{
  vector<PAIR<V>> result;
  for (V vertex : vertices)
    {
      PAIR<V> pair = make_pair(vertex, getDegree(vertex));
      result.push_back(pair);
    }
  sort(result.begin(), result.end(), CmpByValue<V>());
  return result;
}

template<typename V>
GraphMatrix<V> GraphLink<V>::getAdjMatrix()
{
  GraphMatrix<V> matrix(vertices, vertices.size());
  for (node* link : linkTable)
    {
      while(link)
	{
	  matrix.addEdge(link->startIndex, link->endIndex);
	  link = link->nextNode;
	}
    }

  return matrix;
}

template<typename V>
GraphLink<V> GraphLink<V>::getComplement()
{
  GraphLink<V> graphLinkComplement(vertices, vertices.size());
  GraphMatrix<V> matrix = getAdjMatrix();

  size_t size = vertices.size();
  for(size_t i = 0; i < size; i++)
    {
      for(size_t j = 0; j < size; j++)
	{
	  if (matrix.getEdgeWeight(i, j) == 0 && (i != j))
	    {
	      graphLinkComplement.addEdge(i, j);
	    }
	}
    }

  return graphLinkComplement;
}

template <typename V>
void GraphLink<V>::__addEdge(size_t startIndex, size_t endIndex, const uint32_t& type, const W& weight)
{
    // head insertion to add edge
    node* pNewEdge = new node(startIndex, endIndex, type, weight);
    pNewEdge->nextNode = linkTable[startIndex];
    linkTable[startIndex] = pNewEdge;
}

template<typename V>
size_t GraphLink<V>::getIndexOfVertex(const V& v)
{
    for (size_t idx = 0; idx < vertices.size(); idx++)
    {
        if (vertices[idx] == v)
            return idx;
    }

    // If it is not found, an error has occurred.
    assert(false);
    return -1;
}
#endif

#endif /* SCRATCH_VANET_CS_VFC_GRAPH_HPP_ */
