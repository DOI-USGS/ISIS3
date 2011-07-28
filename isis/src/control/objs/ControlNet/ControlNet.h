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

#include <QObject> // parent class

#include <map>

#include "ControlNetFile.h"

#include "iString.h"

template< typename A, typename B > class QHash;
template< typename T > class QList;
template< typename A, typename B > class QPair;
class QString;


namespace Isis {
  class Camera;
  class ControlMeasure;
  class ControlPoint;
  class ControlCubeGraphNode;
  class Distance;
  class iString;
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
   *                SerialNumberList class
   *   @history 2008-04-04 Christopher Austin Added Exists function
   *   @history 2008-04-18 Debbie A. Cook Added Progress reports to loading and
   *                SetImages and calculates the total number of measurements in
   *                the control net
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2009-01-06 Jeannie Walldren Fixed typo in SetImages() exception
   *                output. Added documentation.
   *   @history 2009-02-05 Christopher Austin when the created date or the
   *                modified date are not set, they default to the time in which
   *                Write() is called.
   *   @history 2009-04-07 Tracie Sucharski Added NumValidMeasures and
   *                NumIgnoredMeasures methods.
   *   @history 2009-06-03 Christopher Austin Added p_invalid functionality
   *                along with forceBuild, as well as other small fixes
   *                including documentation.
   *   @history 2009-07-13 Stacy Alley The std::vector of ControlPoints called
   *                'p_points' was replaced with a QVector of QString
   *                'p_pointIds' in conjunction with a QHash of
   *                <QString, ControlPoint> called 'p_pointsHash'. This was
   *                done to speed up the Add method which was essentially
   *                slowing down the reading or creation of Control Networks.
   *   @history 2010-05-06 Tracie Sucharski Use defaults of 0. instead of
   *                Isis::Null, because 0. is the default in the protocol
   *                buffers.
   *   @history 2010-08-06 Tracie Sucharski Updated for changes made after
   *                additional working sessions for Control network design.
   *   @history 2009-09-01 Eric Hyer Added two includes: QVector and QString
   *   @history 2010-09-09 Sharmila Prasad Added API to sort Control Net by
   *                Point ID Changed PointID's vector to StringList
   *   @history 2009-09-25 Travis Addair Changed methods which return the number
   *                of control measures in the network to compute those values
   *                at the time the method is called, not when the control
   *                network is first initialized.
   *   @history 2010-10-05 Tracie Sucharski Renamed the Write method to
   *                WritePvl.  Create new method, Write which takes another
   *                parameter indicating whether to write pvl format or binary
   *                format, The default will write binary.
   *   @history 2010-10-05 Eric Hyer ControlMeasure and ControlPoint now
   *                return QStrings for some methods.  Fixed breakages caused by
   *                this.
   *   @history 2010-10-06 Sharmila Prasad Added method to get CreatedDate
   *   @history 2010-11-21 Tracie Sucharski - Added new keyword, jigsawRejected
   *                to the read and write methods.
   *   @history 2011-01-13 Mackenzie Boyd Added copy constructor and assignment
   *                operator.
   *   @history 2011-01-17 Eric Hyer - Points are now owned and deleted by the
   *                network.  Network now stored in such a way that access to
   *                all points in a cube is just as cheap as accessing measures
   *                in a point.  Removed redundant methods and made other api
   *                changes.
   *   @history 2011-02-18 Eric Hyer - Made improvements and bug fixes related
   *                to interaction to other control network classes including
   *                ControlPoint and ControlMesure, but most significantly to
   *                ControlCubeGraphNode.  Most important fix was network
   *                notification of measures added to or removed from points
   *                after the point is added to the network.
   *   @history 2011-02-23 Eric Hyer - Added some methods to support graphing
   *                calculations (RandomBFS, Shuffle, CalcBWAndCE, and
   *                GetNodeConnections).
   *   @history 2011-03-08 Ken Edmundson - Added methods GetNumberOfMeasuresInImage,
   *                GetNumberOfJigsawRejectedMeasuresInImage,
   *                IncrementNumberOfRejectedMeasuresInImage,
   *                DecrementNumberOfRejectedMeasuresInImage, and members
   *                p_cameraMeasuresMap and p_cameraRejectedMeasuresMap.
   *   @history 2011-03-12 Debbie A. Cook - Added member p_targetRadii and
   *                method GetTargetRadii to support SurfacePoint sigma
   *                conversions in ControlPoint.
   *   @history 2011-03-14 Eric Hyer - Cube connection graph now updated when
   *                points or measures are ignored.
   *   @history 2011-03-15 Eric Hyer - Some cube graph bugs were fixed.
   *   @history 2011-03-25 Christopher Austin - Added UpdatePointReference() to
   *                work with ControlPoint's SetId()
   *   @history 2011-03-29 Steven Lambright - Made versioning viable for first
   *                release.
   *   @history 2011-04-27 Steven Lambright - UpdatePointReference had a bug
   *                where the pointIds list was not being correctly updated.
   *   @history 2011-05-04 Eric Hyer - References in graph data structure now
   *                kept for ignored points and measures
   *   @history 2011-05-25 Eric Hyer - Added getPoints method
   *   @history 2011-06-22 James Alexander Crough and Eric Hyer - Added
   *                getGraphNode method.
   *   @history 2011-06-28 Eric Hyer - Added getEdgeCount() method
   *   @history 2011-07-08 Travis Addair - Locked points can no longer be
   *                deleted
   *    @history 2011-07-27 Sharmila Prasad - Fixed bug in GetNumEditLockMeasures()
   */
  class ControlNet : public QObject {
      Q_OBJECT

