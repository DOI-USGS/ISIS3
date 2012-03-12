/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/15 18:27:43 $
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

#ifndef Chip_h
#define Chip_h

#include <vector>

#include "Affine.h"
#include "Interpolator.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "geos/geom/MultiPolygon.h"

using namespace std;
namespace Isis {
  class Cube;
  class Statistics;

  /**
   * @brief A small chip of data used for pattern matching
   *
   * A chip is a small rectangular area that can be used for pattern
   * matching.  Data can be loaded into the chip manually or by reading
   * directly from a cube.
   *
   * @ingroup PatternMatching
   *
   * @author 2005-05-05 Jeff Anderson
   *
   * @see AutoReg
   * @see AutoRegFactory
   *
   * @internal
   *   @history 2006-07-11  Tracie Sucharski - Added reLoad method to use
   *                          p_cube instead of cube passed in.
   *   @history 2006-08-03  Tracie Sucharski - Added Load and ReLoad method
   *                          to apply scale factor to chip.
   *   @history 2006-08-04  Stuart Sides - Added SetClipPolygon method. If the
   *                          clip polygon is set all pixel values outside the
   *                          polygon will be set to NULL.
   *   @history 2007-10-01  Steven Koechle - Fixed inc in LoadChip to fix an
   *                          infinite loop problem when x.size() never grew
   *                          to be more than 3.
   *   @history 2009-01-19  Steven Koechle - Fixed memory leak
   *   @history 2009-06-02  Stacy Alley, Added a check in the SetSize() method to
   *                          make sure the given samples and lines are not equal
   *                          to or less than zero.
   *   @history 2009-08-19  Kris Becker - Added new Extract method that applies an
   *                          Affine transform to the extract a portion of the
   *                          chip; added an assigment operator that sets the
   *                          entire chip to a single value for convenience;
   *                          added a getter that returns a const reference to the
   *                          internal Affine transform of this chip.
   *   @history 2009-08-20  Steven Lambright - Removed local cube pointer and
   *                          parenthesis operator
   *   @history 2009-08-20 Travis Addair - Added Statistics method
   *   @history 2009-08-28 Kris Becker - Added new Affine setter method to
   *                          establish a new Affine transform to the chip; added
   *                          another Load method that uses a new Affine transform
   *                          to load a chip from a cube.
   *   @history 2009-09-01  Travis Addair, Added valid Min/Max pixel value
   *                          functionality for Statistics method
   *   @history 2010-01-28  Tracie Sucharski - In the Load method (with match
   *                          chip) when calculating control points away from the
   *                          corners, added a linc to move into the center of the
   *                          chip in a non-linear fashion to prevent control
   *                          points that fall in a line and cause the matrix
   *                          inversion to fail.
   *   @history 2010-05-24  Jeannie Walldren - Fixed bug in the Load() method
   *                          (with match chip).  Modified to look for control
   *                          points from each corner, rather than looping around.
   *                          Added a method, PointsColinear() to check whether
   *                          the points added are almost along the same line.
   *                          Moved the code from Load() that chose new points to
   *                          a new method, MovePoints().
   *   @history 2010-06-10  Jeannie Walldren - Modified PointsColinear() method to
   *                          take in user defined tolerance as parameter to allow
   *                          registration of more narrow search chip areas.
   *                          Updated documentation, error messages and unitTest.
   *   @history 2010-06-15  Jeannie Walldren - Added set and accessor methods for
   *                          Read() method's Interpolator::interpType.  Updated
   *                          documentation and unitTest.
   *   @history 2010-09-16  Jeannie Walldren - Updated unitTest, truth file and
   *                          test cube to run properly with ShapeModel changes
   *                          to Sensor class.
   *   @history 2011-03-29  Jai Rideout - Added copy constructor and equals
   *                          operator.
   *   @history 2011-10-02  Kris Becker - Corrected a bug in the
   *                          Extract(Chip, Affine) method when
   *                          computing output pixels coordinates.
   *
   */
  class Chip {
    public:
      Chip();
      Chip(const Chip &other);
      Chip(const int samples, const int lines);
      virtual ~Chip();

