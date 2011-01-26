/**
 * @file
 * $Revision: 1.14 $
 * $Date: 2009/09/08 17:38:17 $
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

#ifndef ControlPoint_h
#define ControlPoint_h

#include "iString.h"
#include "ControlMeasure.h"
#include "SurfacePoint.h"

template< typename A, typename B > class QHash;

class QStringList;

namespace Isis {
  class ControlNet;
  class Latitude;
  class Longitude;
  class PBControlNet_PBControlPoint;
  class PBControlNetLogData_Point;
  class PvlObject;

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
   *            unitTest
   *   @history 2006-06-28 Tracie Sucharski, Added method to return measure
   *            for given serial number.
   *   @history 2006-10-31 Tracie Sucharski, Added HasReference method,
   *            changed ReferenceIndex method to throw error if there is no
   *            Reference ControlMeasure.
   *   @history 2007-01-25 Debbie A. Cook, Removed return statement in
   *            SetApriori method for \Point case so that
   *            FocalPlaneMeasures will get set.  The method already has a later
   *            return statement to avoid changing the lat/lon values.
   *   @history 2007-10-19 Debbie A. Cook, Wrapped longitudes when calculating
   *            apriori longitude for points with a difference of more than 180
   *            degrees of longitude between measures.
   *   @history 2008-01-14 Debbie A. Cook, Changed call to
   *            Camera->SetUniversalGround in ComputeErrors to include the
   *            radius as an argument since the method has been overloaded to
   *            include radius.
   *   @history 2008-09-12 Tracie Sucharski, Add method to return true/false
   *            for existence of Serial Number.
   *   @history 2009-03-07 Debbie A. Cook Fixed ComputeErrors method to set
   *            focal plane coordinates
   *            without changing time and improved error messages.
   *   @history 2009-06-03 Christopher Austin, Added the p_invalid functionality
   *            along with forceBuild, fixed documentation errors.
   *   @history 2009-06-22 Jeff Anderson, Modified ComputeAprior
   *            method to correctly handle ground and held points.
   *            Previosuly it would throw an error if the lat/lon
   *            of a measure could not be computed.  Also, modify
   *            the ComputeErrors method to not abort any longer
   *            if a control point lat/lon could not be projected
   *            back to a image line/sample.
   *   @history 2009-08-13 Debbie A. Cook Corrected calculation of
   *            scale used to get the undistorted focal plane
   *            coordinates to use the signed focal length (Z) from
   *            the CameraDistortionMap,
   *   @history 2009-08-21 Christopher Austin, Put the default return of
   *            ReferenceIndex() back as the first Measured measure.
   *   @history 2009-09-01 Eric Hyer, fixed some include issues.
   *   @history 2009-09-08 Eric Hyer, Added PointTypeToString method.
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error messages.
   *   @history 2009-12-06  Tracie Sucharski, Renamed ComputeErrors to
   *            ComputeResiudals.
   *   @history 2010-03-19 Debbie A. Cook Replaced code in method ComputeErrors
   *            with call to CameraGroundMap->GetXY
   *   @history 2010-01-12 Tracie Sucharski, Added support for binary networds,
   *            added new parameters, renamed ComputeErrors to ComputeResiduals,
   *            renamed MaximumError to MaximumResidual, renamed AverageError to
   *            AverageResidual.
   *   @history 2010-03-19 Debbie A. Cook Replaced code in method ComputeErrors
   *            with call to CameraGroundMap->GetXY
   *   @history 2010-05-06 Tracie Sucharski, Use defaults of 0. instead of
   *            Isis::Null, because 0. is the default in the protocol buffers.
   *   @history 2010-05-11 Sharmila Prasad Added API's Copy Constructor to copy
   *            one point to another and ReferenceIndexNoException not to throw
   *            Exception if there are no reference point or no measures in a
   *            Control Point.  Also added the boolean logical operator method
   *            = and !=.
   *   @history 2010-05-26 Tracie Sucharski, Changed point type of Ground to
   *            GroundXYZ, GroundXY, or GroundZ.
   *   @history 2010-06-01 Tracie Sucharski, Added Ellipsoid and DEM to
   *            AprioriSource enum.  Change AprioriSourceBasemap to
   *            AprioriLatLonSourcFile and AprioriRadiusSourceFile.
   *   @history 2010-06-03 Tracie Sucharski, Moved SetReference method from
   *            ControlMeasure so check for multiple reference meausres can be
   *            done.
   *   @history 2010-06-17 Tracie Sucharski, Added Lock keyword, new methods
   *            SetLocked, Locked and NumLockedMeasures.
   *   @history 2010-06-04 Eric Hyer - removed parametor for PointTypeToString()
   *            additional working sessions for Control network design.
   *   @history 2010-07-27 Tracie Sucharski, Updated for changes made after
   *            additional working sessions for Control network
   *            design.  Major change to keywords, including storing coordinates
   *            as x/y/z instead of lat/lon/radius.  Needed methods to allow
   *            inputting either, and conversion methods.
   *   @history 2010-08-18 Tracie Sucharski, Updated for changes made to
   *            SurfacePoint covariance matrix.
   *   @history 2010-08-25 Tracie Sucharski, Fixed some bugs relating to
   *            conversions between sigmas and covariance matrices.
   *   @history 2010-09-13 Tracie Sucharski, Added methods for setting
   *            planetocentric sigmas as both degrees and meters,
   *            as a result of changes made to the SurfacePoint class.
   *   @history 2010-09-15 Tracie Sucharski, It was decided after mtg with
   *            Debbie, Stuart, Ken and Tracie that ControlPoint
   *            will only function with x/y/z, not lat/lon/radius. It will be
   *            the responsibility of the application or class using
   *            ControlPoint to set up a SurfacePoint object to do conversions
   *            between x/y/z and lat/lon/radius. So... remove all conversion
   *            methods from this class. It was also decided that when importing
   *            old networks that contain Sigmas, the sigmas will not be
   *            imported, due to conflicts with the units of the sigmas.
   *   @history 2010-05-11 Sharmila Prasad Added API's Copy Constructor to copy
   *            one point to another and ReferenceIndexNoException not to throw
   *            Exception if there are no reference point or no measures in a
   *            Control Point.
   *   @history 2010-06-04 Eric Hyer - removed parametor for PointTypeToString()
   *   @history 2010-09-09 Sharmila Prasad - Added API's to get Latitude,
   *            Longitude, Radius from the Reference Measure in the Point. Also
   *            to get the min & max Line & Sample Errors
   *   @history 2010-09-27 Tracie Sucharski, Removed these new methods and move
   *            functionality to the ControlNetFilter class. Add the old
   *            methods, SetUniversalGround, UniversalLatitude,
   *            UniversalLongitude and Radius back in for convenience.
   *   @history 2010-10-05 Eric Hyer - interface to ID is now with QString
   *   @history 2010-10-06 Sharmila Prasad - Added method ReferenceLocked()
   *   @history 2010-10-18 Tracie Sucharski, Change "Setters", ComputeApriori
   *            and ComputeResiduals to return either Success or PointLocked.
   *            If the point is locked do not set values.
   *   @history 2010-10-18 Tracie Sucharski, Change SurfacePoint::Rectangular
   *            and SurfacePoint::Ocentric to SurfacePoint as
   *            both return values and parameters.
   *   @history 2010-10-06 Sharmila Prasad - API to reset Apriori
   *   @history 2010-10-21 Steven Lambright Minimized the header file and moved
   *            most implementations to the cpp. Reorganized the order of
   *            methods. Made more methods callable on a const instance. Added
   *            GetMeasure(...) methods and a suggested implementation for the
   *            bracket operators, not yet implemented due to returning
   *            reference conflicts. Finished implementing the new use of
   *            SurfacePoint mentioned in the last history comment. Marked
   *            SetUniversalGround(), UniversalLatitude(), UniversalLongitude(),
   *            and Radius() as deprecated. These methods need to be phased out
   *            and GetSurfacePoint() used instead. There are naming
   *            conflicts with accessors (some use Get, some don't) still. The
   *            comparison operator now relies on QVector's comparison operator
   *            instead of looping itself. Added private helper methods:
   *              int FindMeasureIndex(iString serialNumber) const
   *              void PointModified();
   *            Updated documentation extensively. Removed everything apost
   *            except for inside of Load() and CreatePvlObject(). Added
   *            automatic updating of DateTime and ChooserName.
   *   @history 2010-10-26 Steven Lambright Change default chooser name from
   *            user name to application name.
   *   @history 2010-11-03 Mackenzie Boyd Added ToString methods for enums,
   *            String and statis ToString now exist for PointType,
   *            RadiusSource, and SurfacePointSource.
   *   @history 2010-11-16 Debbie Cook, Added jigsawRejected keyword.
   *   @history 2010-12-08 Tracie Sucharski, Added IsGround convenience method.
   *   @history 2010-12-28 Steven Lambright Changed accessors to match
   *            ControlMeasure's method of accessing data. Removed obsolete
   *            methods to prevent further use of them.
   *   @history 2011-01-13 Mackenzie Boyd Added pointer to owning ControlNet.
   *   @history 2011-01-17 Eric Hyer - Points now own and delete their measures.
   *                           ControlNet now notified of changes (like adding
   *                           and removing measures).  Returning pointers to
   *                           measures is now safe and encouraged.
   */
  class ControlPoint {
      friend class ControlNet;
    public:
      /**
       * These are the valid 'types' of point. A point type defines what a point
       *   is tying together.
       */
      enum PointType {
        /**
          * A Ground point is a Control Point whose lat/lon is well established
          * and should not be changed. Some people will refer to this as a
          * truth (i.e., ground truth).  Holding a point is equivalent to making
          * it a ground point.  A ground point can be identifed in one or more
          * cubes.
          */
        Ground,
        /**
          * A Tie point is a Control Point that identifies common measurements
          * between two or more cubes. While it could have a lat/lon, it is not
          * necessarily correct and is subject to change.  This is the most
          * common type of control point.
          */
        Tie
      };

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
      ControlPoint(const iString &id);
      ControlPoint(const PBControlNet_PBControlPoint &);
      ControlPoint(const PBControlNet_PBControlPoint &,
                   const PBControlNetLogData_Point &);
      ~ControlPoint();

      ControlNet *Parent() { return parentNetwork; }

      void Load(PvlObject &p);

      void Add(ControlMeasure *measure);
      void Delete(iString serialNumber);
      void Delete(int index);
      Status ResetApriori();

      const ControlMeasure *GetMeasure(iString serialNumber) const;
      ControlMeasure *GetMeasure(iString serialNumber);

      const ControlMeasure * GetMeasure(int index) const;
      ControlMeasure * GetMeasure(int index);

      const ControlMeasure *GetReferenceMeasure() const;
      ControlMeasure *GetReferenceMeasure();

      Status SetChooserName(iString name);
      Status SetDateTime(iString dateTime);
      Status SetEditLock(bool editLock);
      Status SetId(iString id);
      Status SetRejected(bool rejected);
      Status SetIgnored(bool ignore);
      Status SetSurfacePoint(SurfacePoint surfacePoint);
      Status SetType(PointType type);

      Status SetAprioriRadiusSource(RadiusSource::Source source);
      Status SetAprioriRadiusSourceFile(iString sourceFile);
      Status SetAprioriSurfacePoint(SurfacePoint aprioriSurfacePoint);
      Status SetAprioriSurfacePointSource(SurfacePointSource::Source source);
      Status SetAprioriSurfacePointSourceFile(iString sourceFile);

      Status ComputeApriori();
      Status ComputeResiduals();

      iString GetChooserName() const;
      iString GetDateTime() const;
      bool IsEditLocked() const;
      bool IsRejected() const;
      iString GetId() const;
      bool IsIgnored() const;
      bool IsValid() const;
      bool IsInvalid() const;
      SurfacePoint GetSurfacePoint() const;
      PointType GetType() const;
      bool IsGround() const;

      static iString PointTypeToString(PointType type);
      iString GetPointTypeString() const;
      static iString RadiusSourceToString(RadiusSource::Source source);
      iString GetRadiusSourceString() const;
      static iString SurfacePointSourceToString(SurfacePointSource::Source source);
      iString GetSurfacePointSourceString() const;

      SurfacePoint GetAprioriSurfacePoint() const;
      RadiusSource::Source GetAprioriRadiusSource() const;
      iString GetAprioriRadiusSourceFile() const;
      SurfacePointSource::Source GetAprioriSurfacePointSource() const;
      iString GetAprioriSurfacePointSourceFile() const;

      int GetNumMeasures() const;
      int GetNumValidMeasures() const;
      int GetNumLockedMeasures() const;
      bool HasSerialNumber(iString serialNumber) const;
      bool HasReference() const;
      int  GetReferenceIndex() const;
      int  GetReferenceIndexNoException() const;
      QString GetReferenceKey() const;
      QString GetReferenceKeyNoException() const;
      bool IsReferenceLocked() const;

      double GetAverageResidual() const;
      double GetMinimumResidual() const;
      double GetMinimumSampleResidual() const;
      double GetMinimumLineResidual() const;
      double GetMaximumResidual() const;
      double GetMaximumSampleResidual() const;
      double GetMaximumLineResidual() const;

      QList< QString > GetCubeSerialNumbers() const;

      PvlObject ToPvlObject() const;

      const ControlMeasure *operator[](iString serialNumber) const;
      ControlMeasure *operator[](iString serialNumber);

      const ControlMeasure *operator[](int index) const;
      ControlMeasure *operator[](int index);

      bool operator!=(const ControlPoint &pPoint) const;
      bool operator==(const ControlPoint &pPoint) const;
      const ControlPoint &operator=(ControlPoint pPoint);

      // The next 3 methods are specifically to support BundleAdjust
      void ZeroNumberOfRejectedMeasures();
      void SetNumberOfRejectedMeasures(int numRejected);
      int GetNumberOfRejectedMeasures() const;

      PBControlNet_PBControlPoint ToProtocolBuffer() const;
      PBControlNetLogData_Point GetLogProtocolBuffer() const;


    private:
      void validateMeasure(iString serialNumber, bool checkRef = false) const;

      void AddMeasure(ControlMeasure *measure);

      void Init(const PBControlNet_PBControlPoint &);

      void PointModified();

      ControlNet *parentNetwork;

      //!< List of Control Measures
      QHash< QString, ControlMeasure * > * p_measures;

      QStringList *cubeSerials;

      ControlMeasure *referenceMeasure;

      /**
       * This is the control point ID. This is supposed to be a unique
       *   identifier for control points. This often has a number in it, and
       *   looks like "T0052" where the next one is "T0053" and so on.
       */
      iString p_id;

      /**
       * This is the user name of the person who last modified this control
       *   point. Modifications are things like updating the surface point, but
       *   not things like updating the last modified time. The calculations
       *   relating to this control point have to actually change for this to
       *   be updated. This is an empty string if we need to dynamically
       *   get the username of the caller when asked for (or written to file).
       */
      iString p_chooserName;

      /**
       * This is the last modified date and time. This is updated automatically
       *   and works virtually in the same way as p_chooserName.
       */
      iString p_dateTime;

      /**
       * What this control point is tying together.
       * @see PointType
       */
      PointType p_type;

      /**
       * If we forced a build that we would normally have thrown an exception
       *   for then this is set to true. Otherwise, and most of the time, this
       *   is false.
       */
      bool p_invalid;

      /**
       * This stores the edit lock state.
       * @see SetEditLock
       */
      bool p_editLock;

      /**
       * This stores the jigsaw rejected state.
       * @see SetJigsawReject
       */
      bool p_jigsawRejected;

      /**
       * True if we should preserve but ignore the entire control point and its
       *   measures.
       */
      bool p_ignore;

      //! Where the apriori surface point originated from
      SurfacePointSource::Source p_aprioriSurfacePointSource;

      //! Filename where the apriori surface point originated from
      iString p_aprioriSurfacePointSourceFile;

      /**
       * Where the apriori surface point's radius originated from, most commonly
       *   used by jigsaw.
       */
      RadiusSource::Source p_aprioriRadiusSource;

      /**
       * The name of the file that derives the apriori surface point's radius
       */
      iString p_aprioriRadiusSourceFile;

      /**
       * The apriori surface point. This is the "known truth" or trustworthy
       *   point that should not be modified unless done very explicitely. This
       *   comes from places like hand picking where you really don't want the
       *   surface point to vary far from this point, but some variation is
       *   okay (1/10th of a pixel is fair for human accuracy for example). Very
       *   often this point does not exist.
       */
      SurfacePoint p_aprioriSurfacePoint;

      /**
       * This is the calculated, or aposterori, surface point. This is what most
       *   programs should be working with and updating.
       */
      SurfacePoint p_surfacePoint;

      /**
       * This parameter is used and maintained by BundleAdjust for the jigsaw
       * application.  It is stored here because ControlPoint contains the index
       * of the measures.
       */
      int p_numberOfRejectedMeasures;
  };
}

#endif
