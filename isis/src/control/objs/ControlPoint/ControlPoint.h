#ifndef ControlPoint_h
#define ControlPoint_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <bitset>

#include <QObject>
#include <QString>

#include "ControlMeasure.h"
#include "SurfacePoint.h"

template< typename A, typename B > class QHash;

class QStringList;

namespace Isis {
  class ControlNet;
  class ControlPointFileEntryV0002;
  class Latitude;
  class Longitude;
  class PBControlNet_PBControlPoint;
  class PBControlNetLogData_Point;
  class PvlObject;
  class Statistics;

  /**
   * @brief A single control point
   *
   * A control point is one or more measurements that identify the same feature
   * or location in different images.
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlMeasure ControlNet
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Added ReferenceIndex method and updated
   *                           unitTest
   *   @history 2006-06-28 Tracie Sucharski, Added method to return measure
   *                           for given serial number.
   *   @history 2006-10-31 Tracie Sucharski, Added HasReference method,
   *                           changed ReferenceIndex method to throw error if
   *                           there is no Reference ControlMeasure.
   *   @history 2007-01-25 Debbie A. Cook, Removed return statement in
   *                           SetApriori method for \Point case so that
   *                           FocalPlaneMeasures will get set.  The method
   *                           already has a later return statement to avoid
   *                           changing the lat/lon values.
   *   @history 2007-10-19 Debbie A. Cook, Wrapped longitudes when calculating
   *                           apriori longitude for points with a difference of
   *                           more than 180 degrees of longitude between
   *                           measures.
   *   @history 2008-01-14 Debbie A. Cook, Changed call to
   *                           Camera->SetUniversalGround in ComputeErrors to
   *                           include the radius as an argument since the
   *                           method has been overloaded to include radius.
   *   @history 2008-09-12 Tracie Sucharski, Add method to return true/false
   *                           for existence of Serial Number.
   *   @history 2009-03-07 Debbie A. Cook Fixed ComputeErrors method to set
   *                           focal plane coordinates without changing time and
   *                           improved error messages.
   *   @history 2009-06-03 Christopher Austin, Added the invalid functionality
   *                           along with forceBuild, fixed documentation
   *                           errors.
   *   @history 2009-06-22 Jeff Anderson, Modified ComputeAprior
   *                           method to correctly handle ground and held
   *                           points. Previosuly it would throw an error if the
   *                           lat/lon of a measure could not be computed. Also,
   *                           modify the ComputeErrors method to not abort any
   *                           longer if a control point lat/lon could not be
   *                           projected back to a image line/sample.
   *   @history 2009-08-13 Debbie A. Cook Corrected calculation of
   *                           scale used to get the undistorted focal plane
   *                           coordinates to use the signed focal length (Z)
   *                           from the CameraDistortionMap,
   *   @history 2009-08-21 Christopher Austin, Put the default return of
   *                           ReferenceIndex() back as the first Measured
   *                           measure.
   *   @history 2009-09-01 Eric Hyer, fixed some include issues.
   *   @history 2009-09-08 Eric Hyer, Added PointTypeToString method.
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *                           error messages.
   *   @history 2009-12-06 Tracie Sucharski, Renamed ComputeErrors to
   *                           ComputeResiudals.
   *   @history 2010-03-19 Debbie A. Cook Replaced code in method ComputeErrors
   *                           with call to CameraGroundMap->GetXY
   *   @history 2010-01-12 Tracie Sucharski, Added support for binary networds,
   *                           added new parameters, renamed ComputeErrors to
   *                           ComputeResiduals, renamed MaximumError to
   *                           MaximumResidual, renamed AverageError to
   *                           AverageResidual.
   *   @history 2010-03-19 Debbie A. Cook Replaced code in method ComputeErrors
   *                           with call to CameraGroundMap->GetXY
   *   @history 2010-05-06 Tracie Sucharski, Use defaults of 0. instead of
   *                           Isis::Null, because 0. is the default in the
   *                           protocol buffers.
   *   @history 2010-05-11 Sharmila Prasad Added API's Copy Constructor to copy
   *                           one point to another and
   *                           ReferenceIndexNoException not to throw Exception
   *                           if there are no reference point or no measures in
   *                           a Control Point.  Also added the boolean logical
   *                           operator method = and !=.
   *   @history 2010-05-26 Tracie Sucharski, Changed point type of Ground to
   *                           GroundXYZ, GroundXY, or GroundZ.
   *   @history 2010-06-01 Tracie Sucharski, Added Ellipsoid and DEM to
   *                           AprioriSource enum.  Change AprioriSourceBasemap
   *                           to AprioriLatLonSourcFile and
   *                           AprioriRadiusSourceFile.
   *   @history 2010-06-03 Tracie Sucharski, Moved SetReference method from
   *                           ControlMeasure so check for multiple reference
   *                           meausres can be done.
   *   @history 2010-06-17 Tracie Sucharski, Added Lock keyword, new methods
   *                           SetLocked, Locked and NumLockedMeasures.
   *   @history 2010-06-04 Eric Hyer - removed parametor for PointTypeToString()
   *                           additional working sessions for Control network
   *                           design.
   *   @history 2010-07-27 Tracie Sucharski, Updated for changes made after
   *                           additional working sessions for Control network
   *                           design.  Major change to keywords, including
   *                           storing coordinates as x/y/z instead of
   *                           lat/lon/radius.  Needed methods to allow
   *                           inputting either, and conversion methods.
   *   @history 2010-08-18 Tracie Sucharski, Updated for changes made to
   *                           SurfacePoint covariance matrix.
   *   @history 2010-08-25 Tracie Sucharski, Fixed some bugs relating to
   *                           conversions between sigmas and covariance
   *                           matrices.
   *   @history 2010-09-13 Tracie Sucharski, Added methods for setting
   *                           planetocentric sigmas as both degrees and meters,
   *                           as a result of changes made to the SurfacePoint
   *                           class.
   *   @history 2010-09-15 Tracie Sucharski, It was decided after mtg with
   *                           Debbie, Stuart, Ken and Tracie that ControlPoint
   *                           will only function with x/y/z, not
   *                           lat/lon/radius. It will be the responsibility of
   *                           the application or class using ControlPoint to
   *                           set up a SurfacePoint object to do conversions
   *                           between x/y/z and lat/lon/radius. So... remove
   *                           all conversion methods from this class. It was
   *                           also decided that when importing old networks
   *                           that contain Sigmas, the sigmas will not be
   *                           imported, due to conflicts with the units of the
   *                           sigmas.
   *   @history 2010-05-11 Sharmila Prasad Added API's Copy Constructor to copy
   *                           one point to another and
   *                           ReferenceIndexNoException not to throw Exception
   *                           if there are no reference point or no measures in
   *                           a Control Point.
   *   @history 2010-06-04 Eric Hyer - removed parametor for PointTypeToString()
   *   @history 2010-09-09 Sharmila Prasad - Added API's to get Latitude,
   *                           Longitude, Radius from the Reference Measure in
   *                           the Point. Also to get the min & max Line &
   *                           Sample Errors
   *   @history 2010-09-27 Tracie Sucharski, Removed these new methods and move
   *                           functionality to the ControlNetFilter class. Add
   *                           the old methods, SetUniversalGround,
   *                           UniversalLatitude, UniversalLongitude and Radius
   *                           back in for convenience.
   *   @history 2010-10-05 Eric Hyer - interface to ID is now with QString
   *   @history 2010-10-06 Sharmila Prasad - Added method ReferenceLocked()
   *   @history 2010-10-18 Tracie Sucharski, Change "Setters", ComputeApriori
   *                           and ComputeResiduals to return either Success or
   *                           PointLocked.  If the point is locked do not set
   *                           values.
   *   @history 2010-10-18 Tracie Sucharski, Change SurfacePoint::Rectangular
   *                           and SurfacePoint::Ocentric to SurfacePoint as
   *                           both return values and parameters.
   *   @history 2010-10-06 Sharmila Prasad - API to reset Apriori
   *   @history 2010-10-21 Steven Lambright Minimized the header file and moved
   *                           most implementations to the cpp. Reorganized the
   *                           order of methods. Made more methods callable on a
   *                           const instance. Added GetMeasure(...) methods and
   *                           a suggested implementation for the bracket
   *                           operators, not yet implemented due to returning
   *                           reference conflicts. Finished implementing the
   *                           new use of SurfacePoint mentioned in the last
   *                           history comment. Marked SetUniversalGround(),
   *                           UniversalLatitude(), UniversalLongitude(), and
   *                           Radius() as deprecated. These methods need to be
   *                           phased out and GetSurfacePoint() used instead.
   *                           There are naming conflicts with accessors (some
   *                           use Get, some don't) still. The comparison
   *                           operator now relies on QVector's comparison
   *                           operator instead of looping itself. Added private
   *                           helper methods:
   *                             int FindMeasureIndex(QString serialNumber)
   *                                 const
   *                             void PointModified();
   *                           Updated documentation extensively. Removed
   *                           everything apost except for inside of Load() and
   *                           CreatePvlObject(). Added automatic updating of
   *                           DateTime and ChooserName.
   *   @history 2010-10-26 Steven Lambright Change default chooser name from
   *                           user name to application name.
   *   @history 2010-11-03 Mackenzie Boyd Added ToString methods for enums,
   *                           String and statis ToString now exist for
   *                           PointType, RadiusSource, and SurfacePointSource.
   *   @history 2010-11-16 Debbie Cook, Added jigsawRejected keyword.
   *   @history 2010-12-08 Tracie Sucharski, Added IsGround convenience method.
   *   @history 2010-12-28 Steven Lambright Changed accessors to match
   *                           ControlMeasure's method of accessing data.
   *                           Removed obsolete methods to prevent further use
   *                           of them.
   *   @history 2011-01-13 Mackenzie Boyd Added pointer to owning ControlNet.
   *   @history 2011-01-17 Eric Hyer - Points now own and delete their measures.
   *                           ControlNet now notified of changes (like adding
   *                           and removing measures).  Returning pointers to
   *                           measures is now safe and encouraged.
   *   @history 2011-02-10 Eric Hyer - measures no longer know or care if they
   *                           are the reference measure.  This information is
   *                           now completely maintained by this class.  Made
   *                           numerous API and internal changes, eliminating
   *                           substantial duplicate code and increasing
   *                           interface clearity.  Hungarian notation now
   *                           eliminated from this class.
   *   @history 2011-02-11 Steven Lambright - Measure log data is now written to
   *                           the binary file properly.
   *   @history 2011-02-18 Eric Hyer - Added Delete(ControlMeasure *) method.
   *                           Fixed bugs related to network notification of
   *                           measure addition and deletion.
   *   @history 2011-02-28 Eric Hyer - Fixed bug in operator= that caused the
   *                           the reference measure to not get propagated
   *                           correctly
   *   @history 2011-02-28 Steven Lambright - Added a flag for cnetref to say
   *                           whether a reference measure has been explicitly
   *                           or implicitly set.
   *   @history 2011-03-01 Eric Hyer - Added StringToRadiusSource and
   *                           StringToSurfacePointSource methods
   *   @history 2011-03-08 Ken Edmundson and Debbie Cook - Added members
   *                           ConstraintStatus, LatitudeConstrained,
   *                           LongitudeConstrained, RadiusConstrained,
   *                           constraintStatus and methods
   *                           ConputeResiduals_Millimeters(),
   *                           HasAprioriCoordinates(), IsConstrained(),
   *                           IsLatitudeConstrained(),
   *                           IsLongitudeConstrained(),
   *                           and NumberOfConstrainedCoordinates().
   *   @history 2011-03-11 Debbie Cook - changed name of member surfacePoint to
   *                           adjustedSurfacePoint.  Also changed methods
   *                           SetSurfacePoint to SetAdjustedSurfacePoint and
   *                           GetSurfacePoint to GetAdjustedSurfacePoint.
   *   @history 2011-03-14 Eric Hyer - Added GetMeasures method.  Network now
   *                           notified when a point's ignored status changes
   *                           for updating its cube connection graph (cube
   *                           connections were not respecting ignored flags on
   *                           points / measures).
   *   @history 2011-03-14 Christopher Austin - Added GetBestSurfacePoint to
   *                           reduce external duplicate code.
   *   @history 2011-03-15 Steven Lambright - Now writes AdjustedX, AdjustedY,
   *                           and AdjustedZ to the pvl format. Also updated
   *                           proto buffer calls to reflect naming changes.
   *   @history 2011-03-17 Eric Hyer - Added default parameter to GetMeasures
   *                           method for excluding ignored measures.
   *   @history 2011-03-25 Christopher Austin - Added functionality to SetId()
   *   @history 2011-04-01 Debbie A. Cook and Ken Edmundson - Added argument to
   *                           constructor for target radii
   *   @history 2011-04-04 Steven Lambright - Removed an old constructor
   *                           and made the Distance vector one take a const
   *                           reference.
   *   @history 2011-04-04 Steven Lambright - Updated constructor from binary to
   *                           take planetary radii
   *   @history 2011-05-02 Debbie A. Cook - Added new point type Constrained
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point
   *                           types:  Ground ------> Fixed
   *                                   Tie----------> Free
   *   @history 2011-06-30 Eric Hyer - Added StringToPointType() method
   *   @history 2011-07-01 Debbie A. Cook - Removed editLock checks from
   *                    SetAdjustedSurfacePoint and ComputeResiduals
   *   @history 2011-07-08 Travis Addair - Locked measures can no longer be
   *                           deleted
   *   @history 2011-07-12 Ken Edmundson - Modified ComputeApriori method
   *                           to handle radius properly when computing average
   *                           coordinates
   *   @history 2011-07-29 Jai Rideout, Steven Lambright, and Eric Hyer - Made
   *                           this inherit from QObject to get destroyed()
   *                           signal
   *   @history 2011-07-29 Eric Hyer - Changed some graph code in Delete()
   *   @history 2011-09-13 Eric Hyer,Tracie Sucharski - operator= changes:
   *                           Change input parameter to const &.
   *                           Re-wrote to use Delete and AddMeasure methods, so
   *                           that the ControlGraphNode is updated correctly.
   *   @history 2011-09-30 Steven Lambright and Tracie Sucharski - Fixed
   *                           bugs that caused unpredictable behavior in the
   *                           assignment operator.
   *   @history 2011-10-01 Steven Lambright - Simplified the copy constructor
   *                           and fixed problems (which caused ASSERT fails) in
   *                           the copy constructor.
   *   @history 2011-10-06 Steven Lambright - Radii provided in the protocol
   *                           buffer constructor can now be invalid.
   *   @history 2011-10-07 Steven Lambright and Stuart Sides - Fixed bug in the
   *                           constructor given a protocol buffer. This caused
   *                           unpredictable reference measure behaviour
   *                           sometimes (rarely).
   *   @history 2011-10-14 Ken Edmundson Added method ClearJigsawRejected(); to
   *                           set all measure and point JigsawRejected flags to
   *                           false prior to bundle adjustment.
   *   @history 2012-03-31 Debbie A. Cook Programmer note:  Revised
   *                           ComputeResiduals to call
   *                           ComputeResidualsMillimeters and avoid duplication
   *                           of code.  Also revised
   *                           ComputeResidualsMillimeters to make the radar
   *                           case handled the same as other instruments.
   *   @history 2013-11-12 Ken Edmundson Programmer note:  Revised
   *                           ComputeApriori such that initial coordinates are
   *                           computed for "Free" points that have constrained
   *                           coordinates. References #1653.
   *   @history 2013-11-13 Kimberly Oyama - Added missing member variables to == operator and
   *                           made sure the comparisons are being done correctly. Fixes #1014.
   *   @history 2015-11-05 Kris Becker - invalid flag was not properly
   *                           initialized in ControlPointFileEntryV0002
   *                           constructor (Merged by Kristin Berry. Fixes #2392)
   *   @history 2017-05-25 Debbie A. Cook - coordType to SetPrioriSurfacePoint with a default of
   *                            Latitudinal.  Changed LatitudeConstrained to Coord1Constrained, etc.
   *                            References #4649 and #501.
   *   @history 2017-12-18 Kristin Berry - Added convenience methods:
   *                            HasAprioriSurfacePointSourceFile(), HasAprioriRadiusSourceFile(),
   *                            HasRefMeasure().
   *   @history 2017-12-21 Adam Goins - Removed redundant code following ControlNetVersioner
   *                           refactor.
   *   @history 2018-01-05 Adam Goins - Added HasDateTime() and HasChooserName() methods to allow
   *                           to allow the value of these variables to be read without being
   *                           overriden if they're empty. (Getters override if they're empty).
   *   @history 2018-06-06 Jesse Mapel - Modified setIgnored to use new pointIgnored and
   *                           pointUnIgnored methods. References #5434.
   *   @history 2018-06-15 Adam Goins & Jesse Mapel - Added the ModType enum, as well as a series
   *                           of calls to parentNetwork()->emitPointModified() whenever a change
   *                           is made to a Control Point or any of it's measures. This is done
   *                           to allow for communication between the ControlNetVitals class
   *                           and changes made to the Control Network that it is observing.
   *                           Fixes #5435.
   *  @history 2018-06-29 Adam Goins - Modified to operator= method to use setters when copying
   *                           one Control Point to another so that the proper signals get called.
   *                           Fixes #5435.
   *   @history 2018-06-30 Debbie A. Cook Removed all calls to obsolete method
   *                           SurfacePoint::SetRadii.  References #5457.
   *  @history 2019-03-10 Ken Edmundson - See history entry for ComputeApriori method (References
   *                           #2591). Added check to IsConstrained() method to see if point type is
   *                           Free, in which case we ignore stored a priori sigmas on the
   *                           coordinates.
   *   @history 2019-04-28 Ken Edmundson Moved PointModified signal and Measures member variable to
   *                           protected and made destructor virtual for subclass LidarControlPoint.
   *  @history 2019-05-16 Debbie A. Cook  See history entry for ComputeResiduals.  Modified call to
   *                           CameraGroundMap to not do back-of-planet test. References #2591.
   */
  class ControlPoint : public QObject {

