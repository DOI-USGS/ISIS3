#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <QString>
#include <QStringBuilder>

#include "FileName.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlContainer.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"

#include "ShadowCamConstants.h"
#include "ShadowCamUtilities.h"

namespace Isis {
  namespace ShadowCam {

    float Set8bitMaxMintoSpecialPixelsHIS4LIS4(std::uint16_t in) {
      if(in == 0) {
        return LOW_INSTR_SAT4;
      }
      else if (in == 255) {
        return HIGH_INSTR_SAT4;
      }
      else {
        return static_cast<float>(in);
      }
    }

    std::uint16_t Set_LIS_HIS_SpecialPixelsTo_0_255(std::uint16_t in) {
      if (IsNullPixel(in) || IsLowPixel(in)) {
        return 0;
      }
      else if (IsHighPixel(in)) {
        return 255;
      }
      else {
        QString msg = "WARNING: Pixel value is not (LOW_INSTR_SAT8, HIGH_INSTR_SAT8, NULL). Contact programmer to update code to handle this.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    bool IsSpecialPixelSHC(double pixel) {
      return IsNullPixel(pixel) || IsHighPixel(pixel) || IsHrsPixel(pixel) || IsLowPixel(pixel) || IsLrsPixel(pixel);
    }

    std::size_t GetDataIndex(int channel, int channel_width, int pixel) {
      return static_cast<std::size_t>(channel) * channel_width + pixel;
    }

    std::string GetVersionedFilename(const QString &filename) {

      FileName resolvedFilename(filename);

      if (resolvedFilename.isVersioned()) {
        resolvedFilename = resolvedFilename.highestVersion();
      }
      
      if (!resolvedFilename.fileExists()) {
        QString msg = resolvedFilename.expanded() + " does not exist.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      return resolvedFilename.expanded().toStdString();
    }

    QString GetFromLabels(const PvlContainer &pvlContainer, const QString &pvlKeyword) {
      if(!pvlContainer.hasKeyword(pvlKeyword)){
        QString msg = "Error: keyword: " + pvlKeyword + " was not found in labels.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      return QString(pvlContainer[pvlKeyword]);
    }

    int GetTdiFactor(const PvlGroup &instrumentGroup) {
      if (!instrumentGroup.hasKeyword("TDIDirection")) {
        throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
      }

      int tdiFactor = 0;
      if (QString::compare(instrumentGroup["TDIDirection"], "A", Qt::CaseInsensitive) == 0) {
        tdiFactor = 0;
      }
      else if (QString::compare(instrumentGroup["TDIDirection"], "B", Qt::CaseInsensitive) == 0) {
        tdiFactor = 1;
      }
      else {
        QString msg = "Error: TDIDirection value: " % QString(instrumentGroup["TDIDirection"]) % " is not valid. Expected 'A' or 'B'.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      return tdiFactor;
    }

    std::string ToLower(const std::string &str){
      return QString::fromStdString(str).toLower().toStdString();
    }

    bool ContainsKeyword(const std::string &line, const std::vector<std::string> &keywords) {
      for (const auto &keyword : keywords) {
        if (line.find(keyword) != std::string::npos) {
          return true;
        }
      }
      return false;
    }

    bool IsCalibrated(const FileName &file) {
      bool isCalibrated = "c" == file.baseName().toLower().back();
      const Pvl label(file.expanded());
      const PvlGroup &dimGroup = label.findGroup("Dimensions", Pvl::Traverse);
      bool isCorrectSize = int(dimGroup["Samples"]) == ShadowCam::SHC_CHANNELS * ShadowCam::SHC_AFE_WIDTH;
      return isCalibrated || !isCorrectSize;
    }
  }
}