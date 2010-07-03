#include <vector>
#include "PvlKeyword.h"
#include "PvlSequence.h" 
#include "Preference.h"

int main (void) {
  Isis::Preference::Preferences(true);

  Isis::PvlKeyword key("Key");
  key += std::string("(xyzzy,plover)");
  key += std::string("(2,b,|,^,2,b)");

  Isis::PvlSequence seq;
  seq = key;

  seq += std::string("(a,b,c)");
  seq += std::string("(d,e)");
  seq += std::string("singleton");

  std::vector<std::string> slist;
  slist.push_back("1");
  slist.push_back("2");
  seq += slist;

  std::vector<int> ilist;
  ilist.push_back(-1);
  ilist.push_back(-2);
  ilist.push_back(-3);
  seq += ilist;

  std::vector<double> dlist;
  dlist.push_back(1.0);
  dlist.push_back(0.0);
  dlist.push_back(-1.0);
  seq += dlist;

  std::cout << seq.Size() << std::endl;
  for (int i=0; i<seq.Size(); i++) {
    for (int j=0; j<(int)seq[i].size(); j++) {
      std::cout << seq[i][j] << " ";
    }
    std::cout << std::endl;
  }

  seq.Clear();
  std::cout << seq.Size() << std::endl;
  return 0;
}
