/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// $Id: hyb2onccal.cpp 6045 2015-02-07 02:06:59Z moses@GS.DOI.NET $
#include "Isis.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>

#include <QDebug>
#include <QFile>
#include <QString>
#include <QScopedPointer>
#include <QTemporaryFile>
#include <QVector>

#include "AlphaCube.h"
#include "Buffer.h"
#include "FileName.h"
#include "Hyb2OncCalUtils.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "Pixel.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "ProcessByBrick.h"
#include "ProcessByBoxcar.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Spice.h"
#include "Statistics.h"
#include "TextFile.h"


using namespace Isis;
using namespace std;

// Calibration support routines
FileName DetermineFlatFieldFile(const QString &filter);
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out);

QString loadCalibrationVariables(const QString &config);

// Temporary cube file pointer deleter
struct TemporaryCubeDeleter {
  static inline void cleanup(Cube *cube) {
    if ( cube ) {

      FileName filename( cube->fileName() );
      delete cube;
      remove( filename.expanded().toLatin1().data() );
    }
  }
};

enum InstrumentType{ONCW1,ONCW2,ONCT};

static double g_bitDepth(12);

InstrumentType g_instrument;
//For subimage and binning mapping
static AlphaCube *alpha(0);

static QString g_filter = "";
static QString g_target ="";
static Pvl g_configFile;

//Bias calculation variables
static double g_b0(0);
static double g_b1(0);
static double g_b2(0);
static double g_bae0(0);
static double g_bae1(0);
static double g_bias(0);

//Device (AE/CCD/ECT temps for ONC-T,ONC-W1,ONC-W2

static double g_AEtemperature(0.0);

static double g_CCD_T_temperature(0.0);
static double g_ECT_T_temperature(0.0);


static double g_CCD_W1_temperature(0.0);
static double g_ECT_W1_temperature(0.0);


static double g_CCD_W2_temperature(0.0);
static double g_ECT_W2_temperature(0.0);


static QString g_startTime;

//Dark Current variables
static double g_d0(0);
static double g_d1(0);
static double g_darkCurrent(0);

//Linearity correction variables
static double g_L0(0);
static double g_L1(0);
static double g_L2(0);

// TODO: we do not have the readout time (transfer period) for Hayabusa2 ONC.
//Smear calculation variables
static bool g_onBoardSmearCorrection(false);
static double g_Tvct(0);       // Vertical charge-transfer period (in seconds).
static double g_texp(1);       // Exposure time.
static double g_timeRatio(1.0);

// Calibration parameters
static int binning(1);         //!< The number of samples/lines which are binned
static double g_compfactor(1.0);  // Default if OutputMode = LOSS-LESS; 16.0 for LOSSY

static QString g_iofCorrection("IOF");  //!< Is I/F correction to be applied?

//  I/F variables
static double g_solarDist(1.0);  /**< Distance from the Sun to the target body
(used to calculate g_iof) */
static double g_iof(1.0);        //!< I/F conversion value
static double g_iofScale(1.0);
static double g_solarFlux(1.0);  //!< The solar flux (used to calculate g_iof).
// TODO: we do not have this conversion factor for Hayabusa 2 ONC.
static double g_v_standard(1.0);
// static double g_v_standard(3.42E-3);//!< Base conversion for all filters (Tbl. 9)

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    hyb2onccal(ui, &appLog);
  }
  catch (...) {
    for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
      Application::Log(*grpIt);
    }
    throw;
  }

  for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
    Application::Log(*grpIt);
  }
}