      Q_OBJECT

      friend class ControlNet;
    public:
      /**
       * These are the valid 'types' of point. A point type defines what a point
       *   is tying together.
       */
      enum PointType {
        /**
          * A Fixed point is a Control Point whose lat/lon is well established
          * and should not be changed. Some people will refer to this as a
          * truth (i.e., ground truth).  A fixed point can be identifed in one or
          * more cubes.  Historically this point was called a "Ground" point.
          */
        Fixed = 0,
        /**
          * A Constrained point is a Control Point whose lat/lon/radius is somewhat
          * established and should not be changed.
          */
        Constrained = 1,
        /**
          * A Free point is a Control Point that identifies common measurements
          * between two or more cubes. While it could have a lat/lon, it is not
          * necessarily correct and is subject to change.  This is the most
          * common type of control point.  This point type floats freely in
          * a bundle adjustment.  Historically this point type was called "Tie".
          */
        Free = 2
      };
      static const int PointTypeCount = 3;

      /**
       * This is a return status for many of the mutating (setter) method calls.
       *   We chose to use return status' because often times ignoring them
       *   is the behavior the caller wants.
       */
      enum Status {
        /**
         * This is returned when an operation cannot be performed due to a
         *   problem such as the point is ignored and the operation doesn't make
         *   sense.
         */
        Failure,
        /**
         * This is returned when the operation successfully took effect.
         */
        Success,
        /**
         * This is returned when the operation requires Edit Lock to be false
         *   but it is currently true. The operation did not take effect.
         */
        PointLocked
      };

