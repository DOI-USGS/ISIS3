/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <QList>
#include <QScopedPointer>
#include <QString>

#include "AlphaCube.h"
#include "Blob.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "FileList.h"
#include "FileName.h"
#include "History.h"
#include "IException.h"
#include "LineManager.h"
#include "ProcessByLine.h"
#include "PushFrameCameraCcdLayout.h"
#include "Pvl.h"
#include "Table.h"
#include "UserInterface.h"


using namespace std;
using namespace Isis;

/**
 * Struct for storing information about a filter.
 *
 * @author 2017-09-15 Kristin Berry
 *
 * @internal
 *   @history 2017-09-15 Kristin Berry - Original Version
 *
 *   @history 2018-02-15 Adam Goins - Modified unstitch to parse the archive group
 *                           from the stitched frame. Changed "Name" to "FilterName"
 *                           in bandBin group.
 */
struct FilterInfo : public PushFrameCameraCcdLayout::FrameletInfo {
  FilterInfo() : FrameletInfo(), m_wavelength(0), m_width(0) { }
  FilterInfo(const int frameid) : FrameletInfo(frameid), m_wavelength(0), m_width(0) { }
  FilterInfo(const int frameid, QString filterName, int startSample,
             int startLine, int samples, int lines, double wavelength, double width) :
             FrameletInfo(frameid, filterName, startSample, startLine, samples, lines),
             m_wavelength(wavelength), m_width(width) { }
  double  m_wavelength; //!< The center wavelength of the filter associated with this framelet
  double  m_width; //!< The width of the filter associated with this framelet
};


QList<Cube *> g_outputCubes;
Cube *cube = NULL;
QStringList g_filterList;
QList<FilterInfo> g_frameletInfoList;
void unstitchFullFrame(Buffer &in);

