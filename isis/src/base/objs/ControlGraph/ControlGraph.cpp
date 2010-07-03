#include "IsisDebug.h"
#include "ControlGraph.h"

#include <string>
#include <iostream>
#include <utility>

#include <QHash>
#include <QMap>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QPair>

#include "ControlMeasure.h"
#include "GroupedStatistics.h"
#include "ControlNet.h"
#include "iException.h"
#include "iString.h"



namespace Isis
{

  /**
   * construct a ControlGraph given a ControlNet
   *
   * @param someControlNet ControlNet to construct a ControlGraph from
   */
  ControlGraph::ControlGraph(ControlNet * someControlNet)
  {
    cnet = someControlNet;
    cubeIdToIndexHash = NULL;
    cubeIndexToIdHash = NULL;
    graph = NULL;
    graph = new QMap< int, QPair< AdjacentCubeList, GroupedStatistics > >();

    // cubeIdToIndexHash provides a way of assigning unique sequential indices
    // to all of the Cube Serial numbers in the ControlNet, while
    // cubeIndexToIdHash prvides a way to get the Cube Serial numbers back.
    cubeIdToIndexHash = new QHash< QString, int >();
    cubeIndexToIdHash = new QHash< int, QString >();
    cubeIdToIndexHash->reserve(cnet->Size() / 5);
    cubeIndexToIdHash->reserve(cnet->Size() / 5);
    
    HashCubesAndPopulateGraph(someControlNet);
    
    CalculateIslands();
    
    if (islands->size())
      connected = false;
    else
      connected = true;
  }
  

