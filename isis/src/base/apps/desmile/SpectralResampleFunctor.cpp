
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

#include "SpectralResampleFunctor.h"

#include "Buffer.h"
#include "Camera.h"
#include "CameraDetectorMap.h"
#include "Spectel.h"
#include "SpectralDefinition.h"

namespace Isis {

  /**
   * Constructs a spectralResampleFunctor, initializing all of the private member variables.
   * 
   * @param inputSpecralDefinition the wavelength center and filter width information about the 
   *                               input cube.
   * @param outputSpectralDefinition the desired or objective wavelength center and filter width 
   *                                 information for the output cube.
   * @param inCamera the camera associated with the input cube.
   */
  SpectralResampleFunctor::SpectralResampleFunctor(SpectralDefinition *inputSpectralDefinition, 
                                                   SpectralDefinition *outputSpectralDefinition,
                                                   Camera *inCamera) {
    m_inSpectralDef = inputSpectralDefinition;
    m_outSpectralDef = outputSpectralDefinition;
    m_inCamera = inCamera; 

  }


  /**
   * Destructor
   */
  SpectralResampleFunctor::~SpectralResampleFunctor() {
  }


  /**
   * Resample the input spectra to match the output spectra 
   */
  void SpectralResampleFunctor::operator()(Buffer &in, Buffer &out) const {
    // If the entire input buffer has invalid pixel DNs:
    //   we are outside the original image or 
    //   this input spectrum is empty
    bool inHasValid = false;
    for (int index=0; index<in.size(); ++index) {
      if (Pixel::IsValid(in[index])) {
        inHasValid = true;
        break;
      }
    }

    // OUTPUT DRIVEN POSSIBILITY
    //if (inHasValid) {
    //  std::cout << "VALID" << std::endl;
    //
    //  // Find the original/raw input image coordinates of each input buffer position, and
    //  // create a spectral definition for them. Later we can ask this definition which spectels are
    //  // needed to fill an output spectrum (1 or more)
    //  //
    //  // TODO: handle sections
    //  SpectralDefinition1D inDef;
    //  for (int inIndex=0; inIndex<in.size(); ++inIndex) {
    //    int status = m_inCamera->SetImage(out.Sample(inIndex), out.Line(inIndex));
    //    if (status == 0) {
    //      double rawInSample = m_inCamera->DetectorMap()->DetectorSample();
    //      double rawInLine = m_inCamera->DetectorMap()->DetectorLine();
    //      Spectel inSpectel = m_inSpectralDef->getSpectel(rawInSample, rawInLine, inIndex+1);
    //      inSpectel.addDN(in[inIndex]);
    //      inDef.addSpectel(inSpectel);
    //    }
    //  }
    //
    //  for (int outIndex=0; outIndex<in.size(); ++outIndex) {
    //
    //    Spectel outSpectel = m_outSpectralDef->getSpectel(out.Sample(outIndex), out.Line(outIndex), outIndex+1);
    //
    //    out[outIndex] = m_inSpectralDef.getNearestNeighborDN(outSpectel);
    //
    //  }
    //}

    // INPUT DRIVEN NEAREST NEIGHBOR
    // TODO:
    //   handle unset output buffer positions
    //   Think about what needs to be done and what not when the input is NOT projected

    // The input driven process may not place a valid DN into every output buffer position, so
    // fill the output buffer with NULLs 
    for (int outIndex=0; outIndex<out.size(); ++outIndex) {
      out[outIndex] = Isis::NULL8;
    }

    if (inHasValid) {
      // TODO: Change to two for loops 0->in.numsections & 0->in.section.bandcount then remove call to getSectionNumber below
      for (int inIndex=0; inIndex<in.size(); ++inIndex) {
        // For projected images, the input image sample, line will not correctly identify the
        // spectel characteristics (wavelength, width) from the spectral definition. We need to
        // map them back to the unprojected image sample and line.
        m_inCamera->SetBand(inIndex+1);
        int status = m_inCamera->SetImage(in.Sample(inIndex), in.Line(inIndex));
        if (status) {
          double rawSample = m_inCamera->DetectorMap()->DetectorSample();
          double rawLine = m_inCamera->DetectorMap()->DetectorLine();
          //std::cout << " Get Spectel in(s,l,b) " << in.Sample(inIndex) << " " << in.Line(inIndex) << 
          //             " " << inIndex+1 << "   raw(s,l,b): " << rawSample << " " << rawLine << " " << inIndex+1 << std::endl;

          // Check that raw(Sample, Line, Band) are inside the original image
          // Don't check the line bounds. This algorithm only works for line scan instruments
          if (rawSample < 0.5 || rawSample > m_inSpectralDef->sampleCount() + 0.5 || 
              inIndex+1 < 1 || inIndex+1 > m_inSpectralDef->bandCount()) {
            continue;
          }

          // Get the input Spectel associated with original s,l,b
          // TODO: getSpectel needs to take double samp,line,band values
          Spectel inSpectel = m_inSpectralDef->findSpectel(rawSample+0.5, rawLine+0.5, inIndex+1);
          int definitionSection = m_inSpectralDef->sectionNumber(rawSample, rawLine, inIndex+1);

          // Look up the associated output Spectel by searching for the closest wavelength

          Spectel outSpectel = m_outSpectralDef->findSpectel(inSpectel, definitionSection);

          // Move this input DN to the appropriate output band
          out[outSpectel.band() - 1] = in[inIndex]; 
        }
      }
    }
  }
}
