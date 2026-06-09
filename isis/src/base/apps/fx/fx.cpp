/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2010/04/08 15:28:20 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "fx.h"

#include "Camera.h"
#include "CubeCalculator.h"
#include "CubeInfixToPostfix.h"
#include "FileList.h"
#include "FileName.h"
#include "ProcessByLine.h"

using namespace std;

namespace Isis {

  namespace {
    CubeCalculator c;

    void Evaluate(vector<Buffer *> &input, vector<Buffer *> &output) {
      Buffer &outBuffer = *output[0];

      QVector<Buffer *> inputCopy;

      for (int i = 0; i < (int)input.size(); i++) {
        inputCopy.push_back(input[i]);
      }

      QVector<double> results = c.runCalculations(inputCopy,
                                outBuffer.Line(), outBuffer.Band());

      // If final result is a scalar, set all pixels to that value.
      if (results.size() == 1) {
        for (int i = 0; i < (int)outBuffer.size(); i++) {
          outBuffer[i] = results[0];
        }
      }
      // Final result is a valid vector, write to output buffer
      else {
        for (int i = 0; i < (int)results.size(); i++) {
          outBuffer[i] = results[i];
        }
      }
    }
  }

  void fx(UserInterface &ui) {
    ProcessByLine p;
    QVector<Cube *> cubes;
    Cube *outCube;
    Cube *inCube;
    int bands = 1;

    if (ui.GetString("MODE") == "CUBES") {
      // Require atleast one file to be specified
      QString f1 = ui.GetCubeName("F1");
      CubeAttributeInput att1 = ui.GetInputAttribute("F1");
      inCube = p.SetInputCube(f1, att1, Isis::AllMatchOrOne);
      cubes.push_back(inCube);
      if (ui.WasEntered("F2")){
          QString f2 = ui.GetCubeName("F2");
          CubeAttributeInput att2 = ui.GetInputAttribute("F2");
          inCube = p.SetInputCube(f2, att2, Isis::AllMatchOrOne);
          cubes.push_back(inCube);
      }
      if (ui.WasEntered("F3")){
          QString f3 = ui.GetCubeName("F3");
          CubeAttributeInput att3 = ui.GetInputAttribute("F3");
          inCube = p.SetInputCube(f3, att3, Isis::AllMatchOrOne);
          cubes.push_back(inCube);
      }
      if (ui.WasEntered("F4")){
          QString f4 = ui.GetCubeName("F4");
          CubeAttributeInput att4 = ui.GetInputAttribute("F4");
          inCube = p.SetInputCube(f4, att4, Isis::AllMatchOrOne);
          cubes.push_back(inCube);
      }
      if (ui.WasEntered("F5")){
          QString f5 = ui.GetCubeName("F5");
          CubeAttributeInput att5 = ui.GetInputAttribute("F5");
          inCube = p.SetInputCube(f5, att5, Isis::AllMatchOrOne);
          cubes.push_back(inCube);
      }
      bands = cubes[0]->bandCount();
      QString to = ui.GetCubeName("TO");
      CubeAttributeOutput oatt = ui.GetOutputAttribute("TO");
      int ns = cubes[0]->sampleCount();
      int nl = cubes[0]->lineCount();
      int nb = cubes[0]->bandCount();
      outCube = p.SetOutputCube(to, oatt, ns, nl, nb);
    }
    else if (ui.GetString("MODE") == "LIST") {
      FileList list(ui.GetFileName("FROMLIST"));

      // Run through file list and set its entries as input cubes
      for (int i = 0; i < list.size(); i++) {
        CubeAttributeInput att(list[i].original());
        inCube = p.SetInputCube(list[i].original(), att, Isis::AllMatchOrOne);
        cubes.push_back(inCube);
      }
      bands = cubes[0]->bandCount();
      QString to = ui.GetCubeName("TO");
      CubeAttributeOutput oatt = ui.GetOutputAttribute("TO");
      int ns = cubes[0]->sampleCount();
      int nl = cubes[0]->lineCount();
      int nb = cubes[0]->bandCount();
      outCube = p.SetOutputCube(to, oatt, ns, nl, nb);
    }
    else {
      int lines = ui.GetInteger("LINES");
      int samples = ui.GetInteger("SAMPLES");
      bands = ui.GetInteger("BANDS");
      QString to = ui.GetCubeName("TO");
      CubeAttributeOutput oatt = ui.GetOutputAttribute("TO");
      outCube = p.SetOutputCube(to, oatt, samples, lines, bands);
    }

    CubeInfixToPostfix infixToPostfix;
    c.prepareCalculations(infixToPostfix.convert(ui.GetString("EQUATION")), cubes, outCube);
    p.StartProcess(Evaluate);
    p.EndProcess();
  }

}
