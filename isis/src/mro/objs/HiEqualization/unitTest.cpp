#include <iomanip>

#include "Buffer.h"
#include "HiEqualization.h"
#include "Preference.h"
#include "ProcessByLine.h"
#include "IException.h"
#include "FileName.h"
#include "FileList.h"

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
    FileName fromList("FromList.lst");
    QString holdList = "HoldList.lst";

    cout << "UnitTest for Equalization" << endl;
    HiEqualization equalizer(fromList.toString());
    equalizer.addHolds(holdList);

    equalizer.calculateStatistics();

    // Open input cube
    FileList imageList(fromList);
    for (int i = 0; i < imageList.size(); i++) {
      ProcessByLine p;
      CubeAttributeInput att;
      QString inp = imageList[i].toString();
      Cube *inputCube = p.SetInputCube(inp, att);
      TestFunctor func(&equalizer, inputCube->lineCount(), i);
      p.ProcessCubeInPlace(func, false);
      p.EndProcess();
    }
  }
  catch (IException &e) {
    e.print();
  }
}

