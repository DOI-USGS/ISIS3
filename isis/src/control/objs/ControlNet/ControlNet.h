#ifndef ControlNet_h
#define ControlNet_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// This is needed for the QVariant macro
#include <QMetaType>
#include <QObject> // parent class
#include <QSharedPointer>
#include "SurfacePoint.h"
#include <QString>
#include <QMap>
#include <QVariant>
#include <QVector>
#include <QVariant>


// Boost includes
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "ControlMeasure.h"
#include "ControlPoint.h"

template< typename A, typename B > class QHash;
template< typename T > class QList;
template< typename A, typename B > struct QPair;
template< typename T > class QSet;

class QMutex;
class QString;

namespace Isis {
  class Camera;
  class ControlMeasure;
  class ControlPoint;
  class Distance;
  class Progress;
  class Pvl;
  class SerialNumberList;

  /**
   * @brief a control network
   *
   * This class is used to store a network of ControlPoints
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Updated unitTest
   *   @history 2006-06-22 Brendan George Updated to conform to changes in
   *                           SerialNumberList class
   *   @history 2008-04-04 Christopher Austin Added Exists function
   *   @history 2008-04-18 Debbie A. Cook Added Progress reports to loading and
   *                           SetImages and calculates the total number of
   *                           measurements in the control net
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2009-01-06 Jeannie Walldren Fixed typo in SetImages() exception
   *                           output. Added documentation.
   *   @history 2009-02-05 Christopher Austin when the created date or the
   *                           modified date are not set, they default to the
   *                           time in which Write() is called.
   *   @history 2009-04-07 Tracie Sucharski Added NumValidMeasures and
   *                           NumIgnoredMeasures methods.
   *   @history 2009-06-03 Christopher Austin Added p_invalid functionality
   *                           along with forceBuild, as well as other small
   *                           fixes including documentation.
   *   @history 2009-07-13 Stacy Alley The std::vector of ControlPoints called
   *                           'p_points' was replaced with a QVector of QString
   *                           'p_pointIds' in conjunction with a QHash of
   *                           <QString, ControlPoint> called 'p_pointsHash'.
   *                           This was done to speed up the Add method which
   *                           was essentially slowing down the reading or
   *                           creation of Control Networks.
   *   @history 2010-05-06 Tracie Sucharski Use defaults of 0. instead of
   *                           Isis::Null, because 0. is the default in the
   *                           protocol buffers.
   *   @history 2010-08-06 Tracie Sucharski Updated for changes made after
   *                           additional working sessions for Control network
   *                           design.
   *   @history 2009-09-01 Eric Hyer Added two includes: QVector and QString
   *   @history 2010-09-09 Sharmila Prasad Added API to sort Control Net by
   *                           Point ID Changed PointID's vector to StringList
   *   @history 2009-09-25 Travis Addair Changed methods which return the number
   *                           of control measures in the network to compute
   *                           those values at the time the method is called,
   *                           not when the control network is first
   *                           initialized.
   *   @history 2010-10-05 Tracie Sucharski Renamed the Write method to
   *                           WritePvl.  Create new method, Write which takes
   *                           another parameter indicating whether to write pvl
   *                           format or binary format, The default will write
   *                           binary.
   *   @history 2010-10-05 Eric Hyer ControlMeasure and ControlPoint now
   *                           return QStrings for some methods.  Fixed
   *                           breakages caused by this.
   *   @history 2010-10-06 Sharmila Prasad Added method to get CreatedDate
   *   @history 2010-11-21 Tracie Sucharski - Added new keyword, jigsawRejected
   *                           to the read and write methods.
   *   @history 2011-01-13 Mackenzie Boyd Added copy constructor and assignment
   *                           operator.
   *   @history 2011-01-17 Eric Hyer - Points are now owned and deleted by the
   *                           network.  Network now stored in such a way that
   *                           access to all points in a cube is just as cheap
   *                           as accessing measures in a point.  Removed
   *                           redundant methods and made other api changes.
   *   @history 2011-02-18 Eric Hyer - Made improvements and bug fixes related
   *                           to interaction to other control network classes
   *                           including ControlPoint and ControlMesure, but
   *                           most significantly to ControlCubeGraphNode.  Most
   *                           important fix was network notification of
   *                           measures added to or removed from points after
   *                           the point is added to the network.
   *   @history 2011-02-23 Eric Hyer - Added some methods to support graphing
   *                           calculations (RandomBFS, Shuffle, CalcBWAndCE,
   *                           and GetNodeConnections).
   *   @history 2011-03-08 Ken Edmundson - Added methods
   *                           GetNumberOfMeasuresInImage,
   *                           GetNumberOfJigsawRejectedMeasuresInImage,
   *                           IncrementNumberOfRejectedMeasuresInImage,
   *                           DecrementNumberOfRejectedMeasuresInImage, and
   *                           members p_cameraMeasuresMap and
   *                           p_cameraRejectedMeasuresMap.
   *   @history 2011-03-12 Debbie A. Cook - Added member p_targetRadii and
   *                           method GetTargetRadii to support SurfacePoint
   *                           sigma conversions in ControlPoint.
   *   @history 2011-03-14 Eric Hyer - Cube connection graph now updated when
   *                           points or measures are ignored.
   *   @history 2011-03-15 Eric Hyer - Some cube graph bugs were fixed.
   *   @history 2011-03-25 Christopher Austin - Added UpdatePointReference() to
   *                           work with ControlPoint's SetId()
   *   @history 2011-03-29 Steven Lambright - Made versioning viable for first
   *                           release.
   *   @history 2011-04-27 Steven Lambright - UpdatePointReference had a bug
   *                           where the pointIds list was not being correctly
   *                           updated.
   *   @history 2011-05-04 Eric Hyer - References in graph data structure now
   *                           kept for ignored points and measures
   *   @history 2011-05-25 Eric Hyer - Added getPoints method
   *   @history 2011-06-22 James Alexander Crough and Eric Hyer - Added
   *                           getGraphNode method.
   *   @history 2011-06-28 Eric Hyer - Added getEdgeCount() method
   *   @history 2011-07-08 Travis Addair - Locked points can no longer be
   *                           deleted
   *   @history 2011-07-27 Sharmila Prasad - Fixed bug in GetNumEditLockMeasures()
   *   @history 2011-08-01 Eric Hyer - Addressed some graphing issues
   *   @history 2011-08-24 Steven Lambright - Fixed bug in graph code
   *   @history 2011-10-06 Steven Lambright - Having a target is no longer
   *                           required.
   *   @history 2011-10-14 Ken Edmundson Added method ClearJigsawRejected(); to
   *                           set all measure/point JigsawRejected flags to
   *                           false prior to bundle adjustment.
   *   @history 2012-04-13 Orrin Thomas Added method sortedMeasureList, and functor
   *                           ControlMeasureLessThanFunctor.
   *   @history 2012-11-22 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2012-09-11 Tracie Sucharski - Added a Mutex and MutexLocker for the SetTarget
   *                           method.
   *   @history 2012-09-26 Steven Lambright - Fixed assignment operator to return a non-const
   *                           ControlNet reference. Added swap(). Refactored assignment
   *                           operator to use copy-and-swap idiom which fixed a bug where
   *                           you couldn't access control points by index after an assignment.
   *   @history 2013-11-08 Tracie Sucharski - Add camera to ignored measures.  References #1603.
   *   @history 2013-12-18 Tracie Sucharski - Previous change caused problems for jigsaw app.  It
   *                           was expecting the method GetNumberOfMeasuresInImage to only return
   *                           the number of VALID (Ignore=False) measures.  Renamed
   *                           method to GetNumberOfValidMeasuresInImage and the private
   *                           variable p_cameraMeasuresMap to p_cameraValidMeasuresMap. References
   *                           #1603.
   *   @history 2015-09-05 Ken Edmundson - Added QSharedPointer and typedef for later use.
   *   @history 2016-02-15 Kris Becker - Added feature to take ownership of
   *                           points from ControlNet. To support this option,
   *                           added clear() and take() methods.
   *                           (Merged by Kristin Berry. References #2392)
   *   @history 2016-04-22 Jeannie Backer - Added try/catch to SetTarget()'s call to
   *                           TProjection::TargetRadii(). If an error is thrown, the radii are
   *                           now set to Isis:Null. Added SetTarget(Pvl), SetTarget(ControlNet),
   *                           and SetTarget(QString, vector) methods to attempt to read radii from
   *                           various sources, if not found using the TargetName. References #3892
   *   @history 2016-04-28 Tracie Sucharski - Updated toAscii() calls to toLatin1() for QT 5 upgrade.
   *   @history 2016-05-10 Jeannie Backer - Replaced calls to TProjection::TargetRadii() with calls
   *                           to Target::radiiGroup(). References #3934
   *   @history 2016-06-21 Kris Becker - Properly forward declare QPair as struct not class
   *   @history 2016-06-08 Jesse Mapel - Merged changes from IPCE to ISIS.  Fixes #3948.
   *   @history 2016-10-13 Ian Humphrey - Added check to SetImages() to make sure it isn't called
   *                           more than once, which messes up the p_cameraList member. References
   *                           #4293.
   *   @history 2017-08-09 Summer Stapleton - Added throw to caught exception for bad control net
   *                           import in constructor. Also removed p_invalid as it was no longer
   *                           being used anywhere. Fixes #5068.
   *   @history 2017-08-30 Debbie A. Cook - Added an optional argument to the constructor
   *                           for the control point coordinate type.  At this point this value is only
   *                           stored in the active ControlNet.  It will be added to the stored
   *                           ControlNet at a later date.  References #4649 and #501.
   *   @history 2017-12-12 Kristin Berry - Updated to use QMap and QVector rather than std::map
   *                           and std::vector. Fixes #5259.
   *   @history 2017-12-18 Adam Goins - Added GetLastModified() accessor. References #5258.
   *   @history 2017-12-21 Jesse Mapel - Modified read and write methods to use the refactored
   *                           ControlNetVersioner instead of directly parsing the protobuf
   *                           objects from the LatestControlNetFile.
   *   @history 2018-01-12 Adam Goins - Added Progress support back to Read methods.
   *   @history 2017-01-19 Jesse Mapel - Added a method to get all of the valid measures in an
   *                           image. Previously, this had to be done throug the graph.
   *   @history 2018-01-26 Kristin Berry - Added pointAdded() function to eliminate redundant measure
   *                           adds to the control network.
   *   @history 2018-04-05 Adam Goins - Added a check to the versionedReader targetRadii
   *                           group to set radii values to those ingested from the versioner
   *                           if they exist. Otherwise, we call SetTarget with the targetname.
   *                           Fixes #5361.
   *   @history 2018-06-10 Kristin Berry - Removed unused methods and associated code:
   *                           MinimumSpanningTree(), GetNodeConnections(), RandomBFS(), Shuffle(),
   *                           CalcBWAndCE(), CubeGraphToString(), getGraphNode(). References #5434
   *   @history 2018-06-10 Kristin Berry - Updated to use the boost graph library instead of our
   *                           custom graph structure ControlCubeGraphNode.
   *   @history 2018-06-06 Jesse Mapel - Added a method to get all adjacent images to ControlNet.
   *                           Previously this functionality was only available through the
   *                           ControlCubeGraphNode class. References #5434.
   *   @history 2018-06-06 Jesse Mapel - Added a point ignored and un-ignored methods. This will
   *                           prevent edge strengths getting incremented or decremented twice.
   *                           References #5434.
   *   @history 2018-06-15 Adam Goins & Jesse Mapel - Added the ModType enum, as well as a series
   *                           of signals that are emitted whenever a change is made to a
   *                           Control Point or any of it's measures, or to the network itself.
   *                           These signals exist for the purpose of communication between the
   *                           ControlNetVitals class, and the network that it is observing.
   *                           Fixes #5435.
   *  @history 2018-06-22 Ken Edmundson - Added typedef for QSharedPointer to control network.
   *  @history 2018-06-25 Kristin Berry - Updated GetNumberOfValidMeasuresInImage() to use
   *                           GetValidMeasuresInCube() if SetImage has not yet been called to populate
   *                           the p_cameraValidMeasuresMap.
   *   @history 2018-06-25 Jesse Mapel - Fixed the incorrect signal being called when adding and
   *                           removing measures. References #5435.
   *   @history 2018-06-25 Jesse Mapel - Fixed ignoring measures with ignored adjacent measures
   *                           incorrectly modifying the edge between the two image vertices.
   *   @history 2018-06-29 Kristin Berry - Added addEdge() and removeEdge() functions to make
   *                           code cleaner.
   *   @history 2018-07-06 Jesse Mapel - Modified addEdge and removeEdge to always emit a graph
   *                           modified signal if an edge is added or removed. Added graph
   *                           modified signal when a vertex is added.
   *   @history 2018-07-06 Jesse Mapel - Removed target radii from ControlNet objects because
   *                           SurfacePoints now use their local radii to do sigma distance
   *                           conversions instead of the target equatorial and polar radii.
   *                           Fixes #5457.
   *   @history 2018-07-22 Kristin Berry - Updated swap to include the graph and vertex map.
   */
  class ControlNet : public QObject {
      Q_OBJECT

