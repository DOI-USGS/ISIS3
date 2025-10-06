#pragma once 

#include <QString>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include "IException.h"
#include "CubeAttribute.h"
#include "SpecialPixel.h"
#include "FileName.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlContainer.h"
#include "IException.h"
#include "Preference.h"
#include "ShadowCamConstants.h"

using namespace std;

namespace Isis {

  float Set8bitMaxMintoSpecialPixelsHIS4LIS4(uint16_t in);

  uint16_t Set_LIS_HIS_SpecialPixelsTo_0_255(uint16_t in);

  bool IsSpecialPixelSHC(double in);

  size_t GetDataIndex(int channel, int channel_width, int pixel);
  
  std::string GetVersionedFilename(QString q_filename);

  QString GetFromLabels(PvlContainer pvlContainer, QString pvlKeyword);

  int GetTdiFactor(PvlGroup instrument);

  std::string ToLower(const std::string& str);

  bool ContainsKeyword(const std::string& line, const vector<std::string>& keywords);

};
