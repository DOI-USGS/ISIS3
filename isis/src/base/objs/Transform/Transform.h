#ifndef Transform_h
#define Transform_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
namespace Isis {
  /**
   * @brief Pixel transformation
   *
   * This class is used as a virtual base class for rubbersheeting (geometric
   * warping) of image data. In Isis, rubbersheeting is performed by a transform
   * from output space to input space, where a space could be a disk cube or an
   * internalized memory cube. In particular, the transform must provide a method
   * for converting output pixel locations (sample,line) to input pixel locations.
   * A very simple example of a transform is a translation (shifting the cube
   * left/right and up/down). More complex transforms can be created such as
   * rotations, scaling, and converting to a map projection. The Transform class
   * is "pure virtual" and should never be instantiated but instead used in an
   * inheriting class. Using the translation example:
   * @code
   *   class Translate : public Transform {
   *     public: Translate (IsisCube &cube, double sampOffset, double lineOffset)
   *     {
   *       p_lines = cube.Lines();
   *       p_samples = cube.Samples();
   *       p_lineOffset = lineOffset;
   *       p_sampOffset = sampOffset;
   *     }
   *     ~Translate () {};
   *     int OutputSamples () { return p_samples; };
   *     int OutputLines() { return p_lines; };
   *     void Transform (double &insamp, double &inline, const double outsamp,
   *                      const double outline)
   *     {
   *      insamp = outsamp + p_sampOffset; inline = outline + p_lineOffset;
   *     }
   *     private:
   *        double p_sampOffset;
   *        double p_lineOffset;
   *        int p_lines;
   *        int p_samples;
   *   };
   * @endcode
   * If you are developing an application program whose intent is to geometrically
   * manipulate the pixels in a cube, you should investigate the RubberSheet
   * object which will deals nicely with a significant amount of the cube access
   * and user input. Also, check out other applications which use the RubberSheet
   * object such as "rotate" or "translate".
   *
   * If you would like to see Transform
   * being used in implementation, see transform.cpp
   *
   * @ingroup Geometry
   *
   * @author 2002-10-14 Stuart Sides
   *
   * @internal
   *  @history 2002-11-14 Stuart Sides - Modified documentation after review
   *                                     by Jeff Anderson
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2004-06-22 Jeff Anderson - Modified Transform method so that it
   *                                      was not const
   *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *
   *  @todo 2005-02-22 Stuart Sides - finish documentation
   */
  class Transform {
    private:

    protected:

    public:

      //! Constructs a Transform object
      Transform() {};

      //! Destroy the Transform object
      virtual ~Transform() {};

      // Pure virtual members

      /**
       * Allows the retrieval of the calculated number of samples in the output
       * image.
       *
       * @return int The number of samples in the output image.
       */
      virtual int OutputSamples() const{
        return 0;
      }

      /**
       * Allows the retrieval of the calculated number of lines in the output
       * image.
       *
       * @return int The number of lines in the output image.
       */
      virtual int OutputLines() const {
        return 0;
      }

      /**
       * Transforms the given output line and sample to the corresponding output
       * line and sample.
       *
       * @param inSample The calculated input sample where the output pixel came
       *                 from.
       *
       * @param inLine The calculated input line where the output pixel came
       *               from.
       *
       * @param outSample The output sample for which an input line and sample is
       *                  being sought
       *
       * @param outLine The output line for which an input line and sample is
       *                being sought
       */
      virtual bool Xform(double &inSample, double &inLine,
                         const double outSample,
                         const double outLine) {
        return true;
      }

  };
};

#endif
