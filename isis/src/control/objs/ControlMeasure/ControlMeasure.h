#ifndef ControlMeasure_h
#define ControlMeasure_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/09/01 17:53:05 $
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

#include <QObject>

template< class A> class QVector;
template< class A> class QList;
class QString;
class QStringList;
class QVariant;

namespace Isis {
  class Application;
  class Camera;
  class ControlMeasureLogData;
  class ControlPoint;
  class ControlCubeGraphNode;
  class PvlGroup;
  class PvlKeyword;

  /**
   * @brief a control measurement
   *
   * This class is used to record a coordinate (measurement) on a cube
   * for a control point.
   *
   * @ingroup ControlNetwork
   *
   * @author 2005-07-29 Jeff Anderson
   *
   * @see ControlPoint ControlNet
   *
   * @internal
   *   @history 2005-07-29 Jeff Anderson Original version
   *   @history 2006-01-11 Jacob Danton Added a Reference flag and updated
   *                              unitTest
   *   @history 2006-10-05 Brendan George Modified call to retrieve current time
   *                              to use iTime class, instead of Application
   *                              class
   *   @history 2008-06-23 Steven Lambright The ZScore keyword is now supported
   *   @history 2008-06-25 Steven Koechle Added get methods for ZScore values.
   *   @history 2009-09-01 Eric Hyer Added the methods GetMeasureData and
   *                              GetMeasureDataNames.  Also fixed include
   *                              issues.
   *   @history 2009-09-22 Eric Hyer Removed forward declaration for QPair
   *   @history 2009-10-30 Eric Hyer GetMeasurDataNames is now static
   *   @history 2009-12-31 Tracie Sucharski Added new parameters for jigsaw.
   *   @history 2010-04-29 Tracie Sucharski Renamed AutomaticPixel to
   *                              Registered Pixel and AutomaticSubPixel to
   *                              RegisteredSubPixel.
   *   @history 2010-05-06 Tracie Sucharski Use defaults of 0. instead of
   *                              Isis::Null, because 0. is the default in the
   *                              protocol buffers.
   *   @history 2010-06-03 Tracie Sucharski Move SetReference to ControlPoint,
   *                              so error checking to make sure only a single
   *                              measure be set to Reference.
   *   @history 2010-06-17 Tracie Sucharski Added Lock value to MeasureStatus.
   *   @history 2010-07-21 Tracie Sucharski Remove SetReference IsReference
   *                              methods.  Reference is no longer a separate
   *                              keyword, but a possible MeasureType.  Use
   *                              either the ControlMeasure::Type() method or
   *                              ControlPoint::HasReference, ReferenceIndex
   *                              to determine the reference measure.
   *   @history 2010-07-27 Tracie Sucharski Updated for changes made after
   *                              additional working sessions for Control
   *                              network design.
   *   @history 2010-08-26 Tracie Sucharski No longer writing
   *                              ResidualMagnitude, will be calculated as
   *                              needed.
   *   @history 2010-10-04 Sharmila Prasad Add PrintableMeasureType method to
   *                              return String Measure Type
   *   @history 2010-10-05 Eric Hyer No more includes in header file!
   *                              serial number is now a QString.
   *   @history 2010-10-18 Tracie Sucharski Change "Setters", ComputeApriori
   *                              and ComputeResiduals to return either Success
   *                              or PointLocked.  If the point is locked do not
   *                              set values.
   *   @history 2010-10-19 Tracie Sucharski Set the DateTime to Null if
   *                              anything in measure chanes, the date will be
   *                              updated at write time.
   *   @history 2010-10-22 Steven Lambright Moved all implementations to the
   *                              cpp file to shorten the header. Reordered
   *                              methods. Improved consistency as far as the
   *                              types of parameters passed in - some took
   *                              'double' and some 'const double &' depending
   *                              on the original programmer. Now all take
   *                              'double' for the cleaner syntax and hopefully
   *                              the overhead is negligable.
   *   @history 2010-10-26 Steven Lambright Change default chooser name from
   *                              user name to application name.
   *   @history 2010-11-03 Mackenzie Boyd Modified MeasureType to string
   *                              method, added static version. Added
   *                              method PrintableClassData.
   *   @history 2010-11-16 Debbie Cook, Added jigsawRejected keyword.
   *   @history 2010-11-29 Tracie Sucharski, Constructor still had initializations
   *                              in addition to the same intiializers in the
   *                              method InitializeToNull.  Also, values were
   *                              being intialized to 0. instead of Isis::Null.
   *                              Not sure how this happened other than a mistake
   *                              in svn mergina?  This was causing keywords
   *                              with a value of 0. to be written to the output
   *                              network.  Remove all methods relating to
   *                              Ephemeris Time-they are not used anywhere in
   *                              Isis.
   *   @history 2010-12-08 Tracie Sucharski, Added a new measure type of Ground.
   *                              This was done to help qnet functionality, but
   *                              may be used for other functionality in the
   *                              future.  Added IsGround for convenience.
   *   @history 2010-12-22 Steven Lambright Added LogData capabilities and
   *                              prepped for detailed change logs. The
   *                              accessors needed to change names because of a
   *                              conflict with a new enumerated value,
   *                              DataField. The accessors to this class no
   *                              longer give up internal pointers.
   *   @history 2011-01-13 Mackenzie Boyd Added pointer to owning ControlPoint.
   *   @history 2011-02-10 Eric Hyer - Measures no longer know about or care
   *                about whether they are the reference measure or not.  This
   *                is now completely maintained by the ControlPoint class.
   *   @history 2011-02-18 Eric Hyer - ControlCubeGraphNode is now a friend.
   *                Eliminated ConnectControlSN and DisconnectControlSN methods.
   *                Fixed bug in destructor.
   *   @history 2011-03-01 Eric Hyer - Added StringToMeasureType method
   *   @history 2011-03-04 Tracie Sucharski - Fixed bug in DeleteLogData method,
   *                               subtract 1 from size() in the loop.
   *   @history 2011-03-14 Eric Hyer - ControlMeasures now notify their network
   *                when their ignored status changes.
   *   @history 2011-04-04 Steven Lambright - Removed an old constructor
   *   @history 2011-04-07 Steven Lambright - GetResidualMagnitude no longer
   *                              does math on special pixels.
   *   @history 2011-04-11 Steven Lambright - Added GetLogValue for convenience
   *   @history 2011-07-05 Debbie A. Cook - Removed editLock checks from methods
   *                              SetCamera, SetRejected, and SetResidual and
   *                              changed all other editLock tests to use
   *                              IsEditLocked method instead of the private
   *                              member, p_editLock, directly.  Also added
   *                              a check for an implicit lock if the measure is
   *                              the reference measure of the parent point in
   *                              the IsEditLocked method.
   *   @history 2011-07-29 Jai Rideout, Steven Lambright, and Eric Hyer - Made
   *                           this inherit from QObject to get destroyed()
   *                           signal
   *   @history 2012-07-26 Tracie Sucharski, Fixed bug in == operator, comparison of chooserName
   *                           and dateTime, comparison was between the pointers instead of the
   *                           data and added comparisons for missing member data.
   *   @history 2012-08-11 Tracie Sucharski, Add computed and measured ephemeris time set to Null
   *                           in InitializeToNull.
   *   @history 2017-12-19 Adam Goins - Added "HasX()" accessors to ControlMeasure.
   *   @history 2017-12-20 Jesse Mapel - Implemented GetLogDataEntries method for use in
   *                           ControlNetVersioner refactor.
   *   @history 2017-12-21 Adam Goins - Removed protobuf references.
   */
  class ControlMeasure : public QObject {