      void SetSize(const int samples, const int lines);

      bool IsInsideChip(double sample, double line);

      //! Return the number of samples in the chip
      inline int Samples() const {
        return p_chipSamples;
      };

      //! Return the number of lines in the chip
      inline int Lines() const {
        return p_chipLines;
      };

      //! Returns the expanded filename of the cube from
      //! which this chip was chipped.
      inline string Filename() const {
        return p_filename;
      };

      void SetAllValues(const double &d);

      /**
       * This sets a value in the chip
       *
       * @param sample  Sample position to load (1-based)
       * @param line    Line position to load (1-based)
       * @param value   Value to set
       */
      void SetValue(int sample, int line, const double &value) {
        p_buf[line-1][sample-1] = value;
      }

      /**
       * Loads a Chip with a value.  For example,
       * @code
       * Chip c(10,5);
       * c(1,1) = 1.1;
       * c(10,5) = 1.2;
       * @endcode
       *
       * @param sample    Sample position to load (1-based)
       * @param line      Line position to load (1-based)
       */
      inline double GetValue(int sample, int line) {
        return p_buf[line-1][sample-1];
      }

      /** Get a value from a Chip.  For example,
        * @code
        * Chip c(10,5);
        * cout << c[3,3] << endl;
        * @endcode
        *
        * @param sample    Sample position to get (1-based)
        * @param line      Line position to get (1-based)
        */
      inline const double GetValue(int sample, int line) const {
        return p_buf[line-1][sample-1];
      }

      void TackCube(const double cubeSample, const double cubeLine);

      /**
       * Return the fixed tack sample of the chip. That is, the middle of the
       * chip. It is a chip coordinate not a cube coordinate. For a chip with 5
       * samples, this will return 3, the middle pixel. For a chip with 4
       * samples it will return 2
       */
      inline int TackSample() const {
        return p_tackSample;
      };

      /**
       * Return the fixed tack line of the chip. That is, the middle of the
       * chip. It is a chip coordinate not a cube coordinate. For a chip with 5
       * lines, this will return 3, the middle pixel. For a chip with 4 lines
       * it will return 2
       */
      inline int TackLine() const {
        return p_tackLine;
      };

      void Load(Cube &cube, const double rotation = 0.0, const double scale = 1.0,
                const int band = 1);
      void Load(Cube &cube, Chip &match, Cube &matchChipCube,
                const double scale = 1.0, const int band = 1);
      void Load(Cube &cube, const Affine &affine, const bool &keepPoly = true,
                const int band = 1);

      void SetChipPosition(const double sample, const double line);

      //! Returns cube sample after invoking SetChipPosition
      inline double CubeSample() const {
        return p_cubeSample;
      };

      //! Returns cube line after invoking SetChipPosition
      inline double CubeLine() const {
        return p_cubeLine;
      };

      void SetCubePosition(const double sample, const double line);

      //! Returns chip sample after invoking SetCubePosition
      double ChipSample() const {
        return p_chipSample;
      };

      //! Returns chip line after invoking SetCubePosition
      double ChipLine() const {
        return p_chipLine;
      };

      void SetValidRange(const double minimum = Isis::ValidMinimum,
                         const double maximum = Isis::ValidMaximum);
      bool IsValid(double percentage);

      /** Returns whether the value at the given sample, line position is within the
        * valid range
        *
        * @param sample    Sample position
        * @param line      Line position
        */
      inline bool IsValid(int sample, int line) {
        double value = GetValue(sample, line);
        if(value < p_validMinimum) return false;
        if(value > p_validMaximum) return false;
        return true;
      }

      Chip Extract(int samples, int lines, int samp, int line);
      void Extract(int samp, int line, Chip &output);
      Isis::Statistics *Statistics();
      void Extract(Chip &output, Affine &affine);
      void Write(const string &filename);

      void SetClipPolygon(const geos::geom::MultiPolygon &clipPolygon);

      Chip &operator=(const Chip &other);

