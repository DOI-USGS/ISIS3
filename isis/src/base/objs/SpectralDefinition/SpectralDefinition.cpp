/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/01/07 18:33:38 $
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

#include "SpectralDefinition.h"

#include <vector>
#include <QString>

//Other Isis
#include "ProcessByLine.h"
#include "CSVReader.h"

//using namespace std;
namespace Isis {

  //!Constructs an empty SpectralDefinition object
  SpectralDefinition::SpectralDefinition() {
    init();
  }


  //!default destructor
  SpectralDefinition::~SpectralDefinition() {
  }


  /**
   * @brief Returns the number of samples in the calibration image
   */
  int SpectralDefinition::sampleCount() const {
    return m_ns;
  }


  /**
   * @brief Returns the number of line in the calibration image
   */
  int SpectralDefinition::lineCount() const {
    return m_nl;
  }


  /**
   * @brief Returns the number of bands in the calibration image
   */
  int SpectralDefinition::bandCount() const {
    return m_nb;
  }


  /**
   * @brief Returns the number of sections in the calibration 
   *        image.
   */
  int SpectralDefinition::sectionCount() const {
    return m_numSections; 
  }


  //! Constructor initializer
  void SpectralDefinition::init() {
  
    m_ns = 0;
    m_nl = 0;
    m_nb = 0;
    m_numSections = 0;
  }

}