void IsisMain() {
  ProcessByLine p;
  UserInterface &ui = Application::GetUserInterface();
  g_outputCubes.clear();

  // Load in the fullframe cube
  QString from = ui.GetAsString("FROM");
  CubeAttributeInput inAtt(from);
  cube = new Cube();
  cube->setVirtualBands(inAtt.bands());
  from = ui.GetFileName("FROM");
  cube->open(from);

  // Determine the filters / framelets in input fullframe image
  Pvl *inputLabel = cube->label();

  g_frameletInfoList.clear();

  PvlKeyword filterKey = inputLabel->findKeyword("OriginalFilters", PvlObject::Traverse);
  PvlKeyword filterIkCodes = inputLabel->findKeyword("FilterIkCodes", PvlObject::Traverse);
  PvlKeyword filterStartSamples = inputLabel->findKeyword("FilterStartSamples", PvlObject::Traverse);
  PvlKeyword filterSamples = inputLabel->findKeyword("FilterSamples", PvlObject::Traverse);
  PvlKeyword filterStartLines = inputLabel->findKeyword("FilterStartLines", PvlObject::Traverse);
  PvlKeyword filterLines = inputLabel->findKeyword("FilterLines", PvlObject::Traverse);
  PvlKeyword filterWavelength = inputLabel->findKeyword("FilterCenters", PvlObject::Traverse);
  PvlKeyword filterWidth = inputLabel->findKeyword("FilterWidths", PvlObject::Traverse);

  for (int i = 0; i < filterKey.size(); i++) {
    g_frameletInfoList.append(FilterInfo(filterIkCodes[i].toInt(),
                              filterKey[i],
                              filterStartSamples[i].toDouble(),
                              filterStartLines[i].toDouble(),
                              filterSamples[i].toDouble(),
                              filterLines[i].toDouble(),
                              filterWavelength[i].toDouble(),
                              filterWidth[i].toDouble()));
  }

  // Collect the tables and history from the input stitched cube
  QList<Blob> inputTables;
  QScopedPointer<History> inputHistory;
  for(int i = 0; i < inputLabel->objects(); i++) {
    if(inputLabel->object(i).isNamed("Table")) {
      Blob table((QString)inputLabel->object(i)["Name"], inputLabel->object(i).name());
      cube->read(table);
      inputTables.append(table);
    }
    if(inputLabel->object(i).isNamed("History") && Isis::iApp != NULL) {
      inputHistory.reset( new History((QString)inputLabel->object(i)["Name"]) );
      cube->read(*inputHistory);
      inputHistory->AddEntry();
    }
  }

  // Determine sizes of framelets in input fullframe images

  // Allocate this number of total cubes of the correct size
  FileName outputFileName(ui.GetFileName("OUTPUTPREFIX"));

  // Sometimes there will be '.'s in an OUTPUT prefix that could
  // be confused with a file extension
  QString outputBaseName = outputFileName.expanded();
  if (outputFileName.extension() == "cub") {
   outputBaseName = outputFileName.removeExtension().expanded();
  }

  // Create and output a list of
  QFile allCubesListFile(outputBaseName + ".lis");
  if (!allCubesListFile.open(QFile::WriteOnly | QFile::Text)) {
    QString msg = "Unable to write file [" + allCubesListFile.fileName() + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QTextStream allCubesListWriter(&allCubesListFile);

  // Set up framelet output cubes
  Progress progress;
  progress.SetText("Setting up output framelet cubes.");
  progress.SetMaximumSteps(g_frameletInfoList.size());
  for (int i = 0; i < g_frameletInfoList.size(); i++) {
    progress.CheckStatus();
    Cube *frameletCube = new Cube();

    frameletCube->setDimensions(g_frameletInfoList[i].m_samples, g_frameletInfoList[i].m_lines, 1);
    FileName frameletCubeFileName(outputBaseName
                                  + "_" + g_frameletInfoList[i].m_filterName
                                  + ".cub");
    frameletCube->create(frameletCubeFileName.expanded());
    g_outputCubes.append(frameletCube);
    allCubesListWriter << frameletCubeFileName.baseName() << ".cub\n";
  }

  // Unstitch
  p.SetInputCube("FROM");
  p.Progress()->SetText("Processing output cubes.");
  p.StartProcess(unstitchFullFrame);
  p.EndProcess();

  progress.SetText("Updating labels of output cubes.");
  progress.SetMaximumSteps(g_outputCubes.size());
  for (int i = 0; i < g_outputCubes.size(); i++) {
    progress.CheckStatus();
    for (int j = 0; j < inputLabel->findObject("IsisCube").groups(); j++) {
      PvlGroup group = inputLabel->findObject("IsisCube").group(j);

      // The stitched frame has ArchiveRED, ArchiveNIR, ArchivePAN, and ArchiveBLU.
      // We won't add the archive group unless
      if ( group.name().contains("Archive") &&
           group.name() != "Archive" + g_frameletInfoList[i].m_filterName ) {
             continue;
           }

      g_outputCubes[i]->putGroup(group);

    }
    // Update the labels
    Pvl *frameletLabel = g_outputCubes[i]->label();
    frameletLabel->findGroup("Instrument", PvlObject::Traverse).addKeyword(PvlKeyword("Filter",
                                          g_frameletInfoList[i].m_filterName), PvlObject::Replace);

    // Sets the name from ArchiveRED (or NIR, BLU, PAN) to just "Archive" in the unstitched cube.
    frameletLabel->findGroup("Archive" + g_frameletInfoList[i].m_filterName, PvlObject::Traverse).setName("Archive");

    PvlGroup &bandBin = frameletLabel->findGroup("BandBin", PvlObject::Traverse);

    bandBin.addKeyword(PvlKeyword("FilterName", g_frameletInfoList[i].m_filterName),
                                                PvlObject::Replace);
    bandBin.addKeyword(PvlKeyword("Center", toString(g_frameletInfoList[i].m_wavelength)));
    bandBin.addKeyword(PvlKeyword("Width", toString(g_frameletInfoList[i].m_width)));
    bandBin.addKeyword(PvlKeyword("NaifIkCode", toString(g_frameletInfoList[i].m_frameId)));

    // Add the alpha cube
    AlphaCube frameletArea(cube->sampleCount(), cube->lineCount(),
                           g_frameletInfoList[i].m_samples, g_frameletInfoList[i].m_lines,
                           g_frameletInfoList[i].m_startSample + 0.5,
                           g_frameletInfoList[i].m_startLine + 0.5,
                           g_frameletInfoList[i].m_startSample
                             + g_frameletInfoList[i].m_samples + 0.5,
                           g_frameletInfoList[i].m_startLine
                             + g_frameletInfoList[i].m_lines + 0.5);
    frameletArea.UpdateGroup(*g_outputCubes[i]);

    // Delete Stitch group
    frameletLabel->findObject("IsisCube").deleteGroup("Stitch");

    // Propagate Tables
    for (int j = 0; j < inputTables.size(); j++) {
      g_outputCubes[i]->write(inputTables[j]);
    }

    // Propagate History
    g_outputCubes[i]->write(*inputHistory);

    // Close output cube
    g_outputCubes[i]->close();
    delete g_outputCubes[i];
  }
  progress.CheckStatus();

  // Cleanup
  g_outputCubes.clear();
  allCubesListFile.close();
  cube->close();
  delete cube;
  cube = NULL;

  return;
}


/**
 * Separates each of the framelets of the input cube into their own separate output cube.
 *
 * @param in A reference to the input Buffer to process.
 * @internal
 *   @history 2018-02-09 Adam Goins - Modified the second operand of the if() statement
 *                           from in.Line() < [...] to inLine() <= [...] to write all lines
 *                           up to and including the last line. Fixes an error where the last lines
 *                           written would be a line of null pixel DN's.
 *
 *   @history 2018-02-14 Adam Goins - Modified the copying of the data in the buffer to include
 *                           the sample offset (m_startSample) for a cube.
 */
void unstitchFullFrame(Buffer &in) {
  for (int i=0; i < g_frameletInfoList.size(); i++) {

    if (in.Line() >= g_frameletInfoList[i].m_startLine
        && in.Line() <= (g_frameletInfoList[i].m_startLine + g_frameletInfoList[i].m_lines)) {
      int outputCubeLineNumber = (in.Line()-1) % g_frameletInfoList[i].m_startLine + 1;
      LineManager mgr(*g_outputCubes[i]);
      mgr.SetLine(outputCubeLineNumber, 1);

      for (int j = 0; j < mgr.size(); j++) {
        mgr[j] = in[j + g_frameletInfoList[i].m_startSample];
      }
      g_outputCubes[i]->write(mgr);
      return;
    }
  }
}