      /**
       * This is a convenience member for checking number of constrained
       * coordinates in the SurfacePoint
       */
      enum ConstraintStatus {
        Coord1Constrained = 0,
        Coord2Constrained = 1,
        Coord3Constrained = 2
      };

      /**
       *  @brief Control Point Modification Types
       *
       *  This enum is designed to represent the different types of modifications that can be
       *  made to a ControlPoint.
       *
       *  EditLockModified means that the Control Point had it's edit lock flag changed.
       *  IgnoredModified means that the Control Measure had it's ignored flag changed.
       *  TypeModified means that the ControlPoint::PointType for this control point was modified.
       */
      enum ModType {
        EditLockModified,
        IgnoredModified,
        TypeModified
      };

      // This stuff input to jigsaw
      // How did apriori source get computed??
      struct SurfacePointSource {
        enum Source {
          None,
          User,
          AverageOfMeasures,
          Reference,
          Basemap,
          BundleSolution
        };
      };

      struct RadiusSource {
        enum Source {
          None,
          User,
          AverageOfMeasures,
          Ellipsoid,
          DEM,
          BundleSolution
        };
      };

      ControlPoint();
      ControlPoint(const ControlPoint &);
      ControlPoint(const QString &id);
      virtual ~ControlPoint();

      ControlNet *Parent() { return parentNetwork; }