      friend class ControlMeasure;
      friend class ControlPoint;

    public:

      /**
       *  @brief Control Point Modification Types
       *
       *  This enum is designed to represent the different types of modifications that can be
       *  made to a ControlNet.
       *
       *  Swapped means the network was swapped with another network (ControlNet::Swap(ControlNet &other)).
       *  GraphModified means that a vertice or edge was added/removed from the graph..
       */
      enum ModType {
        Swapped,
        GraphModified
      };

      ControlNet(SurfacePoint::CoordinateType = SurfacePoint::Latitudinal);
      ControlNet(const ControlNet &other);
      ControlNet(const QString &filename, Progress *progress = 0,
                 SurfacePoint::CoordinateType = SurfacePoint::Latitudinal);

      ~ControlNet();

      void clear();
      QList< ControlPoint * > take();

      void ReadControl(const QString &filename, Progress *progress = 0);
      void Write(const QString &filename, bool pvl = false);

      void AddPoint(ControlPoint *point);
      int DeletePoint(ControlPoint *point);
      int DeletePoint(QString pointId);
      int DeletePoint(int index);
      bool ContainsPoint(QString pointId) const;

      QList< QString > GetCubeSerials() const;
      QString GraphToString() const;
      QList< QList< QString > > GetSerialConnections() const;
      int getEdgeCount() const;
      QList< QString > getAdjacentImages(QString serialNumber) const;
      QList< ControlMeasure * > GetMeasuresInCube(QString serialNumber);
      QList< ControlMeasure * > GetValidMeasuresInCube(QString serialNumber);
      QList< ControlMeasure * > sortedMeasureList(double(ControlMeasure::*statFunc)() const,
                                                  double min,double max);

