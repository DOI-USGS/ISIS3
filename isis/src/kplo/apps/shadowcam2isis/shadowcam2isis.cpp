/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <array>
#include <cstdint>
#include <memory>

#include <QString>
#include <QStringBuilder>

#include "Buffer.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "UserInterface.h"

#include "ShadowCamConstants.h"
#include "ShadowCamUtilities.h"

#include "shadowcam2isis.h"

using std::uint8_t, std::uint16_t;

namespace Isis {

  void shadowcam2isis(UserInterface &ui) {
    try{
      const bool keepSpecial = ui.GetBoolean("KEEPSPECIALPIXELS");
      const FileName from = FileName(ui.GetCubeName("FROM"));

      if (ShadowCam::IsCalibrated(from)) {
        const QString msg = "File [" + from.name() + "] is calibrated, no need to run shadowcam2isis";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // Use a smart pointer for automatic memory management
      const auto inLabel = std::make_unique<Pvl>(from.expanded());

      // Error check for instrument label
      const PvlGroup &instrument = inLabel->findObject("IsisCube").findGroup("Instrument");

      if (!instrument.hasKeyword("InstrumentId") && !instrument.hasKeyword("InstrumentID")) {
        const QString msg = "Keyword InstrumentID or InstrumentId was not found in labels.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (QString::compare(instrument["InstrumentId"], "ShadowCam", Qt::CaseInsensitive) != 0) {
        const QString msg = "Error: InstrumentId not equal to ShadowCam.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // Get xTerm, bTerm, lines from raw edr label
      std::array<int, 6> bTerm;
      bTerm.fill(9999);
      std::array<int, 6> xTerm;
      xTerm.fill(9999);
      xTerm[5] = 4096; 

      // Load term arrays
      for (int i = 0; i < 6; i++) {
        if (i != 0) {
          QString bTermKey = "Bterm" % toString(i);
          bTerm[i] = (ShadowCam::GetFromLabels(instrument, bTermKey)).toInt();
        }

        if (i != 5) {
          QString xTermKey = "Xterm" % toString(i);
          xTerm[i] = (ShadowCam::GetFromLabels(instrument, xTermKey)).toInt();
        }
      }

      /**
       * @brief Compands DN values to 8-bit values.
       *
       * This function is used to create a companding table for the decompanding routine.
       *
       * @param dn Incoming DN pixel value.
       * @param bTerm ShadowCam bterms.
       * @param xTerm ShadowCam xterms.
       *
       * @return Companded 8-bit value.
       **/
      const auto compand = [](uint16_t dn, std::array<int, 6> bTerm, std::array<int, 6> xTerm) -> uint8_t {
        for (int xTermIndex = 0; static_cast<size_t>(xTermIndex) < xTerm.size(); xTermIndex++) {
          if (dn < xTerm[xTermIndex]) {
            return ((dn >> xTermIndex) + bTerm[xTermIndex]) & 0xff;
          }
        }
        const QString msg = QString("Failed to compand value: %1").arg(toString(dn));
        throw IException(IException::User, msg, _FILEINFO_);
      };

      /**
       * @brief Decompands 8-bit DN values.
       *
       * Decompands 8-bit DN values and handles special pixels.
       *
       * @param in Input buffer.
       * @param out Output buffer.
       **/
      const auto decompand = [&compand, &bTerm, &xTerm, keepSpecial](Buffer &in, Buffer &out) -> void { 
        std::array<uint8_t, 4096> companding_table;

        // Create companding table
        for (uint16_t tableIndex = 0; tableIndex < companding_table.size(); tableIndex++) {
          companding_table[tableIndex] = compand(tableIndex, bTerm, xTerm);
        }

        std::array<uint16_t, 256> decompanding_table;

        // Create decompanding table
        for (uint16_t decompandIndex = 0; decompandIndex < decompanding_table.size(); decompandIndex++) {
          uint16_t min = 9999, max = 0;
          for (uint16_t compandIndex = 0; compandIndex < 4096; compandIndex++) {
            if (companding_table[compandIndex] == decompandIndex) {
              if (min == 9999) {
                min = compandIndex;
              }
              max = compandIndex;
            }

            if (min != 9999 && companding_table[compandIndex] != decompandIndex) {
              break;
            }
          }

          decompanding_table[decompandIndex] = (min + max) / 2;
        }

        constexpr int bufferSize = ShadowCam::SHC_CHANNELS * ShadowCam::SHC_AFE_WIDTH;

        // handle 8 bit special pixels first

        // Process buffer
        for (int bufferIndex = 0; bufferIndex < bufferSize; bufferIndex++) {
          uint16_t tmpVal = static_cast<float> (in[bufferIndex]);
          float tmpFloatVal;
          if (keepSpecial) {
            tmpFloatVal = ShadowCam::Set8bitMaxMintoSpecialPixelsHIS4LIS4(tmpVal);
          }
          else {
            tmpFloatVal = static_cast<float>(tmpVal);
          }

          if (!ShadowCam::IsSpecialPixelSHC(tmpFloatVal)){
            // only decompanding non-special pixels
            out[bufferIndex] = static_cast<uint16_t>(decompanding_table[(uint16_t) tmpFloatVal]);
            if (out[bufferIndex] < 0) {
              QString msg = QString("Value is less than zero for line: %1, pixel: %2.").arg(
                QString::number(in.Line()), QString::number(bufferIndex));
              throw IException(IException::User, msg, _FILEINFO_);
            }
          }
          
          /*
          if (ShadowCam::IsSpecialPixelSHC(in[bufferIndex])) {
            if (keepSpecial)
              out[bufferIndex] = ShadowCam::Set8bitMaxMintoSpecialPixelsHIS4LIS4(in[bufferIndex]);
            else {
              // Special pixel handling
              cout << "WARNING: Special pixels set to 0 or 255." << endl;
              out[bufferIndex] = static_cast<float>(ShadowCam::Set_LIS_HIS_SpecialPixelsTo_0_255(in[bufferIndex]));
            }
          } 
          else {
            // Decompanding non-special pixels
            out[bufferIndex] = static_cast<float>(decompanding_table[(uint16_t) in[bufferIndex]]);
            #if (out[bufferIndex] < 0){
             # cout << "Value is less than zero for line: " << std::to_string(in.Line()) << ", pixel: " << std::to_string(bufferIndex) << "at(i): " << std::to_string(in.at(bufferIndex)) << endl;
          }*/
        }
      };

      // Process raw EDR cube
      CubeAttributeOutput outAttr = CubeAttributeOutput("+Real");
      Isis::CubeAttributeInput &att = ui.GetInputAttribute("FROM");
      ProcessByLine p;
      p.SetInputCube(ui.GetCubeName("FROM"), att);
      p.SetOutputCube(ui.GetCubeName("TO"), outAttr);
      p.Progress()->SetText("Importing 8-bit EDR cube and decompanding...");
      p.StartProcess(decompand);
      p.Progress()->SetText("Finalizing import...");
      p.Finalize();
      p.ClearCubes();
    }
    catch (const IException &e) {
      QString msg = QString("ISIS Exception. Unable to import ShadowCam image to ISIS");
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
    catch (const std::exception &e) {
      QString msg = QString("Standard exception: %1. Unable to import ShadowCam image to ISIS").arg(QString(e.what()));
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occurred. Unable to import ShadowCam image to ISIS", _FILEINFO_);
    }
  }
}
