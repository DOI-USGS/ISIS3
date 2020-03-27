#ifndef ProcessRubberSheet_h
#define ProcessRubberSheet_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2009/06/05 16:17:13 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
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

#include "Process.h"
#include "Buffer.h"
#include "Transform.h"
#include "Interpolator.h"
#include "Portal.h"
#include "TileManager.h"

namespace Isis {
  class Brick;

  /**
   * @brief Derivative of Process, designed for geometric transformations
   *
   * This is the processing class for geometric transformations of cube data.
   * Objects of this class can be used to apply rubber sheet transformations
   * from one space to another, such as converting from one map projection to
   * another or from instrument space to ground space. Each pixel position in
   * the output cube will be processed by querying a transformer to find what
   * input pixel should be used and an interpolator to find the value of the
   * pixel. Any application using this class must supply a Transform object and
   * an Interpolator object. This class allows only one input cube and one
   * output cube.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2002-10-22 Stuart Sides
   *
   * @internal
   *   @history 2003-02-13 Stuart Sides - Created a unit test for the object.
   *   @history 2003-03-31 Stuart Sides - Added false argument to
   *                           IsisError.report call in the unit test.
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-05-28 Stuart Sides - Added a new member function to allow
   *                           an application to be notified when the current
   *                           band number of the output cube changes
   *                           (BandChange).
   *   @history 2003-10-23 Jeff Anderson - Modified StartProcess method to use a
   *                           quadtree mechanism to speed processing of cubes.
   *                           Essentially it generates an internalized file
   *   @history 2004-03-09 Stuart Sides - Modified quadtree mechanism to not
   *                           give up on a quad unless none of the edge pixels
   *                           transform. This needs to be refined even more.
   *   @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2006-03-28 Tracie Sucharski - Modified the StartProcess to
   *                           process multi-band cubes using the same tile map
   *                           if there is no Band dependent function.
   *   @history 2006-04-04 Tracie Sucharski - Added ForceTile method and two
   *                           private variables to hold the sample/line of the
   *                           input cube position which will force tile
   *                           containing that position to be processed in
   *                           ProcessQuad even if all 4 corners of tile are
   *                           bad.
   *   @history 2008-09-10 Steven Lambright - Made tiling start and end sizes
   *                           variable
   *   @history 2009-05-21 Steven Lambright - A tiling hint starting at 2 will
   *                           now automatically jump to doing a SlowGeom
   *                           instead of not working/being a valid option. This
   *                           fixes 2-line push frame cameras.
   *   @history 2008-10-30 Steven Lambright - Fixed problem with definition
   *                           of class Quad, pointed out by "novus0x2a"
   *                           (Support Board Member)
   *   @history 2009-06-05 Steven Lambright - Added TestLine(...) method and
   *                           made the checking for inserting a null tile more
   *                           strict (checks the outline of a 2x2 box instead
   *                           of just the outline of the quad).
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                            $temporary variable instead of /tmp directory.
   *   @history 2012-05-07 Jeff Anderson - Added the forward patch algorithm.
   *   @history 2012-05-15 Jeff Anderson - Added UniqueCachingAlgorithm to
   *                           reverse patch algorithm (startProcess)
   *   @history 2012-07-11 Jeff Anderson - Fixed a bug in the reverse patch
   *                           algorithm.  When an input patch contains the
   *                           0/360 seam (or -180/180) it caused stripes in
   *                           certain output projections (cylindrical).
   *                           This has been fixed by making sure the output
   *                           patch size is not too large.
   *   @history 2012-08-03 Kimberly Oyama and Steven Lambright - Changed the
   *                           percentage (from 99% to 50%) of the output cube
   *                           that a patch can cover before it is split into
   *                           a smaller patch. The 99% produced unpredictable
   *                           results on different machines due to double
   *                           rounding error. References #604.
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to StartProcess
   *                                            function to ensure successful
   *                                            inheritance between Process and its
   *                                            child classes.  Made destructor virtual.
   *                                            References #2215.
   *   @history 2017-06-09 Christopher Combs - Changed loop counter int in
                               StartProcess to long long int. References #4611.
   *
   *   @todo 2005-02-11 Stuart Sides - finish documentation and add coded and
   *                        implementation example to class documentation
   */
  class ProcessRubberSheet : public Process {
    public:

      ProcessRubberSheet(int startSize = 128, int endSize = 8);

      //! Destroys the RubberSheet object.
      virtual ~ProcessRubberSheet() {};

      using Isis::Process::StartProcess;
      // Output driven processing method for one input and output cube
      virtual void StartProcess(Transform &trans, Interpolator &interp);

      // Input driven processing method for one input and output cube
      virtual void processPatchTransform(Transform &trans, Interpolator &interp);

      // Register a function to be called when the band number changes
      virtual void BandChange(void (*funct)(const int band));

      virtual void ForceTile(double Samp, double Line) {
        p_forceSamp = Samp;
        p_forceLine = Line;
      }

      /**
       * This sets the start and end tile sizes for the rubber sheet; numbers
       * are inclusive and must be powers of 2.
       *
       * @param start Start position; must be at least 4 and a power of 2
       * @param end End position; must be at least 4, a power of 2 and less than
       *          start
       */
      virtual void SetTiling(long long start, long long end) {
        p_startQuadSize = start;
        p_endQuadSize = end;
      }

      virtual void setPatchParameters(int startSample, int startLine,
                                int samples, int lines,
                                int sampleIncrement, int lineIncrement);


    private:

      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class Quad {
        public:
          int slineTile; //!<
          int ssampTile; //!<
          int sline;     //!<
          int ssamp;     //!<
          int eline;     //!<
          int esamp;     //!<
      };

      void ProcessQuad(std::vector<Quad *> &quadTree, Transform &trans,
                       std::vector< std::vector<double> > &lineMap,
                       std::vector< std::vector<double> > &sampMap);

      void SplitQuad(std::vector<Quad *> &quadTree);
      void SlowQuad(std::vector<Quad *> &quadTree, Transform &trans,
                    std::vector< std::vector<double> > &lineMap,
                    std::vector< std::vector<double> > &sampMap);
      double Det4x4(double m[4][4]);
      double Det3x3(double m[3][3]);

      // SlowGeom method is never used but saved for posterity
      void SlowGeom(TileManager &otile, Portal &iportal,
                    Transform &trans, Interpolator &interp);
      void QuadTree(TileManager &otile, Portal &iportal,
                    Transform &trans, Interpolator &interp,
                    bool useLastTileMap);

      bool TestLine(Transform &trans, int ssamp, int esamp, int sline,
                    int eline, int increment);

      void (*p_bandChangeFunct)(const int band);

      void transformPatch (double startingSample, double endingSample,
                           double startingLine, double endingLine,
                           Portal &iportal, Transform &trans, Interpolator &interp);

      void splitPatch (double startingSample, double endingSample,
                       double startingLine, double endingLine,
                       Portal &iportal, Transform &trans, Interpolator &interp);
#if 0
      void transformPatch (double startingSample, double endingSample,
                           double startingLine, double endingLine);

      void splitPatch (double startingSample, double endingSample,
                       double startingLine, double endingLine);
#endif

      std::vector< std::vector<double> > p_sampMap; //!<
      std::vector< std::vector<double> > p_lineMap; //!<

      double p_forceSamp; //!<
      double p_forceLine; //!<

      long long p_startQuadSize; //!<
      long long p_endQuadSize;   //!<

      int m_patchStartSample;
      int m_patchStartLine;
      int m_patchSamples;
      int m_patchLines;
      int m_patchSampleIncrement;
      int m_patchLineIncrement;

#if 0
      Portal *m_iportal;
      Brick *m_obrick;
      Transform *m_transform;
      Interpolator *m_interpolator;
#endif
  };
};

#endif