      void ComputeResiduals();
      void ComputeApriori();

      const ControlPoint *GetPoint(QString pointId) const;
      ControlPoint *GetPoint(QString pointId);
      const ControlPoint *GetPoint(int index) const;
      ControlPoint *GetPoint(int index);

      double AverageResidual();
      Isis::Camera *Camera(int index);
      Isis::Camera *Camera(QString serialNumber);                                                                       
      QString CreatedDate() const;
      QString Description() const;
      ControlPoint *FindClosest(QString serialNumber,
          double sample, double line);
      //bool IsValid() const;
      double GetMaximumResidual();
      QString GetNetworkId() const;
      int GetNumEditLockMeasures();
      int GetNumEditLockPoints();
      int GetNumIgnoredMeasures();
      int GetNumberOfValidMeasuresInImage(const QString &serialNumber);
      int GetNumberOfJigsawRejectedMeasuresInImage(const QString &serialNumber);
      void ClearJigsawRejected();
      void IncrementNumberOfRejectedMeasuresInImage(const QString &serialNumber);
      void DecrementNumberOfRejectedMeasuresInImage(const QString &serialNumber);
      int GetNumMeasures() const;
      int GetNumPoints() const;
      int GetNumValidMeasures();
      int GetNumValidPoints();
      QString GetTarget() const;
      QString GetUserName() const;
      QString GetLastModified() const;
      QList< ControlPoint * > GetPoints();
      QList< QString > GetPointIds() const;
      SurfacePoint::CoordinateType GetCoordType();