      /**
       * @brief Returns the Affine transformation of chip-to-cube indices
       *
       * This method returns the affine transform used to load a chip from the
       * same area as a match cube.  It also is used to track the tack point
       * line and sample translations from the chip indices to the absolute cube
       * coordiates.
       *
       * @return @b const @b Affine& Transform map from chip coordinates to cube
       *         coordinates
       */
      const Affine &GetTransform() const {
        return (p_affine);
      }

      /**
       * @brief Sets the internal Affine transform to new translation
       *
       * Provides the ability to establish a new affine transformation without
       * overhead of, say, loading the chip with a new translation.
       *
       * The caller also has the option to specify the disposition of an
       * established polygon.
       *
       * @param affine   New affine tranform to set for this chip
       * @param keepPoly Indicates whether an existing polygon clipper should be kept
       *                 (default of true)
       */
      void SetTransform(const Affine &affine, const bool &keepPoly = true) {
        p_affine = affine;
        if(!keepPoly) {
          delete p_clipPolygon;
          p_clipPolygon = 0;
        }
        return;
      }

      /**
       * Access method that returns the Interpolator Type used for loading a chip.
       * @return @b const @b Interpolator::interpType Interpolator used to read
       *             data from cube and put it into a chip.
       * @see Read()
       * @see SetReadInterpolator()
       * @author Jeannie Walldren
       * @internal
       *   @history 2010-06-05 Jeannie Walldren - Original version
      */
      const Interpolator::interpType GetReadInterpolator() {
        return p_readInterpolator;
      }


      /**
       * Sets Interpolator Type for loading a chip.  This type is used in the Read()
       * method.
       * @param type Interpolator type to be used.
       * @throws IException::Programmer - Invalid Interpolator Type
       * @see Read()
       * @see SetReadInterpolator()
       * @author Jeannie Walldren
       * @internal
       *   @history 2010-06-05 Jeannie Walldren - Original version
      */
      void SetReadInterpolator(const Interpolator::interpType type) {
        if(type == Interpolator::NearestNeighborType ||
            type == Interpolator::BiLinearType ||
            type == Interpolator::CubicConvolutionType) {
          p_readInterpolator = type;
          return;
        }
        // Interpolator::None is not valid type
        string msg = "Invalid Interpolator type.  Cannot use [";
        msg += iString(type) + "] to read cube into chip.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

    private:
      void Init(const int samples, const int lines);
      void Read(Cube &cube, const int band);
      vector<int> MovePoints(const int startSamp, const int startLine,
                             const int endSamp, const int endLine);
      bool PointsColinear(const double x0, const double y0,
                          const double x1, const double y1,
                          const double x2, const double y2,
                          const double tol);


      int p_chipSamples;                           //!< Number of samples in the chip
      int p_chipLines;                             //!< Number of lines in the chip
      vector<vector <double> > p_buf;              //!< Chip buffer
      int p_tackSample;                            //!< Middle sample of the chip
      int p_tackLine;                              //!< Middle line of the chip

      double p_cubeTackSample;                     //!< cube sample at the chip tack
      double p_cubeTackLine;                       //!< cube line at the chip tack

      double p_validMinimum;                       //!< valid minimum chip pixel value
      double p_validMaximum;                       //!< valid maximum chip pixel value

      double p_chipSample;                         //!< chip sample set by SetChip/CubePosition
      double p_chipLine;                           //!< chip line set by SetChip/CubePosition
      double p_cubeSample;                         //!< cube sample set by SetCubePosition
      double p_cubeLine;                           //!< cube line set by SetCubePosition
      geos::geom::MultiPolygon *p_clipPolygon;     //!< clipping polygon set by SetClipPolygon (line,samp)

      Affine p_affine;                             //!< Transform set by SetTransform.  Used to load cubes into chip
      Interpolator::interpType p_readInterpolator; //!< Interpolator type set by SetReadInterpolator. Used to read cubes into chip.
      string p_filename;                           //!< Filename of loaded cube
  };
};

#endif
