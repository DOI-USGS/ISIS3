#include <iostream>
#include <sstream>

#include "PvlObject.h"
#include "PvlTokenizer.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main () {
  Preference::Preferences(true);

  PvlObject o("Beasts");
  o += PvlKeyword("CAT","Meow");
  cout << o << endl;
  cout << endl;

  PvlGroup g("Fish");
  g += PvlKeyword("Trout","Brown");
  g += PvlKeyword("Bass","Large mouth");
  o += g;
  cout << o << endl;
  cout << endl;

  PvlGroup g2("Birds");
  g2 += PvlKeyword("Sparrow","House");
  g2 += PvlKeyword("Crow");
  o += g2;
  cout << o << endl;
  cout << endl;

  PvlObject o2("Snake");
  o2.AddComment("Are slimey");
  o2 += PvlKeyword("Rattler","DiamondBack");
  o += o2;
  cout << o << endl;
  cout << endl;

  o.FindObject("Snake").AddGroup(g);
  cout << o << endl;
  cout << endl;

  o.FindObject("Snake") += o2;
  cout << o << endl;
  cout << endl;

  cout << "New for PvlObjectFindKeyword" << endl;
  
  cout << o.HasKeyword("Trout", PvlObject::Traverse) << endl; 
  cout << o.FindKeyword("Trout", PvlObject::Traverse) << endl; 
  cout << o.HasKeyword("Crow",PvlObject::Traverse) << endl; 
  cout << o.FindKeyword("Crow",PvlObject::Traverse) << endl; 
  cout << o.HasKeyword("Rattler",PvlObject::Traverse) << endl; 
  cout << o.FindKeyword("Rattler",PvlObject::Traverse) << endl; 
  cout << o.HasKeyword("Cat",PvlObject::Traverse) << endl; 
  cout << o.FindKeyword("Cat",PvlObject::Traverse) << endl; 

  try {
    cout << o.FindKeyword("Trout", PvlObject::None) << endl; 
  }
  catch (iException &e) {
    e.Report(false);
  }
  try {
    cout << o.FindKeyword("Bus", PvlObject::Traverse) << endl; 
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << "Keyword Trout should not exist at top level " << o.HasKeyword("Trout", PvlObject::None) << endl; 
  cout << "Keyword Bus should dnot exit at top level " << o.HasKeyword("Bus", PvlObject::Traverse) << endl; 

  cout << "End new for PvlObjectFindKeyword" << endl;

  cout << "------------" << endl;
  o.FindObject("Snake").AddObject(o2);
  o.FindObject("Snake").FindObject("Snake") += 
                        PvlKeyword("Gopher","Constrictor");
  cout << o << endl;
  cout << endl;


  stringstream os;
  os << o;

  cout << "------------" << endl;

  PvlObject o3;
  os >> o3;
  cout << o3 << endl;

  PvlObject o4;
  stringstream os4;

  os4 << "Object = Hello\nKey=Value\nEndObject";
  os4 >> o4;
  cout << o4 << endl << endl;

  cout << "Testing Object with no end tag" << endl;
  try {
    PvlObject o5;
    stringstream os5;
  
    os5 << "Object = Hello\nKey=Value\n";
    os5 >> o5;
    cout << o5;
  }
  catch(iException &e) {
    cout.flush();
    e.Report(false);
  }


  cout << "Testing Object with wrong end tag" << endl;
  try {
    PvlObject o5;
    stringstream os5;
  
    os5 << "Object = Hello\nKey=Value\nEndGroup\n";
    os5 >> o5;
    cout << o5;
  }
  catch(iException &e) {
    cout.flush();
    e.Report(false);
  }
}
