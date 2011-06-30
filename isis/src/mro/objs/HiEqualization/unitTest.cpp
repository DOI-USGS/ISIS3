#include <iomanip>

#include "Buffer.h"
#include "HiEqualization.h"
#include "Preference.h"
#include "ProcessByLine.h"
#include "iException.h"

using namespace std;
using namespace Isis;


class TestFunctor {
  public:
    TestFunctor(const Equalization *equalizer, int imageIndex) {
      m_sample = 0;
      m_line = 0;
      m_equalizer = equalizer;
      m_imageIndex = imageIndex;
    }

    void operator()(Buffer &in) {
      if (IsValidPixel(in[m_sample])) {
        int bandIndex = in.Band() - 1;

        cout << "sample " << m_sample + 1 << ", ";
        cout << "line " << m_line + 1 << ": ";
        cout << in[m_sample] << " => ";

        cout << m_equalizer->evaluate(
            in[m_sample], m_imageIndex, bandIndex) << endl;
      }

      m_sample = (m_sample + 1) % in.size();
      m_line++;
    }

  private:
    const Equalization *m_equalizer;
    int m_imageIndex;
    int m_sample;
    int m_line;
};


int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << setprecision(9);

  try {
    string fromList = "FromList.lst";
    string holdList = "HoldList.lst";

    cout << "UnitTest for Equalization" << endl;
    HiEqualization equalizer(fromList);
    equalizer.addHolds(holdList);

    equalizer.calculateStatistics();

    // Open input cube
    FileList imageList(fromList);
    for (unsigned int i = 0; i < imageList.size(); i++) {
      ProcessByLine p;
      CubeAttributeInput att;
      const string inp = imageList[i];
      p.SetInputCube(inp, att);
      TestFunctor func(&equalizer, i);
      p.StartProcessInPlace(func);
      p.EndProcess();
    }
  }
  catch (iException &e) {
    e.Report();
  }
}

