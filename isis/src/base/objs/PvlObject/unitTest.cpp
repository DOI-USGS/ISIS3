#include <iostream>
#include <sstream>

#include "PvlObject.h"
#include "PvlTokenizer.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);

  PvlObject o("Beasts");
  o += PvlKeyword("CAT", "Meow");
  cout << o << endl;
  cout << endl;

  PvlGroup g("Fish");
  g += PvlKeyword("Trout", "Brown");
  g += PvlKeyword("Bass", "Large mouth");
  o += g;
  cout << o << endl;
  cout << endl;

  PvlGroup g2("Birds");
  g2 += PvlKeyword("Sparrow", "House");
  g2 += PvlKeyword("Crow");
  o += g2;
  cout << o << endl;
  cout << endl;

  PvlObject o2("Snake");
  o2.addComment("Are slimey");
  o2 += PvlKeyword("Rattler", "DiamondBack");
  o += o2;
  cout << o << endl;
  cout << endl;

  o.findObject("Snake").addGroup(g);
  cout << o << endl;
  cout << endl;

  o.findObject("Snake") += o2;
  cout << o << endl;
  cout << endl;

  cout << "New for PvlObjectFindKeyword" << endl;

  cout << o.hasKeyword("Trout", PvlObject::Traverse) << endl;
  cout << o.findKeyword("Trout", PvlObject::Traverse) << endl;
  cout << o.hasKeyword("Crow", PvlObject::Traverse) << endl;
  cout << o.findKeyword("Crow", PvlObject::Traverse) << endl;
  cout << o.hasKeyword("Rattler", PvlObject::Traverse) << endl;
  cout << o.findKeyword("Rattler", PvlObject::Traverse) << endl;
  cout << o.hasKeyword("Cat", PvlObject::Traverse) << endl;
  cout << o.findKeyword("Cat", PvlObject::Traverse) << endl;

  try {
    cout << o.findKeyword("Trout", PvlObject::None) << endl;
  }
  catch(IException &e) {
    e.print();
  }
  try {
    cout << o.findKeyword("Bus", PvlObject::Traverse) << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << "Keyword Trout should not exist at top level " << o.hasKeyword("Trout", PvlObject::None) << endl;
  cout << "Keyword Bus should dnot exit at top level " << o.hasKeyword("Bus", PvlObject::Traverse) << endl;

  cout << "End new for PvlObjectFindKeyword" << endl;

  cout << "------------" << endl;
  o.findObject("Snake").addObject(o2);
  o.findObject("Snake").findObject("Snake") +=
    PvlKeyword("Gopher", "Constrictor");
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
  catch(IException &e) {
    cout.flush();
    e.print();
  }


  cout << "Testing Object with wrong end tag" << endl;
  try {
    PvlObject o5;
    stringstream os5;

    os5 << "Object = Hello\nKey=Value\nEndGroup\n";
    os5 >> o5;
    cout << o5;
  }
  catch(IException &e) {
    cout.flush();
    e.print();
  }
  
  try {
    // Validate PvlObject
    // Template Object
    PvlObject pvlTmplObjectRoot("Object0");
    PvlObject pvlTmplObject1("Object1");
    PvlObject pvlTmplObject2("Object2");
    PvlGroup pvlTmplGrp("Point_ErrorMagnitude");
    PvlKeyword pvlTmplKwrd("Point_ErrorMagnitude__Required", "true");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("LessThan", "double");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("LessThan__Required", "false");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("LessThan__Repeated", "false");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("GreaterThan", "double");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("GreaterThan__Required", "true");
    pvlTmplGrp += pvlTmplKwrd;
    pvlTmplKwrd.clear();
  
    pvlTmplKwrd = PvlKeyword("GreaterThan__Repeated", "true");
    pvlTmplGrp += pvlTmplKwrd;
   
    pvlTmplObject1 += pvlTmplGrp;
    pvlTmplKwrd.clear();
    pvlTmplKwrd = PvlKeyword("Test_Required", "false");
    pvlTmplObject1 += pvlTmplKwrd;
    pvlTmplKwrd.clear();
     
    pvlTmplKwrd = PvlKeyword("Test_Repeated", "true");
    pvlTmplObject1 += pvlTmplKwrd;
    pvlTmplKwrd.clear();
    
    pvlTmplKwrd = PvlKeyword("Test", "string");
    pvlTmplObject1 += pvlTmplKwrd;

    pvlTmplObject2 += pvlTmplObject1;
    
    pvlTmplObjectRoot += pvlTmplObject2;
     
    cout << "Template Object:\n" << pvlTmplObjectRoot << endl << endl;
    
    // PvlGroup to be Validated
    PvlObject pvlObjectRoot("Object0");
    PvlObject pvlObject1("Object1");
    PvlObject pvlObject2("Object2");
    PvlGroup pvlGrp("Point_errormagnitude");
    PvlKeyword pvlKwrd("LessThan", "2");
    pvlGrp += pvlKwrd;
      
    pvlKwrd.clear();
    pvlKwrd = PvlKeyword("GreaterThan", "3.5");
    pvlGrp += pvlKwrd;
      
    pvlKwrd.clear();
    pvlKwrd = PvlKeyword("GreaterThan", "4.4545");
    pvlGrp += pvlKwrd;
    
    pvlObject1 += pvlGrp;
    
    pvlKwrd.clear();
    pvlKwrd = PvlKeyword("Test", "testing1");
    pvlObject1 += pvlKwrd;
    
    pvlKwrd.clear();
    pvlKwrd = PvlKeyword("Test", "testing2");
    pvlObject1 += pvlKwrd;
    
    pvlKwrd.clear();
    pvlKwrd = PvlKeyword("TestTest", "Not in Template");
    pvlObject1 += pvlKwrd;
  
    pvlObject2 += pvlObject1;
    
    pvlObjectRoot += pvlObject2;
     
    pvlTmplObjectRoot.validateObject(pvlObjectRoot);
    
    cout << "After Validation Results PVL:\n" << pvlObjectRoot << endl;
    
  } catch (IException &e) {
    cout.flush();
    e.print();
  }

  cout << endl << "Testing reallocation ..." << endl;
  PvlObject po;
  po += PvlObject("firstObj");
  PvlObject * ptro1 = &po.findObject("firstObj");
  for (int i = 0; i < 250; i++) 
    po += PvlObject("testObj" + i);

  PvlObject * ptro2 = &po.findObject("firstObj");
  if (ptro1 == ptro2) 
    cout << "PvlObject pointers are equal" << endl; 
  else
    cout << "FAILURE: PvlObject pointers were not the same after adding more objects" << endl;
  
  po += PvlGroup("firstGroup");
  PvlGroup * ptrg1 = &po.findGroup("firstGroup");
  for (int i = 0; i < 250; i++)
    po += PvlObject("testGroup" + i);

  PvlGroup * ptrg2 = &po.findGroup("firstGroup");
  if (ptrg1 == ptrg2)
    cout << "PvlGroup pointers are equal" << endl;
  else
    cout << "FAILURE: PvlGroup pointers were not the same after adding more groups" << endl;
}
