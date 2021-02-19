/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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