      void SetCreatedDate(const QString &date);
      void SetDescription(const QString &newDescription);
      void SetImages(const QString &imageListFile);
      void SetImages(SerialNumberList &list, Progress *progress = 0);

      void SetModifiedDate(const QString &date);
      void SetMutex(QMutex *mutex);
      void SetNetworkId(const QString &id);
      void SetTarget(const QString &target);
      void SetTarget(Pvl label);
      void SetTarget(const ControlNet &other);
      void SetUserName(const QString &name);
      void SetCoordType(SurfacePoint::CoordinateType coordType);

      void swap(ControlNet &other);
      ControlNet &operator=(const ControlNet &other);

      const ControlPoint *operator[](QString id) const;
      ControlPoint *operator[](QString id);
      const ControlPoint *operator[](int id) const;
      ControlPoint *operator[](int id);


    signals:
      void networkStructureModified();
      void networkModified(ControlNet::ModType type);
      void pointModified(ControlPoint *point, ControlPoint::ModType type, QVariant oldValue, QVariant newValue);
      void measureModified(ControlMeasure *measure, ControlMeasure::ModType type, QVariant oldValue, QVariant newValue);
      void pointDeleted(ControlPoint *point);
      void newPoint(ControlPoint *);
      void newMeasure(ControlMeasure *);
      void measureRemoved(ControlMeasure *);