      void Load(PvlObject &p);

      void Add(ControlMeasure *measure);
      int Delete(ControlMeasure *measure);
      int Delete(QString serialNumber);
      int Delete(int index);
      Status ResetApriori();

      const ControlMeasure *GetMeasure(QString serialNumber) const;
      ControlMeasure *GetMeasure(QString serialNumber);

      const ControlMeasure *GetMeasure(int index) const;
      ControlMeasure *GetMeasure(int index);

      bool HasRefMeasure() const;
      const ControlMeasure *GetRefMeasure() const;
      ControlMeasure *GetRefMeasure();

      Status SetChooserName(QString name);
      Status SetDateTime(QString newDateTime);
      Status SetEditLock(bool editLock);
      Status SetId(QString id);
      Status SetRefMeasure(ControlMeasure *cm);
      Status SetRefMeasure(int index);
      Status SetRefMeasure(QString sn);
      Status SetRejected(bool rejected);
      Status SetIgnored(bool newIgnoreStatus);
      Status SetAdjustedSurfacePoint(SurfacePoint newSurfacePoint);
      Status SetType(PointType newType);

      Status SetAprioriRadiusSource(RadiusSource::Source source);
      Status SetAprioriRadiusSourceFile(QString sourceFile);
      Status SetAprioriSurfacePoint(SurfacePoint aprioriSP);
      Status SetAprioriSurfacePointSource(SurfacePointSource::Source source);
      Status SetAprioriSurfacePointSourceFile(QString sourceFile);

//    Status UpdateSphericalPointCoordinates(const Latitude &lat, const Longitude &lon,
//                                      const Distance &radius);