      friend class ControlMeasure;
      friend class ControlPoint;

    public:
      ControlNet();
      ControlNet(const ControlNet &other);
      ControlNet(const iString &filename, Progress *progress = 0);

      ~ControlNet();

      void ReadControl(const iString &filename, Progress *progress = 0);
      void Write(const iString &filename, bool pvl = false);

      void AddPoint(ControlPoint *point);
      int DeletePoint(ControlPoint *point);
      int DeletePoint(iString pointId);
      int DeletePoint(int index);
      bool ContainsPoint(iString pointId) const;

      QList< QString > GetCubeSerials() const;
      QList< ControlCubeGraphNode * > GetCubeGraphNodes();
      QList< QList< QString > > GetSerialConnections() const;
      QList< QList< ControlCubeGraphNode * > > GetNodeConnections() const;
      int getEdgeCount() const;
      iString CubeGraphToString() const;
      QList< ControlMeasure * > GetMeasuresInCube(iString serialNumber);
      void DeleteMeasuresWithId(iString serialNumber);

      void ComputeResiduals();
      void ComputeApriori();

      const ControlPoint *GetPoint(QString pointId) const;
      ControlPoint *GetPoint(QString pointId);
      const ControlPoint *GetPoint(int index) const;
      ControlPoint *GetPoint(int index);
      
      const ControlCubeGraphNode *getGraphNode(QString serialNumber) const;

      double AverageResidual();
      Isis::Camera *Camera(int index);
      iString CreatedDate() const;
      iString Description() const;
      ControlPoint *FindClosest(iString serialNumber,
          double sample, double line);
      bool IsValid() const;
      double GetMaximumResidual();
      iString GetNetworkId() const;
      int GetNumEditLockMeasures();
      int GetNumEditLockPoints();
      int GetNumIgnoredMeasures();
      int GetNumberOfMeasuresInImage(const std::string &serialNumber);
      int GetNumberOfJigsawRejectedMeasuresInImage(const std::string &serialNumber);
      void IncrementNumberOfRejectedMeasuresInImage(const std::string &serialNumber);
      void DecrementNumberOfRejectedMeasuresInImage(const std::string &serialNumber);
      int GetNumMeasures() const;
      int GetNumPoints() const;
      int GetNumValidMeasures();
      int GetNumValidPoints();
      iString GetTarget() const;
      iString GetUserName() const;
      QList< ControlPoint * > getPoints() const;
      QList< QString > GetPointIds() const;
      std::vector<Distance> GetTargetRadii();

      void SetCreatedDate(const iString &date);
      void SetDescription(const iString &newDescription);
      void SetImages(const iString &imageListFile);
      void SetImages(SerialNumberList &list, Progress *progress = 0);
      void SetModifiedDate(const iString &date);
      void SetNetworkId(const iString &id);
      void SetTarget(const iString &target);
      void SetUserName(const iString &name);

      const ControlNet &operator=(ControlNet other);

      const ControlPoint *operator[](QString id) const;
      ControlPoint *operator[](QString id);
      const ControlPoint *operator[](int id) const;
      ControlPoint *operator[](int id);


    signals:
      void networkStructureModified();

    private:
      void nullify();
      void ValidateSerialNumber(iString serialNumber) const;
      void measureAdded(ControlMeasure *measure);
      void measureDeleted(ControlMeasure *measure);
      void measureIgnored(ControlMeasure *measure);
      void measureUnIgnored(ControlMeasure *measure);
      void UpdatePointReference(ControlPoint *point, iString oldId);
      void emitNetworkStructureModified();


    private: // graphing functions
      QList< ControlCubeGraphNode * > RandomBFS(QList <
          ControlCubeGraphNode * > list) const;
      void Shuffle(QList< ControlCubeGraphNode * > & list) const;
      QPair< int, int > CalcBWAndCE(QList< QString > serials) const;


    private: // data
      //! hash ControlPoints by ControlPoint Id
      QHash< QString, ControlPoint * > * points;

      //! hash ControlCubeGraphNodes by CubeSerialNumber
      QHash< QString, ControlCubeGraphNode * > * cubeGraphNodes;
      QStringList *pointIds;

      iString p_targetName;            //!< Name of the target
      iString p_networkId;             //!< The Network Id
      iString p_created;               //!< Creation Date
      iString p_modified;              //!< Date Last Modified
      iString p_description;           //!< Textual Description of network
      iString p_userName;              //!< The user who created the network
      std::map<iString, Isis::Camera *> p_cameraMap; //!< A map from serialnumber to camera
      std::map<std::string, int> p_cameraMeasuresMap; //!< A map from serialnumber to #measures
      std::map<std::string, int> p_cameraRejectedMeasuresMap; //!< A map from serialnumber to
      //!  #rejected measures
      std::vector<Isis::Camera *> p_cameraList; //!< Vector of image number to camera
      std::vector<Distance> p_targetRadii;        //!< Radii of target body

      bool p_invalid;  //!< If the Control Network is currently invalid
  };
}

#endif
