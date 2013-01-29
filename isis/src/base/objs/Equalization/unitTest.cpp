#include <iomanip>

#include "Buffer.h"
#include "Equalization.h"
#include "IException.h"
#include "OverlapNormalization.h"
#include "OverlapStatistics.h"
#include "PvlGroup.h"
#include "Preference.h"
#include "ProcessByLine.h"

using namespace std;
using namespace Isis;


class TestFunctor {
  public:
    TestFunctor(const Equalization *equalizer, int lineCount, int imageIndex) {
      m_equalizer = equalizer;
      m_imageIndex = imageIndex;
      m_lineCount = lineCount;
    }

    void operator()(Buffer &in) const {
      int lineIndex = ((in.Band() - 1) * m_lineCount + (in.Line() - 1));
      int sampleIndex = lineIndex % in.size();

      if (IsValidPixel(in[sampleIndex])) {
        int bandIndex = in.Band() - 1;

        cout << "sample " << sampleIndex + 1 << ", ";
        cout << "line " << (lineIndex + 1) << ": ";
        cout << in[sampleIndex] << " => ";

        cout << m_equalizer->evaluate(
            in[sampleIndex], m_imageIndex, bandIndex) << endl;
      }
    }

  private:
    const Equalization *m_equalizer;
    int m_imageIndex;
    int m_lineCount;
};


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    QString fromList = "FromList.lst";
    QString holdList = "HoldList.lst";
    QString toList   = "ToList.lst";

    cout << "UnitTest for Equalization" << endl;
    cout << "Create object without fromlist file." << endl;
    Equalization equalizerNoFromList();

    cout << "Create object using fromlist file." << endl;
    Equalization equalizer(fromList);
    cout << "     Add hold list..." << endl;
    equalizer.addHolds(holdList);

    double percent = 100.0;
    int mincount = 1000;
    bool weight = false;
    OverlapNormalization::SolutionType adjust = OverlapNormalization::Both;
    LeastSquares::SolveMethod solvemethod = LeastSquares::QRD;

    cout << "     Calculate statistics for contrast and brightness using QRD method..." << endl;
    equalizer.calculateStatistics(percent, mincount, weight, adjust, solvemethod);

    PvlGroup results = equalizer.getResults();
    cout << "Results:" << endl;
    cout << results << endl;

    cout << "     Write results to file..." << endl;
    FileName outputStatsFile("$TEMPORARY/results.tmp.cub");
    equalizer.write(outputStatsFile.expanded());

    // Open input cube
    FileList imageList(fromList);
    for (int i = 0; i < imageList.size(); i++) {
      ProcessByLine p;
      CubeAttributeInput att;
      const QString inp = imageList[i].toString();
      Cube *inputCube = p.SetInputCube(inp, att);
      TestFunctor func(&equalizer, inputCube->lineCount(), i);
      p.ProcessCubeInPlace(func, false);
    }

    cout << "     Apply corrections to temporary output cubes..." << endl;
    equalizer.applyCorrection(toList);

    cout << "     Import statistics from results file..." << endl;
    equalizer.importStatistics(outputStatsFile.expanded());
    for (int i = 0; i < imageList.size(); i++) {
      ProcessByLine p;
      CubeAttributeInput att;
      const QString inp = imageList[i].toString();
      Cube *inputCube = p.SetInputCube(inp, att);
      TestFunctor func(&equalizer, inputCube->lineCount(), i);
      p.ProcessCubeInPlace(func, false);
    }

    remove(outputStatsFile.expanded().toAscii());
    FileList toFileList(toList);
    for (int i = 0; i < toFileList.size(); i++) {
      remove(toFileList[i].expanded().toAscii());
    }


  }
  catch (IException &e) {
    e.print();
  }
}

