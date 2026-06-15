#include "lineeq.h"

#include <vector>

#include "Application.h"
#include "IException.h"
#include "ProcessByLine.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "QuickFilter.h"
#include "Statistics.h"
#include "TextFile.h"

using namespace std;

namespace Isis {

  /**
   * Normalize a cube based on line averages. This is the programmatic interface
   * to the lineeq application.
   *
   * The line averages for each band are collected, smoothed with a lowpass
   * boxcar filter, and then used to normalize every line. Optionally writes the
   * raw and smoothed line averages to a CSV file.
   *
   * @param ui The user interface used to gather parameters for the equalization.
   *
   * @return PvlGroup The results group describing the boxcar size used and CSV
   *                  output settings.
   */
  PvlGroup lineeq(UserInterface &ui) {
    ProcessByLine p;
    Cube *icube = p.SetInputCube(ui.GetCubeName("FROM"), ui.GetInputAttribute("FROM"));
    int numIgnoredLines = 0;

    // Per-band cube averages and per-band, per-line averages. Local state keeps
    // lineeq re-entrant so it can be called repeatedly in-process.
    vector<double> cubeAverage(icube->bandCount(), 0.0);
    vector< vector<double> > lineAverages(icube->bandCount(),
                                          vector<double>(icube->lineCount(), 0.0));

    int boxcarSize = 0;

    if (ui.GetString("BOXTYPE").compare("NONE") == 0) {
      boxcarSize = (int)(icube->lineCount() * 0.10);
    }
    else if (ui.GetString("BOXTYPE").compare("ABSOLUTE") == 0) {
      boxcarSize = ui.GetInteger("BOXSIZE");
    }
    else if (ui.GetString("BOXTYPE").compare("PERCENTAGE") == 0) {
      boxcarSize = (int)(((double)ui.GetInteger("BOXSIZE") / 100.0) * icube->lineCount());
    }

    // Boxcar must be odd size
    if (boxcarSize % 2 != 1) {
      boxcarSize ++;
    }

    PvlGroup results("lineeq");
    results += PvlKeyword("BoxcarSize", toString(boxcarSize), "lines");
    results += PvlKeyword("OutputCsv", toString((int)ui.GetBoolean("AVERAGES")));

    TextFile *csvOutput = NULL;
    if (ui.GetBoolean("AVERAGES")) {
      csvOutput = new TextFile(ui.GetFileName("CSV"), "overwrite", "");
      csvOutput->PutLine("Average,SmoothedAvg");
      results += PvlKeyword("CsvFile", ui.GetFileName("CSV"));
    }

    // Gather the average DN for every line of every band.
    auto gatherAverages = [&](Buffer &in) {
      Statistics lineStats;
      lineStats.AddData(in.DoubleBuffer(), in.size());

      double average = lineStats.Average();

      lineAverages[in.Band() - 1][in.Line() - 1] = average;

      // The cube average will finish being calculated before the correction is applied.
      if (!IsSpecial(average)) {
        cubeAverage[in.Band() - 1] += average;
      }
      else {
        numIgnoredLines ++;
      }
    };

    p.Progress()->SetText("Gathering line averages");
    p.StartProcess(gatherAverages);

    // Now filter the bands
    p.Progress()->SetText("Smoothing line averages");
    p.Progress()->SetMaximumSteps((icube->bandCount() + 1) * icube->lineCount());
    p.Progress()->CheckStatus();
    QuickFilter filter(icube->lineCount(), boxcarSize, 1);

    if (icube->lineCount() <= numIgnoredLines) {
      delete csvOutput;
      throw IException(IException::User, "Image does not contain any valid data.", _FILEINFO_);
    }

    for (int band = 0; band < icube->bandCount(); band ++) {
      cubeAverage[band] /= (icube->lineCount() - numIgnoredLines);
      filter.AddLine(&lineAverages[band][0]);

      for (int line = 0; line < icube->lineCount(); line ++) {
        p.Progress()->CheckStatus();

        double filteredLine = filter.Average(line);

        if (csvOutput != NULL) {
          csvOutput->PutLine(toString(lineAverages[band][line]) + (QString)"," + toString(filteredLine));
        }

        lineAverages[band][line] = filteredLine;
      }

      filter.RemoveLine(&lineAverages[band][0]);
    }

    if (csvOutput != NULL) {
      delete csvOutput; // This closes the file automatically
      csvOutput = NULL;
    }

    // Normalize each line with its smoothed average. Special pixels pass through
    // unchanged.
    auto apply = [&](Buffer &in, Buffer &out) {
      for (int sample = 0; sample < in.size(); sample ++) {
        if (!IsSpecial(in[sample])) {
          out[sample] = in[sample] * cubeAverage[in.Band() - 1] / lineAverages[in.Band() - 1][in.Line() - 1];
        }
        else {
          out[sample] = in[sample];
        }
      }
    };

    p.SetOutputCube(ui.GetCubeName("TO"), ui.GetOutputAttribute("TO"),
                    icube->sampleCount(), icube->lineCount(), icube->bandCount());
    p.Progress()->SetText("Applying Equalization");
    p.StartProcess(apply);

    p.EndProcess();

    return results;
  }
}
