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


template< class A> class QVector;
template< class A> class QList;
class QStringList;

namespace Isis {
  class Application;
  class Camera;
  class iString;
  class PvlGroup;

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
   */
  class ControlMeasure {
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
        //! // Reference Measure 
        Reference,
        //! (e.g., autoseed, interest) AKA predicted, unmeasured, unverified
        Candidate,
        //! Hand Measured (e.g., qnet)
        Manual,
        //! Registered to whole pixel (e.g.,pointreg)
        RegisteredPixel, 
        //! Registered to sub-pixel (e.g., pointreg)
        RegisteredSubPixel,
        //! Coordinate in ground source
        Ground
      };

      enum Status {
        Success,
        MeasureLocked
      };

      /*enum MeasureData {
        AprioriLine,
        AprioriSample,
        ChooserName,
        ComputerEphemerisTime,
        CubeSerialNumber,
        DateTime,
        Diameter,
        EditLock,
        FocalPlaneComputedX,
        FocalPlaneComputedY,
        FocalPlaneMeasuredX,
        FocalPlaneMeasuredY,
        Ignore,
        IsMeasured,
        IsRegistered,
        Line,
        LineResidual,
        LineSigma,
        MeasuredEphemerisTime,
        ResidualMagnitude,
        Sample,
        SampleResidual,
        SampleSigma,
        Type,
        NumMeasureDataFields
      };*/

      ControlMeasure();
      ControlMeasure(const ControlMeasure & other);
      ~ControlMeasure();

      void Load(PvlGroup &p);

      Status SetAprioriLine  (double aprioriLine);
      Status SetAprioriSample(double aprioriSample);
      Status SetCamera (Isis::Camera *camera);
      Status SetCubeSerialNumber(iString newSerialNumber);
      Status SetChooserName();
      Status SetChooserName(iString name);
      Status SetComputedEphemerisTime(double et);
      Status SetCoordinate(double sample, double line);
      Status SetCoordinate(double sample, double line, MeasureType type);
      Status SetDateTime();
      Status SetDateTime(iString datetime);
      Status SetDiameter(double diameter);
      Status SetEditLock(bool editLock);
      Status SetRejected(bool rejected);
      Status SetFocalPlaneMeasured(double x, double y);
      Status SetFocalPlaneComputed(double x, double y);
      Status SetIgnore(bool ignore);
      Status SetLineSigma(double lineSigma);
      Status SetMeasuredEphemerisTime(double et);
      Status SetResidual(double sampResidual, double lineResidual);
      Status SetSampleSigma(double sampleSigma);
      Status SetType(MeasureType type);

      double AprioriLine() const;
      double AprioriSample() const;
      Isis::Camera *Camera() const;
      iString ChooserName() const;
      double ComputedEphemerisTime() const;
      iString CubeSerialNumber() const;
      iString DateTime() const;
      double Diameter() const;
      bool EditLock() const;
      bool IsRejected() const;
      double FocalPlaneComputedX() const;
      double FocalPlaneComputedY() const;
      double FocalPlaneMeasuredX() const;
      double FocalPlaneMeasuredY() const;
      bool Ignore() const;
      bool IsMeasured () const;
      bool IsRegistered () const;
      bool IsGround () const;
      //static bool IsStatisticallyRelevant(MeasureData) const;
      double Line() const;
      double LineResidual() const;
      double LineSigma() const;
      double MeasuredEphemerisTime() const;
      double ResidualMagnitude() const;
      double Sample() const;
      double SampleResidual() const;
      double SampleSigma() const;
      MeasureType Type () const;

      double GetMeasureData(iString data) const;
      static QVector<iString> GetMeasureDataNames();

      QList<QStringList> PrintableClassData() const;       

      PvlGroup CreatePvlGroup();
      static iString MeasureTypeToString(MeasureType type);
      iString MeasureTypeString() const;

      const ControlMeasure & operator=(const ControlMeasure & other);
      bool operator != (const Isis::ControlMeasure &pMeasure) const;
      bool operator == (const Isis::ControlMeasure &pMeasure) const;

    private: // methods
      void InitializeToNull();
      void MeasureModified();

    private: // data
      iString *p_serialNumber;
      MeasureType p_measureType;

      /**
       * list the program used and the definition file or include the user
       * name for qnet
       */
      iString *p_chooserName;
      iString *p_dateTime;
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
