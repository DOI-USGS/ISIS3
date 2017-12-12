#ifndef ControlNet_h
#define ControlNet_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
 *
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

// This is needed for the QVariant macro
#include <QMetaType>
#include <QObject> // parent class
#include <QSharedPointer>

#include "ControlNetFile.h"

#include <QString>
#include <QMap>
#include <QVector>

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
  class ControlCubeGraphNode;
  class Distance;
  class Progress;
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
   *   @history 2017-12-12 Kristin Berry - Updated to use QMap and QVector rather than std::map
   *                            and std::vector. 
   */
  class ControlNet : public QObject {
      Q_OBJECT

      friend class ControlMeasure;
      friend class ControlPoint;

    public:
      ControlNet();
      ControlNet(const ControlNet &other);
      ControlNet(const QString &filename, Progress *progress = 0);

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
      QList< ControlCubeGraphNode * > GetCubeGraphNodes();
      QList< QList< QString > > GetSerialConnections() const;
      QList< QList< ControlCubeGraphNode * > > GetNodeConnections() const;
      QSet< ControlMeasure * > MinimumSpanningTree(
          QList< ControlCubeGraphNode *> &island,
          bool lessThan(const ControlMeasure *, const ControlMeasure *)) const;
      int getEdgeCount() const;
      QString CubeGraphToString() const;
      QList< ControlMeasure * > GetMeasuresInCube(QString serialNumber);
      QList< ControlMeasure * > sortedMeasureList(double(ControlMeasure::*statFunc)() const,
                                                  double min,double max);
      void DeleteMeasuresWithId(QString serialNumber);

      void ComputeResiduals();
      void ComputeApriori();

      const ControlPoint *GetPoint(QString pointId) const;
      ControlPoint *GetPoint(QString pointId);
      const ControlPoint *GetPoint(int index) const;
      ControlPoint *GetPoint(int index);
      
      const ControlCubeGraphNode *getGraphNode(QString serialNumber) const;
      ControlCubeGraphNode *getGraphNode(QString serialNumber);

      double AverageResidual();
      Isis::Camera *Camera(int index);
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
      QList< ControlPoint * > GetPoints();
      QList< QString > GetPointIds() const;
      std::vector<Distance> GetTargetRadii();

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
      void SetTarget(const QString &target,
                     const QVector<Distance> &radii);
      void SetUserName(const QString &name);

      void swap(ControlNet &other);
      ControlNet &operator=(const ControlNet &other);

      const ControlPoint *operator[](QString id) const;
      ControlPoint *operator[](QString id);
      const ControlPoint *operator[](int id) const;
      ControlPoint *operator[](int id);


    signals:
      void networkStructureModified();

    private:
      void nullify();
      void ValidateSerialNumber(QString serialNumber) const;
      void measureAdded(ControlMeasure *measure);
      void measureDeleted(ControlMeasure *measure);
      void measureIgnored(ControlMeasure *measure);
      void measureUnIgnored(ControlMeasure *measure);
      void UpdatePointReference(ControlPoint *point, QString oldId);
      void emitNetworkStructureModified();


    private: // graphing functions
      QList< ControlCubeGraphNode * > RandomBFS(QList <
          ControlCubeGraphNode * > list) const;
      void Shuffle(QList< ControlCubeGraphNode * > & list) const;
      QPair< int, int > CalcBWAndCE(QList< QString > serials) const;

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


      /**
       * Encapsulation of a vertex in a minimum spanning tree.  Can be either a
       * Control Point or a Graph Node.  Each vertex is connected to another by
       * a measure.  A vertex without a parent vertex is considered a root node,
       * or the base of its own tree.
       *
       * @author ????-??-?? Unknown
       *
       * @internal 
       */
      class ControlVertex {
        public:
          //! Construct a vertex from a Graph Node
          ControlVertex(ControlCubeGraphNode *node) {
            m_node = node;
            m_point = NULL;
            m_parent = NULL;
          }

          //! Construct a vertex from a Control Point
          ControlVertex(ControlPoint *point) {
            m_point = point;
            m_node = NULL;
            m_parent = NULL;
          }

          //! Does not own any of its private data
          ~ControlVertex() {}

          //! Set the parent vertex, removing the root node status.
          void setParent(ControlVertex *v) { m_parent = v; }

          //! Get the root node, or greatest ancestor
          ControlVertex * getRoot() {
            ControlVertex *current = this;
            while (current->getParent() != NULL)
              current = current->getParent();
            return current;
          }

          //! Get the parent node.  A root node has no parent.
          ControlVertex * getParent() { return m_parent; }

          //! Get the node representation of this vertex
          ControlCubeGraphNode * getNode() { return m_node; }

          //! Get the point representation of this vertex
          ControlPoint * getPoint() { return m_point; }

          //! Join two nodes by setting one root to be the other's parent
          static void join(ControlVertex *v1, ControlVertex *v2) {
            v1->getRoot()->setParent(v2->getRoot());
          }

        private:
          //! The possibly non-existant graph node
          ControlCubeGraphNode *m_node;

          //! The possibly non-existant control point
          ControlPoint *m_point;

          //! The possibly non-existant parent vertex
          ControlVertex *m_parent;
      };


    private: // data
      //! hash ControlPoints by ControlPoint Id
      QHash< QString, ControlPoint * > * points;

      //! hash ControlCubeGraphNodes by CubeSerialNumber
      QHash< QString, ControlCubeGraphNode * > * cubeGraphNodes;
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
      QVector<Distance> p_targetRadii;        //!< Radii of target body

      bool m_ownPoints; //!< Specifies ownership of point list. True if owned by this object. 
  };

  //! This typedef is for future implementation of target body
  typedef QSharedPointer<ControlNet> ControlNetQsp;
}

//! This allows ControlNet *'s to be stored in a QVariant.
Q_DECLARE_METATYPE(Isis::ControlNet *);

#endif
