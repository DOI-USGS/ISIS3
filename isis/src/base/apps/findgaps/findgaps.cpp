/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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

#include "Isis.h"

#include <QMessageBox>

#include "FindGapsFunctor.h"
#include "IException.h"
#include "IString.h"
#include "MultivariateStatistics.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  double corTol = ui.GetDouble("CORTOL"); // The correlation tolerance
  int bufferSizeBeforeGap = ui.GetInteger("ABOVE");
  int bufferSizeAfterGap = ui.GetInteger("BELOW");
  bool outputCubeSpecified = (ui.GetAsString("TO") != "none");
  bool logFileSpecified = (ui.GetAsString("LOG") != "none");

  if (outputCubeSpecified || logFileSpecified) {
    ProcessByLine p;
    Cube *iCube = p.SetInputCube("FROM");

    FindGapsFunctor gapsFunctor(iCube->lineCount(), corTol, bufferSizeBeforeGap,
                                bufferSizeAfterGap);
    p.ProcessCubeInPlace(gapsFunctor, false);

    if (outputCubeSpecified) {
      gapsFunctor.setModification("NULL buffers added to output cube");
      p.SetOutputCube("TO");
      p.ProcessCube(gapsFunctor, false);
    }

    if (logFileSpecified) {
      gapsFunctor.gaps().Write(ui.GetFileName("LOG"));
    }
  }
  else {
    throw IException(IException::User,
                     "At least one form of output (a log file or cube) needs to be entered.",
                     _FILEINFO_);
  }
}
