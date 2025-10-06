/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QString>
#include "Cube.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"
#include "SpecialPixel.h"
#include "ShadowCamUtilities.h"
#include "ShadowCamConstants.h"
#include "shadowcam2isis.h"

using namespace std;

namespace Isis {

  void shadowcam2isis(UserInterface &ui, Pvl *log) {
    try{
      bool keepSpecial = ui.GetBoolean("KEEPSPECIALPIXELS");
      FileName from = ui.GetCubeName("FROM");

      // Use a smart pointer for automatic memory management
      auto inLbl = std::make_unique<Pvl>(from.expanded());

      // Error check for instrument label
      PvlGroup instrument = inLbl->findObject("IsisCube").findGroup("Instrument");

      if (!instrument.hasKeyword("InstrumentId") && !instrument.hasKeyword("InstrumentID")) {
        QString msg = "Keyword InstrumentID or InstrumentId was not found in labels.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (QString::compare(instrument["InstrumentId"], "ShadowCam", Qt::CaseInsensitive) != 0) {
        QString msg = "Error: InstrumentId not equal to ShadowCam.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // get xterm, bterm, lines from raw edr label
      int bterm[6];
      int xterm[6];

      for(int i=0; i<6; i++) {
        bterm[i] = 9999;
        xterm[i] = 9999;
      }

      xterm[5] = 4096; 

      // Load term arrays
      for (int i = 0; i < 6; i++) {
        if (i != 0) {
          QString bt = "Bterm" + toString(i);
          bterm[i] = (GetFromLabels(instrument, bt)).toInt();
        }

        if (i != 5) {
          QString xt = "Xterm" + toString(i);
          xterm[i] = (GetFromLabels(instrument, xt)).toInt();
        }
      }

      /**
       * @brief Lambda to compand
       *
       * This function is used to create a companding table for the decompanding routine.
       *
       * @param dn Incoming DN pixel value.
       * @param bterm ShadowCam bterms.
       * @param xterm ShadowCam xterms.
       * @return Companded 8-bit value.
       **/
      const auto compand = [&](uint16_t dn, int bterm[5], int xterm[5]) -> uint8_t {
        for (int i = 0; i < 6; i++) {
          if (dn < xterm[i]) {
            return ((dn >> i) + bterm[i]) & 0xff;
          }
        }
        QString msg = "Failed to compand value: " + toString(dn) + "\n";
        throw IException(IException::User, msg, _FILEINFO_);
      };

      /**
       * @brief Lambda to decompand
       *
       * Decompands 8-bit DN values and handles special pixels.
       *
       * @param in Input buffer.
       * @param out Output buffer.
       **/
      const auto decompand = [&](Isis::Buffer &in, Isis::Buffer &out) -> void { 
        uint8_t companding_table[4096];

        // Create companding table
        for (uint16_t dn12 = 0; dn12 < 4096; dn12++) {
          companding_table[dn12] = compand(dn12, bterm, xterm);
        }

        uint16_t decompanding_table[256];

        // Create decompanding table
        for (uint16_t dn = 0; dn < 256; dn++) {
          uint16_t min = 9999, max = 0;
          for (uint16_t dn12 = 0; dn12 < 4096; dn12++) {
            if (companding_table[dn12] == dn) {
              if (min == 9999) 
                min = dn12;

              max = dn12;
            }

            if (min != 9999 && companding_table[dn12] != dn)
              break;
          }

          decompanding_table[dn] = (min + max) / 2;
        }

        int size = SHC_CHANNELS * SHC_AFE_WIDTH;

        // handle 8 bit special pixels first

        // Process buffer
        for (int i = 0; i < size; i++) {
          uint16_t tmpVal = static_cast<float> (in[i]);
          float tmpFloatVal;

          if(keepSpecial)
            tmpFloatVal = Set8bitMaxMintoSpecialPixelsHIS4LIS4(tmpVal);
          else
            tmpFloatVal = static_cast<float>(tmpVal);

          if(!IsSpecialPixelSHC(tmpFloatVal)){
            // only decompanding non-special pixels
            out[i] = static_cast<uint16_t>(decompanding_table[(uint16_t) tmpFloatVal]);
            if (out[i] < 0){
              string msg  =  "Value is less than zero for line: " + std::to_string(in.Line()) + ", pixel: " + std::to_string(i);
              throw IException(IException::User, msg, _FILEINFO_);
            }
          }
          
          
          /*
          if (IsSpecialPixelSHC(in[i])) {
            if (keepSpecial)
              out[i] = Set8bitMaxMintoSpecialPixelsHIS4LIS4(in[i]);
            else {
              // Special pixel handling
              cout << "WARNING: Special pixels set to 0 or 255." << endl;
              out[i] = static_cast<float>(Set_LIS_HIS_SpecialPixelsTo_0_255(in[i]));
            }
          } 
          else {
            // Decompanding non-special pixels
            out[i] = static_cast<float>(decompanding_table[(uint16_t) in[i]]);
            #if (out[i] < 0){
             # cout << "Value is less than zero for line: " << std::to_string(in.Line()) << ", pixel: " << std::to_string(in[i]) << "at(i): " << std::to_string(in.at(i)) << endl;
          }*/
        }
      };

      // Process raw EDR cube
      CubeAttributeOutput outAttr = CubeAttributeOutput("+Real");
      ProcessByLine p;
      p.SetInputCube("FROM");
      p.SetOutputCube(ui.GetCubeName("TO"), outAttr);
      p.Progress()->SetText("Importing 8-bit EDR cube and decompanding...");
      p.StartProcess(decompand);
      p.Progress()->SetText("Finalizing import...");
      p.Finalize();
      p.ClearCubes();
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to import shadowcam image to ISIS", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to import shadowcam image to ISIS" << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to import shadowcam image to ISIS", _FILEINFO_);
    }
  }
}
