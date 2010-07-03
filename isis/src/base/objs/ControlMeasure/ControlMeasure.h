#ifndef ControlMeasure_h
#define ControlMeasure_h
/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/06/10 23:56:44 $
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

#include <string>

template< class A> class QVector;
class QString;

namespace Isis {

  class PvlGroup;
  class Camera;

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
   *   @history 2005-07-29 Jeff Anderson - Original version
   *   @history 2006-01-11 Jacob Danton Added a Reference flag and updated unitTest
   *   @history 2006-10-05 Brendan George Modified call to retrieve current time to
   *                                    use iTime class, instead of Application class
   *   @history 2008-06-23 Steven Lambright - The ZScore keyword is now supported
   *   @history 2008-06-25 Steven Koechle - Added get methods for ZScore values.
   *   @history 2009-09-01 Eric Hyer - Added the methods GetMeasureData and
   *                                   GetMeasureDataNames.  Also fixed include
   *                                   issues.
   *   @history 2009-09-22 Eric Hyer - Removed forward declaration for QPair
   *   @history 2009-10-30 Eric Hyer - GetMeasurDataNames is now static
   */
  class ControlMeasure {
    public:
      /**
       * @brief Control network measurement types
       *
       * Unmeasured implies the coordinate (sample, line) has not been
       * identified and therefore should not be used.  An error will be
       * thrown if the programmer attempts to acquire a coordinate for
       * an unmeasured measurement
       *
       * Manual implies the coordinate was selected by a human
       * but still may be in error.  It is subject to refinement by other
       * computer programs.
       *
       * Estimated implies the coordinate was selected by a computer program
       * but has not been sub-pixel registered and is more than likely in
       * error.  It is subject to refinement by other computer programs
       *
       * Automatic implies the coordinate was selected by a computer program
       * and met registration criteria (but still may be in error).  It is
       * subject to refinement by other computer programs
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
      enum MeasureType { Unmeasured, Manual, Estimated, Automatic,
                         ValidatedManual, ValidatedAutomatic
                       };

      // Constructor
      ControlMeasure();

      //! Destroy a control point measurement
      ~ControlMeasure() {};

      void Load(PvlGroup &p);
      PvlGroup CreatePvlGroup();

      /**
       * @brief Set the coordinate of the measurement
       *
       * @param sample  Sample coordinate of the measurement
       * @param line    Line coordinate of the measurement
       */
      void SetCoordinate(const double &sample, const double &line) {
        p_sample = sample;
        p_line = line;
      };

      /**
       * @brief Set the coordinate of the measurement
       *
       * @param sample  Sample coordinate of the measurement
       * @param line    Line coordinate of the measurement
       * @param type    The type of the coordinate
       */
      void SetCoordinate(const double &sample, const double &line,
                         const MeasureType &type) {
        p_sample = sample;
        p_line = line;
        SetType(type);
      };

      //! Return the sample coordinate of the measurement
      double Sample() const {
        return p_sample;
      };

      //! Return the line coordinate of the measurement
      double Line() const {
        return p_line;
      };

      /**
       * @brief Set the error of the coordinate
       *
       * @param serror  Sample error
       * @param lerror  Line error
       */
      void SetError(const double &serror, const double &lerror) {
        p_sampleError = serror;
        p_lineError = lerror;
      };

      /**
       * @brief Sets the Z Scores of the coordinate
       *
       * @param zScoreMin Z Score of Minimum DN
       * @param zScoreMin Z Score of Maximum DN
       */
      void SetZScores(const double &zScoreMin, const double &zScoreMax) {
        p_zScoreMin = zScoreMin;
        p_zScoreMax = zScoreMax;
      }

      /**
       * @brief Returns the minimum zScore
       * @return the minimum zScore
       */
      double GetZScoreMin() const {
        return p_zScoreMin;
      };

      /**
       * @brief Returns the maximum zScore
       * @return the maximum zScore
       */
      double GetZScoreMax() const {
        return p_zScoreMax;
      };

      //! Return error the sample coordinate of the measurement
      double SampleError() const {
        return p_sampleError;
      };

      //! Return error the line coordinate of the measurement
      double LineError() const {
        return p_lineError;
      };

      double ErrorMagnitude() const;

      //! Set how the coordinate was obtained
      void SetType(const MeasureType &type) {
        p_measureType = type;
      };

      //! Return the type of the measurment
      MeasureType Type() const {
        return p_measureType;
      };

      //! Has the measurement be measured??
      bool IsMeasured() const {
        return p_measureType != Unmeasured;
      };

      //! Has the measurement be validated by a human?
      bool IsValidated() const {
        return (p_measureType == ValidatedManual) ||
               (p_measureType == ValidatedAutomatic);
      };