      Status ComputeApriori();
      Status ComputeResiduals();
      Status ComputeResiduals_Millimeters();

      SurfacePoint GetAdjustedSurfacePoint() const;

      SurfacePoint GetBestSurfacePoint() const;
      QString GetChooserName() const;
      QString GetDateTime() const;
      bool IsEditLocked() const;
      bool IsRejected() const;
      QString GetId() const;
      bool IsIgnored() const;
      bool IsValid() const;
      // Can we get rid of this? It doesn't appear to be used anywhere.  *** ToDo ***
      bool IsInvalid() const;
      bool IsFree() const;
      bool IsFixed() const;
      bool HasAprioriCoordinates();

      bool IsConstrained();
      bool IsCoord1Constrained();
      bool IsCoord2Constrained();
      bool IsCoord3Constrained();
      int NumberOfConstrainedCoordinates();

      static QString PointTypeToString(PointType type);
      static PointType StringToPointType(QString pointTypeString);

      QString GetPointTypeString() const;
      PointType GetType() const;

      static QString RadiusSourceToString(RadiusSource::Source source);
      static RadiusSource::Source StringToRadiusSource(QString str);
      QString GetRadiusSourceString() const;
      static QString SurfacePointSourceToString(SurfacePointSource::Source source);
      static SurfacePointSource::Source StringToSurfacePointSource(QString str);
      QString GetSurfacePointSourceString() const;
      SurfacePoint GetAprioriSurfacePoint() const;

