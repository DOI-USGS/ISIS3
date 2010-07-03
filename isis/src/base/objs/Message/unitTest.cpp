#include <iostream>
#include "Message.h"
#include "Preference.h"

using namespace std;
int main (void) {
  Isis::Preference::Preferences(true);


//***************************************************************************

  cout << Isis::Message::ArraySubscriptNotInRange (100000) 
       << endl;

  cout << endl;

//***************************************************************************

  cout << Isis::Message::KeywordAmbiguous ("KEY")
       << endl; 

  cout << Isis::Message::KeywordUnrecognized ("KEY")
       << endl; 

  cout << Isis::Message::KeywordDuplicated ("KEY")
       << endl; 

  cout << Isis::Message::KeywordNotArray ("KEY")
       << endl; 

  cout << Isis::Message::KeywordNotFound ("KEY")
       << endl; 

  cout << endl;

//***************************************************************************

  cout << Isis::Message::KeywordBlockInvalid ("BLOCK")
       << endl; 

  cout << Isis::Message::KeywordBlockStartMissing ("BLOCK","FOUND")
       << endl; 

  cout << Isis::Message::KeywordBlockEndMissing ("BLOCK","FOUND")
       << endl; 

  cout << endl;

//***************************************************************************

  cout << Isis::Message::KeywordValueBad ("KEY")
       << endl; 

  cout << Isis::Message::KeywordValueBad ("KEY","12345678901234567890") 
       << endl; 

  cout << Isis::Message::KeywordValueBad ("KEY","abcdefghijklmnopqrstuvwxyz")
       <<endl; 

  cout << Isis::Message::KeywordValueExpected ("KEY")
       <<endl; 

  cout << Isis::Message::KeywordValueNotInRange ("KEY","0","(0,20]")
       <<endl; 

  vector<string> list;
  list.push_back ("X"); list.push_back ("Y"); list.push_back ("Z");
  cout << Isis::Message::KeywordValueNotInList ("KEY","A",list)
       <<endl; 

  cout << endl;

//***************************************************************************

  cout << Isis::Message::MissingDelimiter (')')
       <<endl; 

  cout << Isis::Message::MissingDelimiter (')',"12345678901234567890")
       <<endl; 

  cout << Isis::Message::MissingDelimiter (')',"abcdefghijklmnopqrstuvwxyz")
       <<endl; 

  cout << endl;

//***************************************************************************

  cout << Isis::Message::FileOpen ("test.dat") << endl;
  cout << Isis::Message::FileCreate ("test.dat") << endl;
  cout << Isis::Message::FileRead ("test.dat") << endl;
  cout << Isis::Message::FileWrite ("test.dat") << endl;

  cout << endl;
}
