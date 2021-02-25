#ifndef ProcessImportVicar_h
#define ProcessImportVicar_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessImport.h"

namespace Isis {
  /**
   * @brief Import a Vicar file
   *
   * This class allows a programmer to develop application programs which import
   * Vicar cubes and mangles the the Vicar labels into appropriate ISIS labels.
   *
   * Here is an example of how to use ProcessImportVicar
   * @code
   *   ImportVicar p;
   *   Pvl inlab;
   *   p.SetVicarFile("test.vic",inlab);
   *   CubeInfo *opack = p.SetOutputCube("TO");
   *   p.StartProcess();
   *   // extract vicar keywords ...
   *   Pvl outLab;
   *   outLab.addGroup("VICAR");
   *   outLab.addKeyword("RecordSize")",inlab.GetInteger("RECSIZ"); opack->addGroup(outLab,"VICAR");
   *   p.EndProcess();
   * @endcode
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-02-13 Jeff Anderson
   *
   * @internal
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2008-06-26 Christopher Austin - Added the termination char to
   *           SetVicarFile's buf
   *  @history 2011-01-27 Jai Rideout - Fixed to handle VICAR files that
   *           have end labels
   *  @history 2016-04-21 Makayla Shepherd - Added UnsignedWord pixel type handling.
   *
   */

  class ProcessImportVicar : public ProcessImport {

    public:
      void SetVicarFile(const QString &vicarFile, Pvl &vicarLab);

    private:
      QString ExtractPvlLabel(int startPos, std::ifstream &vicarFile) const;
  };
};

#endif


