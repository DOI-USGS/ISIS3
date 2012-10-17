#ifndef Spice_h
#define Spice_h
/**
 * @file
 * $Revision: 1.21 $
 * $Date: 2010/04/09 22:31:16 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include "Pvl.h"
#include "ShapeModel.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

namespace Isis {
  class iTime;
  class Distance;
  class EllipsoidShape;
  class Longitude;
  class Target;

  /**
   * @brief Obtain SPICE information for a spacecraft
   *
   * This class initializes standard NAIF SPICE kernels in order to allow queries
   * of a spacecraft's position and attitude at a given time. It also allows for
   * access to the position of the sun and transformation matrices from J2000 to a
   * body-fixed reference frame for a given target (e.g., Mars). The constructor
   * for this class expects a PVL object with the following minimum information:
   *   @code
   *     Group = Instrument
   *       TargetName = Mars
   *     EndGroup
   *     Group = Kernels
   *       NaifFrameCode       = -94030
   *       LeapSecond          = naif0007.tls
   *       TargetAttitudeShape = pck00006.tpc
   *       TargetPosition      = de405.bsp
   *       InstrumentPointing  = (mgs_sc_ab1.bc,
   *       Instrument          = moc13.ti
   *       SpacecraftClock     = MGS_SCLKSCET.00045.tsc
   *       InstrumentPosition  = mgs_ab1.bsp
   *       InstrumentAddendum  = mocAddendum.ti
   *     EndGroup
   *   @endcode
   * This group is typically found in the image labels after it has been run
   * through the program "spiceinit" It is recommended you read NAIF documentation
   * to obtain a better understanding about the various types of SPICE kernels.
   * The NAIF toolkit accesses information from kernels on a
   * last-in-first-out (LIFO) basis. This means that the creation of a second
   * object can cause problems with the first object. To alleviate this problem we
   * have supplied the CreateCache method which should be invoked immediately
   * after the object is constructed. This caches information (spacecraft
   * position, pointing, etc) internally in the object and unloads all NAIF
   * kernels.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2003-03-13 Jeff Anderson
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2003-10-15 Jeff Anderson - Added requirement for frame kernel in
   *                                      labels
   *  @history 2003-10-28 Jeff Anderson - Changed SpaceCraft to Spacecraft in
   *                                      labels and method names
   *  @history 2003-11-03 Jeff Anderson - Added SubSolarPoint and
   *                                      SubSpacecraftPoint methods
   *  @history 2003-11-12 Jeff Anderson - Added Target method
   *  @history 2004-01-14 Jeff Anderson - Changed how the SPK, CK, and Instrument
   *                                      codes where handled. The instrument code
   *                                      must be in the labels as NaifFrameCode
   *                                      and then the other two can be
   *                                      automatically computed.
   *  @history 2004-02-18 Jeff Anderson - Modified to ignore kernel labels which
   *                                      were blank
   *  @history 2004-03-25 Jeff Anderson - Modified NaifBodyCode method to convert
   *                                      Jupiter target code of 599 to 5
   *  @history 2004-03-25 Jeff Anderson - Fixed bug in destructor and added
   *                                      GetString method.
   *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2005-02-24 Jeff Anderson - Modified SubSolarPoint and
   *                                      SubSpacecraftPoint to return positive
   *                                      longitudes only
   *  @history 2005-09-12 Jeff Anderson - Check for case-insensitive values for
   *                                      TargetName of SKY
   *  @history 2005-09-20 Jeff Anderson - Added IsSky method
   *  @history 2006-01-05 Debbie A. Cook - Added units to comments
   *  @history 2006-03-28 Jeff Anderson - Refactored using SpiceRotation and
   *                                      SpicePosition classes.  Added Tables
   *                                      and nadir kernel information.
   *  @history 2006-03-31 Elizabeth Miller - Added TargetCenterDistance method
   *  @history 2006-04-19 Elizabeth Miller - Added SolarLongitude method
   *  @history 2007-01-30 Tracie Sucharski - Throw error in the load method before
   *                              calling furnish if the file does not exist.
   *  @history 2007-07-09 Steven Lambright - Frame kernel is now optional, added Extra kernel
   *                              support.
   *  @history 2007-07-10 Debbie A. Cook - Modified method ComputeSolarLongitude to use
   *                                       pxform instead of tipbod to get body-fixed to
   *                                       J2000 rotation matrix so that the correct frame
   *                                       will be used.  If the frame is different from the
   *                                       default IAU frame, the correct frame should be set
   *                                       in the iak file (see frames.req).  Also modified
   *                                       setting of m_bodyRotation frameCode.  The old code
   *                                       forced the IAU_ frame.  The new code uses the Naif
   *                                       routine cidfrm to get the frame associated with the
   *                                       body id.  These change will recognize any frame
   *                                       changes made in the iak file.
   *  @history 2007-08-10 Steven Lambright - Added support for Nadir keyword in InstrumentPointing group
   *                                       to not be the first element in the PvlKeyword.
   *  @history 2007-08-24 Debbie A. Cook - Removed m_sB so it is recalculated every time it is used
   *                                       insuring that any updates to the position or rotation are applied.
   *                                       Also removed m_BP since it is no longer used
   *  @history 2008-02-13 Steven Lambright - Added StartPadding and EndPadding caching capabilties
   *  @history 2008-02-13 Steven Lambright - Added Support Check for StartPadding and EndPadding caching capabilties;
   *                                        An clarified exception is thrown if a framing camera tries to use time padding
   *  @history 2008-02-27 Kris Becker - Modified so that planetary ephemeris SPKs
   *                                    are loaded before spacecraft SPKs so that
   *                                    missions that augment planet ephemerides
   *                                    will take precidence.
   *  @history 2008-06-23 Steven Lambright - Added NaifStatus error checking
   *  @history 2008-06-25 Debbie A. Cook - Added method InstrumentVelocity to support miniRF
   *  @history 2008-11-28 Debbie A. Cook - Added method hasKernels()
   *  @history 2009-03-18 Tracie Sucharski - Cleaned up some unnecessary,obsolete code.  Make sure the
   *                                    table is used if the kernel names follow the "Table" keyword value, due to change
   *                                    made to spiceinit to retain kernel names if the spice is written to blob.
   *  @history 2009-06-18 Debbie A. Cook - Modified to downsize instrument rotation table when loading cache
   *  @history 2009-07-01 Debbie A. Cook - Modified to downsize body rotation, and sun position tables when loading cache
   *  @history 2009-08-03 Debbie A. Cook - Added tolerance argument to method
   *                                       CreateCache to allow downsizing of
   *                                       instrument position Spice table.
   *  @history 2009-08-21 Kris Becker - Moved the NAIF code methods to public
   *                                    scope.
   *  @history 2010-01-29 Debbie A. Cook - Redid Tracie's change to make sure the table is loaded instead of the
   *                                        kernels if the kernel keyword value lists "Table" before the kernel files.
   *  @history 2010-03-19 Debbie A. Cook - Added constructor and moved common constructor initialization into
   *                                        new method Init.  Also added parameter notab to method Load.
   *  @history 2010-04-09 Debbie A. Cook - Moved the loading of the "extra" kernel(s) from the middle of
   *                                       the loads to the end.
   *  @history 2011-02-08 Jeannie Walldren - Added documentation to methods and private variables. Commented out
   *                                         createCache(double,double) since it appears that this method is not
   *                                         needed. Initialize pointers to NULL in Init() method.
   *  @history 2011-02-09 Steven Lambright - Refactored to use iTime where
   *                                         possible. Changed m_radii to a
   *                                         Distance so the units are no longer
   *                                         ambiguous. These changes were meant
   *                                         for readability and reducing the
   *                                         likelyhood of future code having
   *                                         bugs due to unit mismatches.
   *  @history 2011-02-11 Jeannie Walldren - Changed documentation references to
   *                                         SetEphemerisTime() method (these
   *                                         were replaced with references to
   *                                         setTime()). Added missing
   *                                         documentation to new methods.
   *  @history 2011-05-03 Jeannie Walldren - Added Isis Disclaimer to files.
   *  @history 2011-05-25 Janet Barrett and Steven Lambright - Added API that
   *                                         stores naif values and arbitrary
   *                                         computations so that the text
   *                                         kernels do not have to be
   *                                         furnished. This makes the
   *                                         Camera instantiation much, much
   *                                         quicker. Text kernels are no longer
   *                                         furnished when their data has been
   *                                         stored in the labels.
   *  @history 2011-05-26 Debbie A. Cook -  Put back the code for spkwriter that
   *                                         was checked in May 25 but disappeared
   *                                         in the May 26 build.  This code turns
   *                                         aberration corrections off for the
   *                                         instrument position if the spk file
   *                                         was created by spkwriter.
   *  @history 2011-07-08 Jeff Anderson  -  Fixed Init method to record the
   *                                         integer body frame code in the labels
   *                                         of the cube. Vesta exposed this
   *                                         problem because it was not a
   *                                         instrinsic body in the NAIF toolkit
   *                                         version 63.
   *  @history 2011-07-11 Jeff Anderson  -  Added private copy constructors and
   *                                         operator= methods
   *  @history 2011-09-19 Debbie Cook -  Added cubes with Ideal Cameras to the
   *                                         exclusion list for reading instrument
   *                                         keywords from the label.  The Ideal
   *                                         Camera has variable values for the
   *                                         affine coefficients that are set in
   *                                         the camera itself and not read from
   *                                         a kernel.  The camera puts these
   *                                         values into the Naif kernel pool.
   *  @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *  @history 2012-09-10 Steven Lambright - Undid Debbie's change on 2012-09-19 because the
   *                           Ideal camera now supports putting those keywords in the label
   *                           on the fly. References #1094.
   *  @history 2012-10-11 Debbie A. Cook - Deleted deprecated createCache code already commented
   *                                         out for over a year.  Updated to use new Target and ShapeModel classes.
   *                                         Added Resolution method needed for Target and its ShapeModel.
   *                                         Changed private member names from p_ to m_ to comply with coding
   *                                         standards.  References Mantis tickets #775 and #1114.
   */
  class Spice {
    public:
      // constructors
      Spice(Pvl &label);
      Spice(Pvl &label, bool noTables);

      // destructor
      ~Spice();

      // Methods
      void setTime(const iTime &time);
      void instrumentPosition(double p[3]) const;
      void sunPosition(double p[3]) const;
      double targetCenterDistance() const;
      Longitude solarLongitude();
      void instrumentVelocity(double v[3]) const;
      iTime time() const;

      void radii(Distance r[3]) const;

      void createCache(iTime startTime, iTime endTime,
                       const int size, double tol);
      iTime cacheStartTime() const;
      iTime cacheEndTime() const;

      void subSpacecraftPoint(double &lat, double &lon);
      void subSolarPoint(double &lat, double &lon);

      Target *target() const;
      IString targetName() const;

      iTime getClockTime(IString clockValue,
                         int sclkCode = -1);
      SpiceDouble getDouble(const IString &key, int index = 0);
      SpiceInt getInteger(const IString &key,   int index = 0);
      IString getString(const IString &key,     int index = 0);

      /**
       * Accessor method for the sun position.
       * @return @b iTime Sun position for the image.
       * @author Steven Lambright
       * @internal
       *   @history 2011-02-09 Steven Lambright - Original version.
       */
      SpicePosition *sunPosition() const {
        return m_sunPosition;
      };

      /**
       * Accessor method for the instrument position.
       * @return @b iTime Instrument position for the image.
       * @author Steven Lambright
       * @internal
       *   @history 2011-02-09 Steven Lambright - Original version.
       */
      SpicePosition *instrumentPosition() const {
        return m_instrumentPosition;
      };

      /**
       * Accessor method for the body rotation.
       * @return @b iTime Body rotation for the image.
       * @author Steven Lambright
       * @internal
       *   @history 2011-02-09 Steven Lambright - Original version.
       */
      SpiceRotation *bodyRotation() const {
        return m_bodyRotation;
      };

      /**
       * Accessor method for the instrument rotation.
       * @return @b iTime Instrument rotation for the image.
       * @author Steven Lambright
       * @internal
       *   @history 2011-02-09 Steven Lambright - Original version.
       */
      SpiceRotation *instrumentRotation() const {
        return m_instrumentRotation;
      };

      bool hasKernels(Pvl &lab);

      SpiceInt naifBodyCode() const;
      SpiceInt naifSpkCode() const;
      SpiceInt naifCkCode() const;
      SpiceInt naifIkCode() const;
      SpiceInt naifSclkCode() const;

      PvlObject getStoredNaifKeywords() const;

      /**
       * Pure virtual method that returns the pixel resolution of the sensor in
       * meters/pix.
       *
       * @return @b double Resolution value of 1.0
       */
      virtual double Resolution() {
        return 1.;
          };

    protected:
      enum SpiceValueType {
        SpiceDoubleType,
        SpiceStringType,
        SpiceIntType,
        SpiceByteCodeType
      };

      QVariant readValue(IString key, SpiceValueType type, int index = 0);

      void storeResult(IString name, SpiceValueType type, QVariant value);
      QVariant getStoredResult(IString name, SpiceValueType type);

      void storeValue(IString key, int index, SpiceValueType type,
                      QVariant value);
      QVariant readStoredValue(IString key, SpiceValueType type, int index);

      // Leave these protected so that inheriting classes don't
      // have to convert between double and spicedouble
      // None of the below data elements are usable (except
      // m_radii) until SetEphemerisTime is invoked
      SpiceDouble m_uB[3];    /**< This contains the sun position (u) in the
                                  bodyfixed reference frame (B). It is left
                                  protected so that conversions between double
                                  and SpiceDouble do not have to occur in
                                  inheriting classes. Units are km */
      SpiceDouble m_BJ[3][3]; /**< This contains the transformation matrix from
                                  J2000 (J) to Body fixed (B). Recall that the
                                  transpose of this matrix JB will convert from
                                  body-fixed to J2000. It is left in protected
                                  space so that conversions between double and
                                  SpiceDouble do not have to occur in inheriting
                                  classes.*/


    private:
      void init(Pvl &lab, bool noTables);

      void load(PvlKeyword &key, bool notab);
      void computeSolarLongitude(iTime et);

      Longitude *m_solarLongitude; //!< Body rotation solar longitude value
      iTime *m_et; //!< Ephemeris time (read NAIF documentation for a detailed description)
      QVector<IString> * m_kernels; //!< Vector containing kernels filenames
      Target *m_target; //!< Target of the observation

      // cache stuff
      iTime *m_startTime; //!< Corrected start (shutter open) time of the observation.
      iTime *m_endTime; //!< Corrected end (shutter close) time of the observation.
      SpiceDouble *m_cacheSize; //!< Cache size.  Note:  This value is 1 for Framing cameras.

      SpiceDouble *m_startTimePadding; //!< Kernels pvl group StartPadding keyword value
      SpiceDouble *m_endTimePadding; //!< Kernels pvl group EndPadding keyword value

      SpicePosition *m_instrumentPosition; //!< Instrument spice position
      SpiceRotation *m_instrumentRotation; //!< Instrument spice rotation
      SpicePosition *m_sunPosition; //!< Sun spice position
      SpiceRotation *m_bodyRotation; //!< Body spice rotation

      bool m_allowDownsizing; //!< Indicates whether to allow downsizing

      // Constants
      //      SpiceInt *m_bodyCode;        /**< The NaifBodyCode value, if it exists in the
      //                                        labels. Otherwise, if the target is sky,
      //                                        it's the SPK code and if not sky then it's
      //                                        calculated by the naifBodyCode() method.*/
      SpiceInt *m_spkCode;         //!< Spacecraft and planet ephemeris kernel (SPK) code
      SpiceInt *m_ckCode;          //!< Camera kernel (CK) code
      SpiceInt *m_ikCode;          //!< Instrument kernel (IK) code
      SpiceInt *m_sclkCode;        //!< Spacecraft clock correlation kernel (SCLK) code
      SpiceInt *m_spkBodyCode;     //!< Spacecraft and planet ephemeris kernel (SPK) body code

      PvlObject *m_naifKeywords;

      bool m_usingNaif;

      // Don't allow copies
      Spice(const Spice &other);
      Spice &operator=(const Spice &other);

  };
}

#endif
