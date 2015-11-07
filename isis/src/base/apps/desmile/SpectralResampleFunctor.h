#ifndef SpectralResampleFunctor_h
#define SpectralResampleFunctor_h

/**
 * @file
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

class QString;

namespace Isis {
  class Buffer;
  class SpectralDefinition;
  class Camera;

  /**
   * @brief Resample a spectra
   *
   * this functor provides the support and processing operator for resampling a spectra according to
   * a given input spectral definision and an output spectral definision 
   * 
   * @author 2015-05-04 Stuart Sides and Kristin Berry
   *
   * @internal
   */
  class SpectralResampleFunctor {
    public:
      SpectralResampleFunctor(SpectralDefinition *inputSpectralDefinition, 
                              SpectralDefinition *outputSpectralDefinition,
                              Camera *inCamera);
      ~SpectralResampleFunctor();

      // Processing operator for used by ISIS ProcessBySpectra
      void operator() (Buffer &in, Buffer &out) const;
      

    private:
      SpectralDefinition *m_inSpectralDef;
      SpectralDefinition *m_outSpectralDef;
      
      Camera *m_inCamera;
  };
}

#endif