      //! Set if a reference measurement
      void SetReference(bool value) {
        p_isReference = value;
      };

      //! Is the measurement a reference?
      bool IsReference() const {
        return (p_isReference && IsMeasured());
      };

      /**
       * @brief Set cube serial number
       *
       * This method is used to set the serial number of the cube.  That is,
       * the coordinate was selected from a cube with this unique serial
       * number
       *
       * @param sn  Serial number of the cube where the coordinate was
       *            selected
       */
      void SetCubeSerialNumber(const std::string &sn) {
        p_serialNumber = sn;
      };

      //! Return the serial number of the cube containing the coordinate
      std::string CubeSerialNumber() const {
        return p_serialNumber;
      };

      /**
       * @brief Set the crater diameter at the coordinate
       *
       * This method sets the crater diameter at the coordinate.  If
       * left unset a diameter of 0 is assumed which implies no crater
       *
       * @param diameter  The diameter of the crater in pixels
       */
      void SetDiameter(double diameter) {
        p_diameter = diameter;
      };

      //! Return the diameter of the crater in pixels (0 implies no crater)
      double Diameter() const {
        return p_diameter;
      };

      void SetDateTime();

      //! Set date/time the coordinate was last changed to specified date/time
      void SetDateTime(const std::string &datetime) {
        p_dateTime = datetime;
      };

      //! Return the date/time the coordinate was last changed
      std::string DateTime() const {
        return p_dateTime;
      };

      void SetChooserName();

      //! Set the chooser name to an application that last changed the coordinate
      void SetChooserName(const std::string &name) {
        p_chooserName = name;
      };

      //! Return the chooser name
      std::string ChooserName() const {
        return p_chooserName;
      };

      //! Set up to ignore this measurement
      void SetIgnore(bool ignore) {
        p_ignore = ignore;
      };

      //! Return if this measurement should be ignored
      bool Ignore() const {
        return p_ignore;
      };

      //! Set the Goodness of Fit variable
      void SetGoodnessOfFit(const double fit) {
        p_goodnessOfFit = fit;
      };

      //! Return the Goodnes of Fit
      double GoodnessOfFit() const {
        return p_goodnessOfFit;
      };

      //! Set the camera for this measure
      void SetCamera(Isis::Camera *camera) {
        p_camera = camera;
      };

      //! Return the camera associated with this measure
      Isis::Camera *Camera() const {
        return p_camera;
      };

      void SetFocalPlaneMeasured(double x, double y);

      //! Return the measured focal plane x
      double FocalPlaneMeasuredX() const {
        return p_focalPlaneMeasuredX;
      };

      //! Return the measured focal plane y
      double FocalPlaneMeasuredY() const {
        return p_focalPlaneMeasuredY;
      };

      void SetFocalPlaneComputed(double x, double y);

      //! Return the computed focal plane x
      double FocalPlaneComputedX() const {
        return p_focalPlaneComputedX;
      };

      //! Return the computed focal plane y
      double FocalPlaneComputedY() const {
        return p_focalPlaneComputedY;
      };

      //! Set the measured ephemeris time of the measure
      void SetMeasuredEphemerisTime(double et) {
        p_measuredEphemerisTime = et;
      };

      //! Get the measured ephemeris time of the measure
      double MeasuredEphemerisTime() const {
        return p_measuredEphemerisTime;
      };

      //! Set the computed ephemeris time of the measure
      void SetComputedEphemerisTime(double et) {
        p_computedEphemerisTime = et;
      };

      //! Get the computed ephemeris time of the measure
      double ComputedEphemerisTime() const {
        return p_computedEphemerisTime;
      };

      const double GetMeasureData(QString type) const;
      static const QVector< QString > GetMeasureDataNames();

      bool operator == (const Isis::ControlMeasure &pMeasure) const;
      bool operator != (const Isis::ControlMeasure &pMeasure) const;

    private:
      MeasureType p_measureType;
      std::string p_serialNumber;
      double p_line;
      double p_sample;
      double p_diameter;
      std::string p_dateTime;
      std::string p_chooserName;
      bool p_ignore;
      bool p_isReference;
      double p_sampleError;
      double p_lineError;
      double p_zScoreMin;
      double p_zScoreMax;
      double p_goodnessOfFit;
      Isis::Camera *p_camera;
      double p_focalPlaneMeasuredX;
      double p_focalPlaneMeasuredY;
      double p_focalPlaneComputedX;
      double p_focalPlaneComputedY;

      double p_measuredEphemerisTime;
      double p_computedEphemerisTime;
  };
};

#endif
