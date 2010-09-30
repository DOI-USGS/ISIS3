#include "PvlKeyword.h"
#include "iException.h"
#include "PvlSequence.h"
#include "Preference.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  string keywordsToTry[] = {
    "KEYWORD",
    "KEYWORD X",
    "KEYWORD =",
    "KEYWORD = SOME_VAL",
    "KEYWORD = \"  val  \"",
    "KEYWORD = \" 'val' \"",
    "KEYWORD = (VAL",
    "KEYWORD = (VAL1,VAL2",
    "KEYWORD = (A B,C,D)",
    "KEYWORD = ((A B),(C),(D",
    "KEYWORD = (SOME_VAL)",
    "KEYWORD = (SOME_VAL) <a>",
    "KEYWORD=(SOME_VAL)<a>",
    "KEYWORD = (A, )",
    "KEYWORD = ()",
    "KEYWORD = (A,B)",
    "KEYWORD = {A, B}",
    "KEYWORD = (A,B) #comment this",
    "KEYWORD = ( A , B )",
    "KEYWORD\t=\t( A\t,\tB )",
    "KEYWORD = (A, B,C,D,E))",
    "KEYWORD = ((1, 2), {3,  4}, (5), 6)",
    "KEYWORD = { \"VAL1\" ,   \"VAL2\", \"VAL3\"}",
    "KEYWORD = { \"VAL1\" , \"VAL2\", \"VAL3\")",
    "KEYWORD = { \"VAL1\" ,",
    "KEYWORD = \"(A,B,\"",
    "KEYWORD = ',E)'",
    "KEYWORD = (A <a>, B <b>, C, D <d>)",
    "KEYWORD = (A <a>, B <b>, C, D <d>) <e>",
    "KEYWORD = ',E) <unit>",
    "KEYWORD = ,E) <unit>",
    "#SOMECOMMENT\nKEYWORD = SOME_VAL",
    "#SOMECOMMENT1\n#SOMECOMMENT2\nKEYWORD = SOME_VAL",
    "//SOMECOMMENT1\n#SOMECOMMENT2\nKEYWORD = SOME_VAL",
    "/*SOMECOMMENT1*/\nKEYWORD = SOME_VAL",
    "KEYWORD = '/*\n*/'",
    "/* SOMECOMMENT1\n  SOMECOMMENT2\nSOMECOMMENT3 */\nKEYWORD = SOME_VAL",
    "/*C1\n\nA\n/*\nC3*/\nKEYWORD = SOME_VAL",
    "/*C1\n/**/\nKEYWORD = SOME_VAL",
    "/*C1\nA/**/\nKEYWORD = SOME_VAL",
    "/*           A            */\n/* B *//*C*/\nKEYWORD = SOME_VAL",
    "/*C1/**/\nKEYWORD = SOME_VAL",
    "/*C1   \n\nA\n\nC3*//*Neato*//*Man*/KEYWORD = (A,B,C) /*Right?\nYes!*/"
  };

  cout << endl << endl;
  cout << "----- Testing Basic Read/Write -----" << endl;
  PvlKeyword keyword;
  for(unsigned int key = 0;
      key < sizeof(keywordsToTry) / sizeof(string);
      key++) {
    string keywordCpy = keywordsToTry[key];
    while(keywordCpy.find("\n") != string::npos) {
      cout << keywordCpy.substr(0, keywordCpy.find("\n") + 1);
      keywordCpy = keywordCpy.substr(keywordCpy.find("\n") + 1);
    }

    cout << "'" << keywordCpy << "' ";

    for(int pad = keywordCpy.size(); pad < 30; pad++) {
      cout << "-";
    }

    cout << "> ";

    vector< string > keywordComments;
    string keywordName;
    vector< pair<string, string> > keywordValues;

    bool result = false;

    try {
      result = keyword.ReadCleanKeyword(keywordsToTry[key],
                                        keywordComments,
                                        keywordName,
                                        keywordValues);

      if(result) cout << "VALID" << endl;
      else cout << "INCOMPLETE" << endl;
    }
    catch(iException &e) {
      cout << "INVALID" << endl;
      cout << "    ";
      cout.flush();
      e.Report(false);
    }

    if(result) {
      for(unsigned int comment = 0; comment < keywordComments.size(); comment++)
        cout << "    COMMENT: " << keywordComments[comment] << endl;

      cout << "    NAME: " << keywordName << endl;

      for(unsigned int value = 0; value < keywordValues.size(); value++) {
        cout << "    VALUE: " << keywordValues[value].first;

        if(!keywordValues[value].second.empty()) {
          cout << " <" << keywordValues[value].second << ">";
        }

        cout << endl;
      }
    }

  }

  cout << endl << endl;
  cout << "----- Testing Stream Read/Write -----" << endl;
  for(unsigned int key = 0;
      key < sizeof(keywordsToTry) / sizeof(string);
      key ++) {
    stringstream stream;
    PvlKeyword someKey;

    cout << "Input:\n" << keywordsToTry[key] << endl;

    cout << endl << "Output: " << endl;
    try {
      stream.write(keywordsToTry[key].c_str(), keywordsToTry[key].size());
      stream >> someKey;
      cout << someKey << endl;
    }
    catch(iException &e) {
      e.Report(false);
    }

    cout << endl;
  }

  cout << "----- Testing Difficult Cases Read/Write -----\n";


  try {

    const Isis::PvlKeyword keyN("THE_INTERNET",
                                "Seven thousand eight hundred forty three million seventy four nine seventy six forty two eighty nine sixty seven thirty five million jillion bajillion google six nine four one two three four five six seven eight nine ten eleven twelve thirteen fourteen",
                                "terrabytes");
    PvlKeyword keyNRead;
    stringstream streamN;
    streamN << keyN;
    streamN >> keyNRead;
    cout << keyNRead << endl;

    const Isis::PvlKeyword keyZ("BIG_HUGE_LONG_NAME_THAT_SHOULD_TEST_OUT_PARSING",
                                "Seven thousand eight hundred forty three million seventy four",
                                "bubble baths");
    PvlKeyword keyZRead;
    stringstream streamZ;
    streamZ << keyZ;
    streamZ >> keyZRead;
    cout << keyZRead << endl;

    Isis::PvlKeyword keyU("ARRAY_TEST", 5.87, "lightyears");
    keyU.AddValue(5465.6, "lightyears");
    keyU.AddValue(574.6, "lightyears");

    PvlKeyword keyURead;
    stringstream streamU;
    streamU << keyU;
    streamU >> keyURead;
    cout << keyURead << endl;

    const Isis::PvlKeyword keyV("FIRST_100_DIGITS_OF_PI", "3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679");
    PvlKeyword keyVRead;
    stringstream streamV;
    streamV << keyV;
    streamV >> keyVRead;
    cout << keyVRead << endl;
    cout << "Raw Data -->" << endl;
    cout << keyVRead[0] << endl << endl;


    const Isis::PvlKeyword keyJ("A", "XXXXXXXXXXxxxxxxxxxxXXXXXXXXXXxxxxxxxxxxXXXXXXXXXXxxxxxxxxxxXXXXXXXXXXxxxx");
    PvlKeyword keyJRead;
    stringstream streamJ;
    streamJ << keyJ;
    streamJ >> keyJRead;
    cout << keyJRead << endl;

    string keyB = "TREE = {   \"MAPLE\"   ,\n \"ELM\" \n, \"PINE\"   }";
    PvlKeyword keyBRead;
    stringstream streamB;
    streamB << keyB;
    streamB >> keyBRead;
    cout << keyBRead << endl;

    Isis::PvlKeyword keyW("UGHHHHHHHHHHHH");
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;
    keyW += 59999.0;

    PvlKeyword keyWRead;
    stringstream streamW;
    streamW << keyW;
    streamW >> keyWRead;
    cout << keyWRead << endl;

    const Isis::PvlKeyword key("NAME", 5.2, "meters");

    cout << key << endl;

    Isis::PvlKeyword key2("KEY");
    cout << key2 << endl;

    key2 += 5;
    key2 += string("");
    key2.AddValue(3.3, "feet");
    key2.AddValue("Hello World!");
    string str = "Hello World! This is a really really long comment that needs to"
                 " be wrapped onto several different lines to make the PVL file "
                 "look really pretty!";
    key2.AddCommentWrapped(str);
    cout << key2 << endl;

    cout << key2[1] << endl;
    key2[1] = 88;
    cout << key2 << endl;

    Isis::PvlSequence seq;
    cout.flush();
    seq += "(a,b,c)";
    cout.flush();
    seq += "(\"Hubba Hubba\",\"Bubba\")";
    cout.flush();
    Isis::PvlKeyword k("key");
    k = seq;
    cout << k << endl;

    // Test SetUnits methods
    k = Isis::PvlKeyword("k", "radius", "meters");
    k.AddValue("circumference", "meters");
    cout << "\n\n"
         << "Test SetUnits methods:\n\n"
         << "  original condition of Keyword k :\n"
         << "    " << k << "\n\n"
         << "  after k.SetUnits(\"circumference\", \"Fathoms\") :\n";
    k.SetUnits("circumference", "Fathoms");
    cout << "    " << k << "\n\n"
         << "  after k.SetUnits(\"TeraFathoms\") :\n";
    k.SetUnits("TeraFathoms");
    cout << "    " << k << "\n\n\n";

    //Test casting operators
    cout << "----------------------------------------" << endl;
    cout << "Testing cast operators" << endl;
    Isis::PvlKeyword cast01("cast1", "I'm being casted");
    Isis::PvlKeyword cast02("cast2", "465721");
    Isis::PvlKeyword cast03("cast3", "131.2435");
    cout << "string     = " << (string)cast01 << endl;
    cout << "int     = " << (int)cast02 << endl;
    cout << "BigInt     = " << (Isis::BigInt)cast02 << endl;
    cout << "double     = " << (double)cast03 << endl;

  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
  catch(...) {
    cout << "Unknown error" << endl;
  }

  try {
    Isis::PvlKeyword key(" Test_key_2 ", "Might work");
    cout << key << endl;
    Isis::PvlKeyword key2("Bob is a name", "Yes it is");
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }


  try {
    Isis::PvlKeyword key(" Test_key_3 ", "Might'not work");
    cerr << key << endl;
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
  
  // Test Validation of PvlKeywords
  // Validate type integer
  try {
    // Template Keyword
    PvlKeyword pvlTmplKwrd("KeyName", "integer");
    PvlKeyword pvlKwrd("KeyName", 3);
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
    pvlKwrd.Clear();

    pvlKwrd=PvlKeyword("KeyName", "null");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
    pvlKwrd.Clear();
    
    pvlKwrd=PvlKeyword("KeyName", 3.5);
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
  } 
  catch(Isis::iException &e) {
   cerr << "Invalid Keyword Type: Integer Expected" << endl;
  }
  
  // Validate String
  try {
    PvlKeyword pvlTmplKwrd("KeyName", "string");
    pvlTmplKwrd.AddValue("value1");
    pvlTmplKwrd.AddValue("value2");
    pvlTmplKwrd.AddValue("value3");
    PvlKeyword pvlKwrd("KeyName", "VALUe3");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
    pvlKwrd.Clear();

    pvlKwrd=PvlKeyword("KeyName", "value");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
  } 
  catch(Isis::iException &e) {
    cerr << "Invalid Keyword Value: Expected values \"value1\", \"value2\", \"value3\"" << endl;
  }
  
  // Validate Boolean
  try {
    PvlKeyword pvlTmplKwrd("KeyName", "boolean");
    PvlKeyword pvlKwrd("KeyName", "true");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
    pvlKwrd.Clear();

    pvlKwrd=PvlKeyword("KeyName", "null");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
    pvlKwrd.Clear();
    
    pvlKwrd=PvlKeyword("KeyName", "value");
    pvlTmplKwrd.ValidateKeyword(pvlKwrd);
  } 
  catch(Isis::iException &e) {
    cerr << "Invalid Keyword Type: Expected  Boolean values \"true\", \"false\", \"null\"" << endl;
  }
}