      RadiusSource::Source GetAprioriRadiusSource() const;
      bool HasAprioriRadiusSourceFile() const;
      QString GetAprioriRadiusSourceFile() const;
      SurfacePointSource::Source GetAprioriSurfacePointSource() const;
      bool HasAprioriSurfacePointSourceFile() const;
      QString GetAprioriSurfacePointSourceFile() const;

      int GetNumMeasures() const;
      int GetNumValidMeasures() const;
      int GetNumLockedMeasures() const;
      bool HasSerialNumber(QString serialNumber) const;
      bool HasChooserName() const;
      bool HasDateTime() const;
      int IndexOf(ControlMeasure *, bool throws = true) const;
      int IndexOf(QString sn, bool throws = true) const;
      int IndexOfRefMeasure() const;
      bool IsReferenceExplicit() const;
      QString GetReferenceSN() const;
      void emitMeasureModified(ControlMeasure *measure, ControlMeasure::ModType modType, QVariant oldValue, QVariant newValue);



      Statistics GetStatistic(double(ControlMeasure::*statFunc)() const) const;
      Statistics GetStatistic(long dataType) const;

      QList< ControlMeasure * > getMeasures(bool excludeIgnored = false) const;
      QList< QString > getCubeSerialNumbers() const;

      const ControlMeasure *operator[](QString serialNumber) const;
      ControlMeasure *operator[](QString serialNumber);

