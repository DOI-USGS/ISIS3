#include <iomanip>

#include "Buffer.h"
#include "Equalization.h"
#include "iException.h"
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
    string fromList = "FromList.lst";
    string holdList = "HoldList.lst";

    cout << "UnitTest for Equalization" << endl;
    Equalization equalizer(fromList);
    equalizer.addHolds(holdList);

    double percent = 100.0;
    int mincount = 1000;
    bool weight = false;
    OverlapNormalization::SolutionType adjust = OverlapNormalization::Both;

    equalizer.calculateStatistics(percent, mincount, weight, adjust);

    // Open input cube
    FileList imageList(fromList);
    for (unsigned int i = 0; i < imageList.size(); i++) {
      ProcessByLine p;
      CubeAttributeInput att;
      const string inp = imageList[i];
      Cube *inputCube = p.SetInputCube(inp, att);
      TestFunctor func(&equalizer, inputCube->getLineCount(), i);
      p.ProcessCubeInPlace(func, false);
    }
  }
  catch (iException &e) {
    e.Report();
  }
}

