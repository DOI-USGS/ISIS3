#ifndef ControlGraph_h
#define ControlGraph_h

/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */


// Global forward declarations
template< class A, class B > class QHash;
template< class A, class B > class QMap;
template< class A, class B > class QPair;
template< class A> class QVector;
class QString;

namespace Isis
{

  // Isis forward declarations
  class ControlNet;
  class GroupedStatistics;

  /**
   * @brief Control Network statistics and connectivity
   *
   * This class is used to store statistics on a Control Network
   *
   * This class is include safe meaning that includers of this class will only
   * get this class.
   *
   * @ingroup ControlGraph
   *
   * @author Eric Hyer
   *   @history 2009-09-23 Eric Hyer Original version
   *   @history 2009-10-15 Eric Hyer Added GetCubeList Method
   *
   * @see ControlNet ControlPoint ControlMeasure GroupedStatistics
   *
   * @internal
   *                              
   */
  class ControlGraph 
  {
    public:
      ControlGraph(ControlNet * someControlNet);
      ControlGraph(const ControlGraph & other);
      ~ControlGraph();
      
      const bool IsConnected() const;
      const int GetIslandCount() const;
      const QVector< QString > GetCubesOnIsland(const int & island) const;
      const QVector< QString > GetCubeList() const;
      const GroupedStatistics & GetMeasureStats(const QString &
          CubeSerialNumber) const;
      
      ControlGraph & operator=(const ControlGraph & other);

    private:
      // nested class forward declaration
      class AdjacentCubeList;
      
      void HashCubesAndPopulateGraph(ControlNet * someControlNet);
      void CalculateIslands();

      //! ControlNet to make a graph from
      ControlNet * cnet;
      
      //! Used to get an index from a cube serial number
      QHash< QString, int > * cubeIdToIndexHash;
      
      //! Used to get a cube serial number from an index
      QHash< int, QString > * cubeIndexToIdHash;
      
      /**
       *  THE GRAPH!!  It is a map of cube indices to a pair.  The first pair
       *  element is a list that contains not only all the cubes which are
       *  adjacent to it (as indices also), but also the edges that make these
       *  these connections.  The second pair element contains statistics on
       *  this cube.
       */
      QMap< int, QPair< AdjacentCubeList, GroupedStatistics > > * graph;
      
      /**
       *  Stores the state of the graphs connectivity so that connectivity must
       *  only be calculated once.
       */
      bool connected;
      
      //! Stores a list of islands which are themselves a list of cube indices
      QVector< QVector< int > > * islands;
      
      /**
       * @brief Control Graph nested class
       *
       * This class is used to store adjacent cube connections for ControlGraph
       *
       * @ingroup ControlGraph
       *
       * @author 2009-09-18 Eric Hyer
       *
       * @see ControlGraph
       *
       * @internal
       *                              
       */
      class AdjacentCubeList
      {
        public:
          AdjacentCubeList(const int & cubeIndex, const int & cpIndex, const int
              & cmIndex);
          AdjacentCubeList(const AdjacentCubeList & other);
          ~AdjacentCubeList();
          
          const QVector< int > GetAdjacentCubes() const;
          void AddConnection(const int & cubeIndex, const int & cpIndex, const
              int & cmIndex);
          AdjacentCubeList & operator=(const AdjacentCubeList & other);
           
        private:
          //! stores all edges or connections for an adjacent cube
          QMap< int, QVector< QPair< int, int > > > * connections;
      };
  };
};

#endif

