#include "Isis.h"
#include "ProcessByLine.h"
#include "QuickFilter.h"
#include "TextFile.h"
#include "Statistics.h"

using namespace Isis;

void apply(Buffer &in, Buffer &out);
void gatherAverages(Buffer &in);

std::vector<double> cubeAverage;
std::vector< double * > lineAverages;
int numIgnoredLines;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  numIgnoredLines = 0;

  cubeAverage.resize(icube->Bands());
  lineAverages.resize(icube->Bands());

  for(int i = 0; i < icube->Bands(); i++) {
	cubeAverage[i] = 0;
	lineAverages[i] = NULL;
  }

  int boxcarSize;

  if(ui.GetString("BOXTYPE").compare("NONE") == 0) {
    boxcarSize = (int)(icube->Lines() * 0.10);
  }
  else if(ui.GetString("BOXTYPE").compare("ABSOLUTE") == 0) {
    boxcarSize = ui.GetInteger("BOXSIZE");
  }
  else if(ui.GetString("BOXTYPE").compare("PERCENTAGE") == 0) {
    boxcarSize = (int)(((double)ui.GetInteger("BOXSIZE") / 100.0) * icube->Lines());
  }

  // Boxcar must be odd size
  if(boxcarSize % 2 != 1) {
    boxcarSize ++;
  }

  PvlGroup data("lineeq");
  data += PvlKeyword("BoxcarSize", boxcarSize, "lines");
  data += PvlKeyword("OutputCsv", ui.GetBoolean("AVERAGES"));

  TextFile *csvOutput = NULL;
  if(ui.GetBoolean("AVERAGES")) {
    csvOutput = new TextFile(ui.GetFilename("CSV"), "overwrite", "");
    csvOutput->PutLine("Average,SmoothedAvg");
    data += PvlKeyword("CsvFile", ui.GetFilename("CSV"));
  }

  Application::Log(data);

  for(int band = 0; band < icube->Bands(); band ++) {
    lineAverages[band] = new double[icube->Lines()];
  }

  p.Progress()->SetText("Gathering line averages");
  p.StartProcess(gatherAverages);

  // Now filter the bands
  p.Progress()->SetText("Smoothing line averages");
  p.Progress()->SetMaximumSteps((icube->Bands() + 1) * icube->Lines());
  p.Progress()->CheckStatus();
  QuickFilter filter(icube->Lines(), boxcarSize, 1);

  if(icube->Lines() <= numIgnoredLines) {
	throw iException::Message(iException::User, "Image does not contain any valid data.", _FILEINFO_);
  }

  for(int band = 0; band < icube->Bands(); band ++) {
    cubeAverage[band] /= (icube->Lines() - numIgnoredLines);
    filter.AddLine(lineAverages[band]);

    for(int line = 0; line < icube->Lines(); line ++) {
      p.Progress()->CheckStatus();

	  double filteredLine = filter.Average(line);

      if(csvOutput != NULL) {
        csvOutput->PutLine((iString)lineAverages[band][line] + (iString)"," + (iString)filteredLine);
      }

      lineAverages[band][line] = filteredLine;
    }

    filter.RemoveLine(lineAverages[band]);
  }

  if(csvOutput != NULL) {
    delete csvOutput; // This closes the file automatically
    csvOutput = NULL;
  }

  p.SetOutputCube("TO");
  p.Progress()->SetText("Applying Equalization");
  p.StartProcess(apply);

  for(int band = 0; band < icube->Bands(); band ++) {
    delete [] lineAverages[band];
    lineAverages[band] = NULL;
  }

  p.EndProcess();
}

void gatherAverages(Buffer &in) {
  Statistics lineStats;
  lineStats.AddData(in.DoubleBuffer(), in.size());

  double average = lineStats.Average();

  lineAverages[in.Band() - 1][in.Line() - 1] = average;

  // The cube average will finish being calculated before the correction is applied.
  if(!IsSpecial(average)) {
    cubeAverage[in.Band() - 1] += average;
  }
  else {
	numIgnoredLines ++;
  }
}

void apply(Buffer &in, Buffer &out) {
  for(int sample = 0; sample < in.size(); sample ++) {
    out[sample] = in[sample] * cubeAverage[in.Band() - 1] / lineAverages[in.Band() - 1][in.Line() - 1];
  }
}