      Q_OBJECT

      friend class ControlPoint;
      friend class ControlCubeGraphNode;
    public:
      /**
       * @brief Control network measurement types
       *
       * OLD VERSION:
       * Manual implies the coordinate was selected by a human
       * but still may be in error.  It is subject to refinement by other
       * computer programs.
       *
       * Estimated implies the coordinate was selected by a computer program
       * but has not been sub-pixel registered and is more than likely in
       * error.  It is subject to refinement by other computer programs
       *
       * AutomaticPixel implies the coordinate was selected by a computer
       * program and met registration criteria (but still may be in error). It
       * is subject to refinement by other computer programs.
       *
       * ValidatedManual implies the coordinate was manually selected by a
       * human, was validated by a human, and should not be changed by
       * any automated means.
       *
       * ValidatedAutomatic implies the coordinate was automatically selected
       * by a computer program, was validated by a human, and should not
       * be changed by any automated means.
       *
       */
      enum MeasureType {
        //! (e.g., autoseed, interest) AKA predicted, unmeasured, unverified
        Candidate,
        //! Hand Measured (e.g., qnet)
        Manual,
        //! Registered to whole pixel (e.g.,pointreg)
        RegisteredPixel,
        //! Registered to sub-pixel (e.g., pointreg)
        RegisteredSubPixel,
      };

      enum Status {
        Success,
        MeasureLocked
      };

      enum DataField {
        AprioriLine        = 1,
        AprioriSample      = 2,
        ChooserName        = 4,
        CubeSerialNumber   = 8,
        Coordinate         = 16,
        DateTime           = 32,
        Diameter           = 64,
        EditLock           = 128,
        Rejected           = 256,
        FocalPlaneMeasured = 512,
        FocalPlaneComputed = 1024,
        Ignore             = 2048,
        SampleResidual     = 4096,
        LineResidual       = 8192,
        SampleSigma        = 16384,
        LineSigma          = 32768,
        Type               = 65536
      };

      ControlMeasure();
      ControlMeasure(const ControlMeasure &other);
      ~ControlMeasure();

