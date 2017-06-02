#include <sstream>
#include "LabelTranslationManager.h"
#include "Preference.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"

using namespace Isis;
using namespace std;
/**
 * Child class of LabelTranslationManager to implement pure virtual methods.
 *
 * @author 2017-01-11 Jeannie Backer
 *
 * @internal
 *  @history 2017-01-11 Jeannie Backer - Original Version. Fixes #4584.
 */
class TestTranslationManager : public LabelTranslationManager {
  public:

    TestTranslationManager(const QString &transFile) : LabelTranslationManager() {
    AddTable(transFile);
    }
  
    TestTranslationManager(std::istream &transStrm) : LabelTranslationManager() {
    AddTable(transStrm);
    }
  
    ~TestTranslationManager() {
    }
  
    virtual QString Translate(QString nName, int findex = 0) {
      QString inputValue = "Test Input, Index " + toString(findex);
      return PvlTranslationTable::Translate(nName, inputValue);
    }
};

int main(void) {
  Preference::Preferences(true);

  try {

    stringstream trnsStrm;
    trnsStrm << "Group = NumberOfLines" << endl;
    trnsStrm << "  Auto" << endl;
    trnsStrm << "  OutputName = Lines" << endl;
    trnsStrm << "  OutputPosition = (\"Object\",\"IsisCube\",";
    trnsStrm <<                      "\"Group\",\"Dimensions\")" << endl;
    trnsStrm << "  InputPosition = (Image,Size)" << endl;
    trnsStrm << "  InputKey = NL" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = NumberOfBands" << endl;
    trnsStrm << "  Auto" << endl;
    trnsStrm << "  Optional" << endl;
    trnsStrm << "  OutputName = Bands" << endl;
    trnsStrm << "  OutputPosition = (\"Object\",\"IsisCube\",";
    trnsStrm <<                      "\"Group\",\"Dimensions\")" << endl;
    trnsStrm << "  InputPosition = (Image,Size)" << endl;
    trnsStrm << "  InputKey = Nb" << endl;
    trnsStrm << "  InputDefault = 1" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = Bonus" << endl;
    trnsStrm << "  Auto" << endl;
    trnsStrm << "  Optional" << endl;
    trnsStrm << "  InputPosition = (Image,Pixel)" << endl;
    trnsStrm << "  InputKey = Bonus" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = Extra" << endl;
    trnsStrm << "  Optional" << endl;
    trnsStrm << "  InputPosition = (Image,Bogus)" << endl;
    trnsStrm << "  InputKey = Extra" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = PixelResolution" << endl;
    trnsStrm << "  InputPosition = (Image,Pixel)" << endl;
    trnsStrm << "  InputKey = Resolution" << endl;
    trnsStrm << "  InputDefault = 1" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = BandName" << endl;
    trnsStrm << "  Auto" << endl;
    trnsStrm << "  OutputName = Band" << endl;
    trnsStrm << "  OutputPosition = (\"Object\",\"IsisCube\",";
    trnsStrm <<                      "\"Object\",\"BandBin\")" << endl;
    trnsStrm << "  InputPosition = (Image,BandInfo)" << endl;
    trnsStrm << "  InputKey = Band" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;
    trnsStrm << "Group = CenterLongitude" << endl;
    trnsStrm << "  Auto" << endl;
    trnsStrm << "  OutputPosition = (\"Group\",\"Mapping\")" << endl;
    trnsStrm << "  OutputName = CenterLongitude" << endl;
    trnsStrm << "  InputPosition = IMAGE_MAP_PROJECTION" << endl;
    trnsStrm << "  InputPosition = (QUBE,IMAGE_MAP_PROJECTION)" << endl;
    trnsStrm << "  InputPosition = (SPECTRAL_QUBE,IMAGE_MAP_PROJECTION)" << endl;
    trnsStrm << "  InputKey = CENTER_LONGITUDE" << endl;
    trnsStrm << "  Translation = (*,*)" << endl;
    trnsStrm << "EndGroup" << endl;

    trnsStrm << "End" << endl;

    TestTranslationManager transMgr(trnsStrm);

    cout << "Testing LabelTranslationManager object" << endl;

    cout << endl << "Testing Translate method:" << endl;
    cout << endl << "Translating Extra: ";
    cout << transMgr.Translate("Extra") << endl;

    cout << endl << "Testing Auto method:" << endl;
    Pvl translatedLabel;
    transMgr.Auto(translatedLabel);
    cout << endl << translatedLabel << endl;

  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
