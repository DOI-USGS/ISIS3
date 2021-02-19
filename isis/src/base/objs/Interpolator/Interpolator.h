#ifndef Interpolator_h
#define Interpolator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SpecialPixel.h"

namespace Isis {
  /**
   * @brief Pixel interpolator.
   *
   * This class is used for interpolating buffers of pixel data. It is usually
   * associated with spacial translation, rotations and scaling in geometric
   * warping algorithums (i.e., rubber sheeting). When special-pixel values are
   * found in the data buffer the current interpolator is abandonded and the next
   * lower interpolator is used instead (i.e.; if Cubic-convolution can not be
   * performed then a bi-linear is used and if the bi-linear can not be performed
   * then nearest-neighbor will be used.
   *
   * @ingroup Geometry
   *
   * @author 2002-10-09 Stuart Sides
   *
   * @internal
   *   @todo This class needs an example.
   *   @history 2003-02-12 Stuart Sides Fixed misspelling of object name in XML
   *   file. (Note: XML files no longer used in documentation.)
   *   @history 2003-05-16 Stuart Sides modified schema from
   *   astrogeology...isis.astrogeology.
   */
  class Interpolator {
    public:
      /**
       * The interpolator type, including: None, Nearest Neighbor, BiLinear
       * or Cubic Convultion.
       */
      enum interpType {
        None = 0,
        NearestNeighborType = 1,
        BiLinearType = 2,
        CubicConvolutionType = 4
      };

    private:
      /**
       * The type of interpolation to be performed. (NearestNeighbor, BiLinear
       * or CubicConvultion
       */
      interpType p_type;


      void Init();


      double NearestNeighbor(const double isamp, const double iline,
                             const double buf[]);

      // Do a bilinear interpolation on the buf
      double BiLinear(const double isamp, const double iline,
                      const double buf[]);

      // Do a cubic convolution on the buf
      double CubicConvolution(const double isamp, const double iline,
                              const double buf[]);


    public:
      // Constructores / destructores
      Interpolator();
      Interpolator(const interpType &type);
      ~Interpolator();


      // Perform the interpolation with the current settings
      double Interpolate(const double isamp, const double iline,
                         const double buf[]);


      // Set the type of interpolation
      void SetType(const interpType &type);


      // Return sizes needed by the interpolator
      int Samples();
      int Lines();


      // Return the hot spot (center pixel) of the interpolator zero based
      double HotSample();
      double HotLine();
  };
};
#endif