  /**
   *  copy construct a ControlGraph object
   *
   *  @param other The ControlGraph to construct a copy of
   */
  ControlGraph::ControlGraph(const ControlGraph & other)
  {
    cnet = other.cnet;
    
    cubeIdToIndexHash = NULL;
    cubeIndexToIdHash = NULL;
    graph = NULL;
    islands = NULL;
    
    cubeIdToIndexHash = new QHash< QString, int >(*other.cubeIdToIndexHash);
    cubeIndexToIdHash = new QHash< int, QString >(*other.cubeIndexToIdHash);
    graph = new QMap< int, QPair< AdjacentCubeList, GroupedStatistics > >(
        *other.graph);
    islands = new QVector< QVector< int > >(*other.islands);
    
    connected = other.connected;
  }
  
 
  /**
   *  Destruct a ControlGraph
   */
  ControlGraph::~ControlGraph()
  {
    cnet = NULL;
    
    if (cubeIdToIndexHash)
    {
      delete cubeIdToIndexHash;
      cubeIdToIndexHash = NULL;
    }
    
    if (cubeIndexToIdHash)
    {
      delete cubeIndexToIdHash;
      cubeIndexToIdHash = NULL;
    }

    if (graph)
    {
      delete graph;
      graph = NULL;
    }
    
    if (islands)
    {
      delete islands;
      islands = NULL;      
    }
  }
  
  
  //! Returns true if this ControlGraph is connected or false otherwise
  const bool ControlGraph::IsConnected() const
  {
    return connected;
  }
  
  
  /**
   *  There can be 0 islands or 2 or more islands.  GetIslandCount will never
   *  return 1 since 1 island is really just a connected graph (with 0 islands).
   */
  const int ControlGraph::GetIslandCount() const
  {
    return islands->size();
  }
  
  
  /**
   *  @param island A list of all cubes that are on this island are desired
   *
   *  @returns A list of CubeSerialNumbers which are located on the given island
   */
  const QVector< QString > ControlGraph::GetCubesOnIsland(const int & island)
      const
  {
    if (connected)
    {
      std::string message = "\n\nGetCubesOnIsland called on connected graph ";
      message += "with no islands!!!\n\n";
      throw Isis::iException::Message(Isis::iException::Programmer, message,
                                      _FILEINFO_);
    }
    
    ASSERT(islands->size() != 0);
    
    if (island < 0 || island >= islands->size())
    {
      iString message = "\n\nA list of cubes was requested from island ";
      message += iString(island);
      message += "\nbut that island does not exist!!!";
      message += "\n\nThere are " + iString(islands->size()) + " islands ";
      message += "numbered from 0 to " + iString(islands->size() - 1) + "\n\n";
      throw Isis::iException::Message(Isis::iException::Programmer, message,
                                      _FILEINFO_);
    }
    
    QVector< QString > cubeList;
    for (int i = 0; i < (*islands)[island].size(); i++)
    {
      cubeList.push_back(cubeIndexToIdHash->value((*islands)[island][i]));
    }
    
    return cubeList;    
  }
  
  
  /**
   *  @returns A list of all CubeSerialNumbers in the given ControlNet
   */
  const QVector< QString > ControlGraph::GetCubeList() const
  {
    QVector< QString > cubeList;
    
    // for each key in the cubeIdToIndexHash add the key to a vector    
    QHash< QString, int >::const_iterator i = cubeIdToIndexHash->constBegin();
    while (i != cubeIdToIndexHash->constEnd())
    {
      cubeList.push_back(i.key());
      i++;
    }
    
    return cubeList;    
}
  
  
  /**
   *  @param CubeSerialNumber The Serial number of the cube to get Statistics on
   *
   *  @returns Statistics for all measures associated with the given cube
   */
  const GroupedStatistics & ControlGraph::GetMeasureStats(const QString &
      CubeSerialNumber) const
  {
    return graph->find(cubeIdToIndexHash->value(CubeSerialNumber)).value()
        .second;
  }

  
  /**
   *  @param other The ControlGraph on the right side of the =
   */
  ControlGraph & ControlGraph::operator=(const ControlGraph & other)
  {
    cnet = other.cnet;
    connected = other.connected;
    
    delete cubeIdToIndexHash;
    cubeIdToIndexHash = NULL;
    delete cubeIndexToIdHash;
    cubeIndexToIdHash = NULL;
    delete graph;
    graph = NULL;
    delete islands;
    islands = NULL;
    
    cubeIdToIndexHash = new QHash< QString, int >(*other.cubeIdToIndexHash);
    cubeIndexToIdHash = new QHash< int, QString >(*other.cubeIndexToIdHash);
    graph = new QMap< int, QPair< AdjacentCubeList, GroupedStatistics > >(
        *other.graph);
    islands = new QVector< QVector< int > >(*other.islands);
    
    return *this;
  }
  
  
  /**
   * @param someControlNet The ControlNet to create a ControlGraph from
   */
  void ControlGraph::HashCubesAndPopulateGraph(ControlNet * someControlNet)
  {
    // index assigned to last hashed cube (-1 means empty hash table)
    int cubeIndex = -1;
    
    // whats about to happen is this:
    //
    // for all ControlPoints in the given ControlNet
    //   for each ControlMeasure (cube) in the ControlPoint
    //     for all the other ControlMeasures (other cubes) in the ControlPoint
    //       add connection from previous for loops cube to this for loops cube
    //
    // along the way as we encounter new cubes they are hashed.  Also, for all
    // the measures encountered be the middle for loop statistics are saved.
    
    for (int cpIndex = 0; cpIndex < cnet->Size(); cpIndex++)
    {
      if (!(*cnet)[cpIndex].Ignore())
      {
        // use a reference for the current ControlPoint for clearity
        ControlPoint & curCtrlPoint = (*cnet)[cpIndex];
        for (int cmIndex = 0; cmIndex < curCtrlPoint.Size(); cmIndex++)
        {
          // get current cube's serial number and hash if new
          std::string temp = curCtrlPoint[cmIndex].CubeSerialNumber();
          QString curCube(temp.c_str());
          if (!cubeIdToIndexHash->contains(curCube))
          {
            cubeIdToIndexHash->insert(curCube, ++cubeIndex);
            cubeIndexToIdHash->insert(cubeIndex, curCube);
          }
          int curCubeIndex = cubeIdToIndexHash->value(curCube);

          QMap< int, QPair< AdjacentCubeList, GroupedStatistics >
              >::iterator graphIterator = graph->find(curCubeIndex);

          // look for adjacent cubes
          for (int cmIndex2 = 0; cmIndex2 < curCtrlPoint.Size(); cmIndex2++)
          {
            if (cmIndex2 != cmIndex)
            {
              // get adjacent cube's serial number and hash if new
              std::string temp = curCtrlPoint[cmIndex2].CubeSerialNumber();
              QString adjacentCube(temp.c_str());
              if (!cubeIdToIndexHash->contains(adjacentCube))
              {
                cubeIdToIndexHash->insert(adjacentCube, ++cubeIndex);
                cubeIndexToIdHash->insert(cubeIndex, adjacentCube);
              }
              int adjCubeIndex = cubeIdToIndexHash->value(adjacentCube);
              
              // add a connection from the current cube to the adjacent cube
              if (graphIterator != graph->end())
              {
                graphIterator.value().first.AddConnection(adjCubeIndex, cpIndex,
                                                          cmIndex2);
              }
              else
              {
                AdjacentCubeList newCubeList(adjCubeIndex, cpIndex, cmIndex2);
                GroupedStatistics cmStats;
                graph->insert(curCubeIndex, qMakePair(newCubeList, cmStats));
              }
            }
          } // of for all measures in cp

          // save off statistics
          if (graphIterator != graph->end())
          {
            QVector< QString > dataNames((*cnet)[cpIndex][cmIndex]
                .GetMeasureDataNames());
            
            for (int i = 0; i < dataNames.size(); i++)
              graphIterator.value().second.AddStatistic(dataNames[i],
                  (*cnet)[cpIndex][cmIndex].GetMeasureData(dataNames[i]));
          } // of saving statistics
        } // of for all measures in cp
      } // of if not an igrored point
    } // of for all ControlPoints in net
  } // of HashCubesAndPopulateGraph
  
  
  /**
   *  Determines whether or not islands exist and calculates what they are if
   *  present
   */
  void ControlGraph::CalculateIslands()
  {
    // assume subgraphs exist! (check assumption at the very end of the method)
    
    // A search list has a value for every cube, which defaults to false.
    // A breadth-first search is used to test connectivity and works by setting
    // each cubes cooresponding search list value to true as it is visited.
    // At the end of the first round the true entries make up the first
    // subgragh.  As they are added to the first subgraph they are removed from
    // the search list.  The remaining false entries must have the breadth-first
    // search done on them to determine the next subgraph(s).  This process
    // continues until all entries in the search list are true (until all cubes
    // have been visited)
    QMap< int, bool > searchList;
    for (int i = 0; i < graph->size(); i++)
      searchList.insert(i, false);
  
    // For each subgraph keep a list of the cubes in the subgraph.  This is 
    // represented by a 2d vector where the inner vectors are cubes within a
    // subgraph and the outer vector is a list of subgraphs
    islands = new QVector< QVector< int > >();
    
    // keeps track of which itteration of the breadth-first search we are on and
    // thus also which subgraph we are currently populating
    int subgraphIndex = -1;
    
    while (searchList.size())
    {
      // create a new subgraph
      subgraphIndex++;
      islands->push_back(QVector< int >());
      
      // The queue used for breadth-first searching
      QQueue< int > q;
      
      // visit the first cube
      searchList.begin().value() = true;
      q.enqueue(searchList.begin().key());
      
      // visit all cubes possible using the breadth-first approach
      while (q.size())
      {
        int curVertex(q.dequeue());
        QVector< int > adjacentVertices = graph->find(curVertex).value().first
            .GetAdjacentCubes();
   
        for (int i = 0; i < adjacentVertices.size(); i++)
        {
          const int & curNeighbor = adjacentVertices[i];
          
          ASSERT(searchList.find(curNeighbor) != searchList.end());
          
          if (!searchList.find(curNeighbor).value())
          {
            searchList.find(curNeighbor).value() = true;
            q.enqueue(curNeighbor);
          }
        }
      } // end of breadth-first search
      
      // add all true entries to the current subgraph
      QMap< int, bool >::iterator i = searchList.begin();
      while (i != searchList.end())
      {
        if (i.value())
        {
          (*islands)[subgraphIndex].push_back(i.key());
        }
        i++;
      }
      
      // remove all the true entries from the search list
      for (int i = 0; i < (*islands)[subgraphIndex].size(); i++)
      {
        searchList.remove((*islands)[subgraphIndex][i]);
      }
    }
    
    // if there was only ever one subgraph created then the initial assumption
    // was wrong!  There are no islands at all - this is a connected graph!
    if (subgraphIndex == 0)
    {
      islands->clear();
    }
  }
  
  
  /**
   *  Construct a new AdjacentCubeList given one initial adjacent connection.
   *  An adjacent connection means both an adjacent vertex as well as the edge
   *  that connects it.  The cubeIndex is the vertex.  The edge is a
   *  ControlPoint - ControlMeasure combo.
   *
   *  @param cubeIndex First adjacent cube
   *  @param cpIndex ControlPoint Index
   *  @param cmIndex ControlMeasure Index
   */
  ControlGraph::AdjacentCubeList::AdjacentCubeList(const int & cubeIndex,
      const int & cpIndex, const int & cmIndex)
  {
    connections = NULL;
    
    QVector< QPair< int, int > > firstEdge;
    firstEdge.push_back(qMakePair(cpIndex, cmIndex));
    connections = new QMap< int, QVector< QPair< int, int > > >();
    connections->insert(cubeIndex, firstEdge);
  }
  
  
  /**
   *  copy construct an AdjacentCubeList
   *
   *  @param other The AdjacentCubeList to construct a copy of
   */
  ControlGraph::AdjacentCubeList::AdjacentCubeList(const AdjacentCubeList &
      other)
  {
    connections = NULL;
    connections = new QMap< int, QVector< QPair< int, int > > >(
        *other.connections);
  }
  
  
  //! destruct an AdjacentCubeList
  ControlGraph::AdjacentCubeList::~AdjacentCubeList()
  {
    if (connections)
    {
      delete connections;
      connections = NULL;
    }
  }
  
  
  /**
   *  @returns A list of adjacent cubes!
   */
  const QVector< int > ControlGraph::AdjacentCubeList::GetAdjacentCubes() const
  {
    // vector of adjacent cubes to be returned
    QVector< int > adjacentCubes;
    
    if (!connections)
      return adjacentCubes; 
    
    QMap< int, QVector< QPair< int, int > > >::const_iterator i =
        connections->constBegin();
    while (i != connections->constEnd())
    {
      adjacentCubes.push_back(i.key());
      i++;
    }
    
    return adjacentCubes;
  }
  
  
  /**
   *  Adds a connection to an AdjacentCubeList.  A connection consists of a new
   *  vertex as well as the edge that connects it.  The vertex is the cube index
   *  and the edge is the ControlPoint - ControlMeasure combo.
   *
   *  @param cubeIndex Adjacent cube
   *  @param cpIndex ControlPoint index
   *  @param cmIndex ControlMeasure index
   */
  void ControlGraph::AdjacentCubeList::AddConnection(const int & cubeIndex,
      const int & cpIndex, const int & cmIndex)
  {
    QMap< int, QVector< QPair< int, int > > >::iterator i =
        connections->find(cubeIndex);
    
    // if the cube already exists in our list then just add another edge to it.
    // otherwise we need to add the cube as well.
    if (i != connections->end())
    {
      i.value().push_back(qMakePair(cpIndex, cmIndex));
    }
    else
    {
      QVector< QPair< int, int > > firstEdge;
      firstEdge.push_back(qMakePair(cpIndex, cmIndex));
      connections->insert(cubeIndex, firstEdge);
    }
  }
  
  
  /**
   *  @param other The AdjacentCubeList on the right side of the =
   */
  ControlGraph::AdjacentCubeList & ControlGraph::AdjacentCubeList::operator=(
      const AdjacentCubeList & other)
  {
    delete connections;
    connections = NULL;
    
    connections = new QMap< int, QVector< QPair< int, int > > >(
        *other.connections);
    
    return *this;
  }
}
