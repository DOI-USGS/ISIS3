#include "ControlNetValidMeasure.h"
#include "Preference.h"
#include "IException.h"

using namespace std;
using namespace Isis;

int main(void) {
  Preference::Preferences(true);
  try {
    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "StandardDeviation");
      op += PvlKeyword("DeltaLine", 100);
      op += PvlKeyword("DeltaSamp", 100);
      op += PvlKeyword("Samples", 15);
      op += PvlKeyword("Lines", 15);
      op += PvlKeyword("MinimumInterest", 0.01);
      op += PvlKeyword("MinDN", 1.0);
      op += PvlKeyword("MaxDN", -1.0);
      op += PvlKeyword("MinEmission", 15.0);
      op += PvlKeyword("MaxEmission", 25.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "None");
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 25.0);
      op += PvlKeyword("MaxEmission", 15.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "StandardDeviation");
      op += PvlKeyword("DeltaLine", 100);
      op += PvlKeyword("DeltaSamp", 100);
      op += PvlKeyword("Samples", 15);
      op += PvlKeyword("Lines", 15);
      op += PvlKeyword("MinimumInterest", 0.01);
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 0.0);
      op += PvlKeyword("MaxEmission", 135.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 150.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "None");
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 0.0);
      op += PvlKeyword("MaxEmission", 135.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);
      op += PvlKeyword("MinResolution", 100.0);
      op += PvlKeyword("MaxResolution", -1.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "None");
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 0.0);
      op += PvlKeyword("MaxEmission", 135.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);
      op += PvlKeyword("MinResolution", 100.0);
      op += PvlKeyword("MaxResolution", 500.0);
      op += PvlKeyword("SampleResidual", 5.0);
      op += PvlKeyword("LineResidual", 5.0);
      op += PvlKeyword("ResidualMagnitude", 10.0);
      op += PvlKeyword("SampleShift", 5.0);
      op += PvlKeyword("LineShift", 5.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "None");
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 0.0);
      op += PvlKeyword("MaxEmission", 135.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);
      op += PvlKeyword("MinResolution", 100.0);
      op += PvlKeyword("MaxResolution", 500.0);
      op += PvlKeyword("SampleResidual", 5.0);
      op += PvlKeyword("LineResidual", 5.0);
      op += PvlKeyword("SampleShift", 5.0);
      op += PvlKeyword("LineShift", 5.0);
      op += PvlKeyword("PixelShift", 10.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Pvl pvlLog;
      PvlGroup op("ValidMeasure");
      op += PvlKeyword("Name", "None");
      op += PvlKeyword("MinDN", -1.0);
      op += PvlKeyword("MaxDN", 1.0);
      op += PvlKeyword("MinEmission", 0.0);
      op += PvlKeyword("MaxEmission", 135.0);
      op += PvlKeyword("MinIncidence", 0.0);
      op += PvlKeyword("MaxIncidence", 135.0);
      op += PvlKeyword("MinResolution", 100.0);
      op += PvlKeyword("MaxResolution", 500.0);
      op += PvlKeyword("SampleResidual", 5.0);
      op += PvlKeyword("LineResidual", 5.0);
      op += PvlKeyword("SampleShift", 5.0);
      op += PvlKeyword("LineShift", 5.0);

      pvlLog += op;

      ControlNetValidMeasure cnetVM(pvlLog);
      cout << cnetVM.GetLogPvl();
    }
    catch(IException &e) {
      e.print();
    }
  }
  catch(IException &e) {
    throw IException(e,
        IException::Unknown, "ControlNetValidMeasure Unit test Exception",
        _FILEINFO_);
  }

  return 0;
}