      ControlPoint *Parent() { return parentPoint; }
      ControlCubeGraphNode *ControlSN() { return associatedCSN; }

      Status SetAprioriLine(double aprioriLine);
      Status SetAprioriSample(double aprioriSample);
      Status SetCamera(Isis::Camera *camera);
      Status SetChooserName();
      Status SetChooserName(QString name);
      Status SetCoordinate(double sample, double line);
      Status SetCoordinate(double sample, double line, MeasureType type);
      Status SetCubeSerialNumber(QString newSerialNumber);
      Status SetDateTime();
      Status SetDateTime(QString datetime);
      Status SetDiameter(double diameter);
      Status SetEditLock(bool editLock);
      Status SetFocalPlaneMeasured(double x, double y);
      Status SetFocalPlaneComputed(double x, double y);
      Status SetIgnored(bool newIgnoredStatus);
      Status SetLineSigma(double lineSigma);
      Status SetRejected(bool rejected);
      Status SetResidual(double sampResidual, double lineResidual);
      Status SetSampleSigma(double sampleSigma);
      Status SetType(MeasureType type);

      void DeleteLogData(long dataType);
      QVariant GetLogValue(long dataType) const;
      bool HasLogData(long dataType) const;
      void SetLogData(ControlMeasureLogData);
      void UpdateLogData(ControlMeasureLogData);

      double GetAprioriLine() const;
      double GetAprioriSample() const;
      Isis::Camera *Camera() const;
      QString GetChooserName() const;
      QString GetCubeSerialNumber() const;
      QString GetDateTime() const;
      double GetDiameter() const;
      ControlMeasureLogData GetLogData(long dataType) const;
      bool IsEditLocked() const;
      bool IsRejected() const;
      double GetFocalPlaneComputedX() const;
      double GetFocalPlaneComputedY() const;
      double GetFocalPlaneMeasuredX() const;
      double GetFocalPlaneMeasuredY() const;
      double GetMeasureData(QString) const;
      bool IsIgnored() const;
      bool IsMeasured() const;
      bool IsRegistered() const;
      bool IsStatisticallyRelevant(DataField field) const;
      double GetLine() const;
      double GetLineResidual() const;
      double GetLineSigma() const;
      QVector<ControlMeasureLogData> GetLogDataEntries() const;
      QVector<ControlMeasureLogData> GetLastRunLogDataEntries() const;
      double GetResidualMagnitude() const;
      double GetSample() const;
      double GetSampleResidual() const;
      double GetSampleSigma() const;
      MeasureType GetType() const;
      QString GetPointId() const;

      double GetSampleShift() const;
      double GetLineShift() const;
      double GetPixelShift() const;

      static QVector<QString> GetMeasureDataNames();

      QList<QStringList> PrintableClassData() const;

      static QString MeasureTypeToString(MeasureType type);
      static MeasureType StringToMeasureType(QString str);
      QString GetMeasureTypeString() const;

      const ControlMeasure &operator=(const ControlMeasure &other);
      bool operator != (const Isis::ControlMeasure &pMeasure) const;
      bool operator == (const Isis::ControlMeasure &pMeasure) const;

    private: // methods
      void InitializeToNull();
      void MeasureModified();

    private: // data
      ControlPoint *parentPoint;  //!< Pointer to parent ControlPoint, may be null
      ControlCubeGraphNode *associatedCSN;  //!< Pointer to the Serial Number
      // structure connecting measures in an image

      QString *p_serialNumber;
      MeasureType p_measureType;

      QVector<ControlMeasureLogData> * p_loggedData;
      PvlGroup *p_comments;

      /**
       * list the program used and the definition file or include the user
       * name for qnet
       */
      QString *p_chooserName;
      QString *p_dateTime;
      bool p_editLock;        //!< If true do not edit anything in measure.
      bool p_ignore;
      bool p_jigsawRejected;  //!< Status of measure for last bundle adjust iteration
      double p_sample;        //!< Current sample/line measurement
      double p_line;          //!< Jigsaw uses this measure
      double p_diameter;

      double p_aprioriSample;   //!< The first identified location of the
      double p_aprioriLine;     //!< measure by autoseed.  Pointreg/Interest
      //!< always use this location to start it's search.
      //!< Could be moved by interest program or user.


      double p_computedEphemerisTime;

      double p_sampleSigma;    //!< Uncertainty/sigma in pixels of the measurement (current sample/line)
      double p_lineSigma;      //!< Not sure how we determine this for automated or manual picking
      double p_sampleResidual; //!< Jigsaw information - Solution error - replaces p_sampleError
      double p_lineResidual;   //!< Jigsaw information - Solution error - replaces p_lineError
      Isis::Camera *p_camera;
      double p_focalPlaneMeasuredX;
      double p_focalPlaneMeasuredY;
      double p_focalPlaneComputedX;
      double p_focalPlaneComputedY;
      double p_measuredEphemerisTime;
  };
}

#endif