      const ControlMeasure *operator[](int index) const;
      ControlMeasure *operator[](int index);

      bool operator!=(const ControlPoint &pPoint) const;
      bool operator==(const ControlPoint &pPoint) const;
      const ControlPoint &operator=(const ControlPoint &pPoint);

      // The next 7 methods are specifically to support BundleAdjust
      void ZeroNumberOfRejectedMeasures();
      void SetNumberOfRejectedMeasures(int numRejected);
      int GetNumberOfRejectedMeasures() const;
      double GetSampleResidualRms() const;
      double GetLineResidualRms() const;
      double GetResidualRms() const;
      void ClearJigsawRejected();

    protected:
      void PointModified();
      //!< List of Control Measures
      QHash< QString, ControlMeasure * > * measures;

    private:
      void SetExplicitReference(ControlMeasure *measure);
      void ValidateMeasure(QString serialNumber) const;
      void AddMeasure(ControlMeasure *measure);


      ControlNet *parentNetwork;

      QStringList *cubeSerials;

      ControlMeasure *referenceMeasure;

      /**
       * This is the control point ID. This is supposed to be a unique
       *   identifier for control points. This often has a number in it, and
       *   looks like "T0052" where the next one is "T0053" and so on.
       */
      QString id;

      /**
       * This is the user name of the person who last modified this control
       *   point. Modifications are things like updating the surface point, but
       *   not things like updating the last modified time. The calculations
       *   relating to this control point have to actually change for this to
       *   be updated. This is an empty string if we need to dynamically
       *   get the username of the caller when asked for (or written to file).
       */
      QString chooserName;

      /**
       * This is the last modified date and time. This is updated automatically
       *   and works virtually in the same way as chooserName.
       */
      QString dateTime;

      /**
       * What this control point is tying together.
       * @see PointType
       */
      PointType type;

      /**
       * If we forced a build that we would normally have thrown an exception
       *   for then this is set to true. Otherwise, and most of the time, this
       *   is false.
       */
      bool invalid;

      /**
       * This stores the edit lock state.
       * @see SetEditLock
       */
      bool editLock;

      /**
       * This stores the jigsaw rejected state.
       * @see SetJigsawReject
       */
      bool jigsawRejected;

      /**
       * This stores the constraint status of the a priori SurfacePoint
       *   @todo Eventually add x, y, and z.  Instead we made generic coordinates
       */
      std::bitset<3> constraintStatus;

      /**
       * This indicates if a program has explicitely set the reference in this
       *   point or the implicit reference is still the current reference. This
       *   is useful for programs that want to choose the reference for all
       *   points where this hasn't happened yet.
       */
      bool referenceExplicitlySet;

      /**
       * True if we should preserve but ignore the entire control point and its
       *   measures.
       */
      bool ignore;

      //! Where the apriori surface point originated from
      SurfacePointSource::Source aprioriSurfacePointSource;

      //! FileName where the apriori surface point originated from
      QString aprioriSurfacePointSourceFile;

      /**
       * Where the apriori surface point's radius originated from, most commonly
       *   used by jigsaw.
       */
      RadiusSource::Source aprioriRadiusSource;

      /**
       * The name of the file that derives the apriori surface point's radius
       */
      QString aprioriRadiusSourceFile;

      /**
       * The apriori surface point. This is the "known truth" or trustworthy
       *   point that should not be modified unless done very explicitely. This
       *   comes from places like hand picking where you really don't want the
       *   surface point to vary far from this point, but some variation is
       *   okay (1/10th of a pixel is fair for human accuracy for example). Very
       *   often this point does not exist.
       */
      SurfacePoint aprioriSurfacePoint;

      /**
       * This is the calculated, or aposterori, surface point. This is what most
       *   programs should be working with and updating.
       */
      SurfacePoint adjustedSurfacePoint;

      /**
       * This parameter is used and maintained by BundleAdjust for the jigsaw
       * application.  It is stored here because ControlPoint contains the index
       * of the measures.
       */
      int numberOfRejectedMeasures;
  };
}

#endif