    private:
      void nullify();
      bool ValidateSerialNumber(QString serialNumber) const;
      void measureAdded(ControlMeasure *measure);
      void measureDeleted(ControlMeasure *measure);
      void measureIgnored(ControlMeasure *measure);
      void measureUnIgnored(ControlMeasure *measure);
      void pointIgnored(ControlPoint *point);
      void pointUnIgnored(ControlPoint *point);
      void UpdatePointReference(ControlPoint *point, QString oldId);
      void emitNetworkStructureModified();
      void emitMeasureModified(ControlMeasure *measure, ControlMeasure::ModType type, QVariant oldValue, QVariant newValue);
      void emitPointModified(ControlPoint *point, ControlPoint::ModType type, QVariant oldValue, QVariant newValue);
      void pointAdded(ControlPoint *point);
      bool addEdge(QString sourceSerial, QString targetSerial);
      bool removeEdge(QString sourceSerial, QString targetSerial);

    private: // graphing functions
      /**
       * @author 2012-04-13 Orrin Thomas
       *
       * @internal
       */
      class ControlMeasureLessThanFunctor :
          public std::binary_function<ControlMeasure* const &,
          ControlMeasure * const &, bool > {
        public:
          ControlMeasureLessThanFunctor(double(ControlMeasure::*accessorMethod)() const) {
            m_accessor = accessorMethod;
          }
          ControlMeasureLessThanFunctor(ControlMeasureLessThanFunctor const &other) {
            this->m_accessor = other.m_accessor;
          }
          ~ControlMeasureLessThanFunctor() {}

          bool operator()(ControlMeasure* const &, ControlMeasure* const &);
          ControlMeasureLessThanFunctor & operator=(ControlMeasureLessThanFunctor const &other);

        private:
          double(ControlMeasure::*m_accessor)() const;
      };

    private: // data
      //! hash ControlPoints by ControlPoint Id
      QHash< QString, ControlPoint * > * points;

      //! Used to define the verticies of the graph
      struct Image {
        QString serial; //! The serial number associated with the image
        //! The measures on the image, hashed by pointers to their parent ControlPoints
        QHash< ControlPoint *, ControlMeasure * > measures;
      };

      //! Used to define the edges of the graph.
      struct Connection {
        Connection() : strength(0) {}
        int strength;
      };

      //! Defines the graph type as an undirected graph that uses Images for verticies,
      //! and Connections for edges. It is defined as an adjacency list with the edge list
      //! represented by a set, the and vertex list represented by a list.
      typedef boost::adjacency_list<boost::setS,
                                    boost::listS,
                                    boost::undirectedS,
                                    Image,
                                    Connection> Network;

      typedef Network::vertex_descriptor ImageVertex; //! Reprents the verticies of the graph
      typedef Network::edge_descriptor ImageConnection; //! Represents the edges of the graph

      //! A map between an ImageVertex and its index
      typedef std::map<ImageVertex, size_t> VertexIndexMap;

      //! Converts VertexIndexMap into the appropriate form to be used by boost
      typedef boost::associative_property_map<VertexIndexMap> VertexIndexMapAdaptor;

      //! Iterates over adjacent verticies
      typedef boost::graph_traits<Network>::adjacency_iterator AdjacencyIterator;
      typedef boost::graph_traits<Network>::vertex_iterator VertexIterator;

      QHash<QString, ImageVertex> m_vertexMap; //! The serial number -> vertex hash used by the graph
      Network m_controlGraph; //! The ControlNet graph
      QStringList *pointIds;
      QMutex *m_mutex;

      QString p_targetName;            //!< Name of the target
      QString p_networkId;             //!< The Network Id
      QString p_created;               //!< Creation Date
      QString p_modified;              //!< Date Last Modified
      QString p_description;           //!< Textual Description of network
      QString p_userName;              //!< The user who created the network
      QMap<QString, Isis::Camera *> p_cameraMap; //!< A map from serialnumber to camera
      QMap<QString, int> p_cameraValidMeasuresMap; //!< A map from serialnumber to #measures
      QMap<QString, int> p_cameraRejectedMeasuresMap; //!< A map from serialnumber to
      //!  #rejected measures
      QVector<Isis::Camera *> p_cameraList; //!< Vector of image number to camera

      bool m_ownPoints; //!< Specifies ownership of point list. True if owned by this object.
      SurfacePoint::CoordinateType m_coordType; //!< The coordinate type of the control points
  };

  //! Typedef for QSharedPointer to control network.
  //! This typedef is for future implementation of target body
  typedef QSharedPointer<ControlNet> ControlNetQsp;
}

//! This allows ControlNet *'s to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::ControlNet *);

#endif
