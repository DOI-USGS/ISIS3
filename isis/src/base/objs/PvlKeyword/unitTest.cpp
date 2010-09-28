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
    "/*SOMECOMMENT1\nKEYWORD = SOME_VAL",
    "KEYWORD = '/*\n*/'",
    "/* SOMECOMMENT1\n  SOMECOMMENT2\nSOMECOMMENT3 */\nKEYWORD = SOME_VAL",
    "/*C1\n\nA\n/*\nC3*/\nKEYWORD = SOME_VAL",
    "/*C1\n/**/\nKEYWORD = SOME_VAL",
    "/*C1\nA/**/\nKEYWORD = SOME_VAL",
    "/*           A            */\n/* B *//*C*/\nKEYWORD = SOME_VAL",
    "/*C1/**/\nKEYWORD = SOME_VAL",
    "/*C1   \n\nA\n\nC3*//*Neato*//*Man*/KEYWORD = (A,B,C) /*Right?\nYes!*/"
  };

  cerr << endl << endl;
  cerr << "----- Testing Basic Read/Write -----" << endl;
  PvlKeyword keyword;
  for(unsigned int key = 0;
      key < sizeof(keywordsToTry) / sizeof(string);
      key++) {
    string keywordCpy = keywordsToTry[key];
    while(keywordCpy.find("\n") != string::npos) {
      cerr << keywordCpy.substr(0, keywordCpy.find("\n") + 1);
      keywordCpy = keywordCpy.substr(keywordCpy.find("\n") + 1);
    }

    cerr << "'" << keywordCpy << "' ";

    for(int pad = keywordCpy.size(); pad < 30; pad++) {
      cerr << "-";
    }

    cerr << "> ";

    vector< string > keywordComments;
    string keywordName;
    vector< pair<string, string> > keywordValues;

    bool result = false;

    try {
      result = keyword.ReadCleanKeyword(keywordsToTry[key],
                                        keywordComments,
                                        keywordName,
                                        keywordValues);

      if(result) cerr << "VALID" << endl;
      else cerr << "INCOMPLETE" << endl;
    }
    catch(iException &e) {
      cerr << "INVALID" << endl;
      cerr << "    ";
      cerr .flush();
      e.Report(false);
    }

    if(result) {
      for(unsigned int comment = 0; comment < keywordComments.size(); comment++)
        cerr << "    COMMENT: " << keywordComments[comment] << endl;

      cerr << "    NAME: " << keywordName << endl;

      for(unsigned int value = 0; value < keywordValues.size(); value++) {
        cerr << "    VALUE: " << keywordValues[value].first;

        if(!keywordValues[value].second.empty()) {
          cerr << " <" << keywordValues[value].second << ">";
        }

        cerr << endl;
      }
    }

  }

  cerr << endl << endl;
  cerr << "----- Testing Stream Read/Write -----" << endl;
  for(unsigned int key = 0;
      key < sizeof(keywordsToTry) / sizeof(string);
      key ++) {
    stringstream stream;
    PvlKeyword someKey;

    cerr << "Input:\n" << keywordsToTry[key] << endl;

    cerr << endl << "Output: " << endl;
    try {
      stream.write(keywordsToTry[key].c_str(), keywordsToTry[key].size());
      stream >> someKey;
      cerr << someKey << endl;
    }
    catch(iException &e) {
      e.Report(false);
    }

    cerr << endl;
  }

  cerr << "----- Testing Difficult Cases Read/Write -----\n";


  try {

    const Isis::PvlKeyword keyN("THE_INTERNET",
                                "Seven thousand eight hundred forty three "
                                "million seventy four nine seventy six forty "
                                "two eighty nine sixty seven thirty five "
                                "million jillion bajillion google six nine "
                                "four one two three four five six seven eight "
                                "nine ten eleven twelve thirteen fourteen",
                                "terrabytes");
    PvlKeyword keyNRead;
    stringstream streamN;
    streamN << keyN;
    streamN >> keyNRead;
    cerr << keyNRead << endl;

    const Isis::PvlKeyword keyZ("BIG_HUGE_LONG_NAME_THAT_SHOULD"
                                "_TEST_OUT_PARSING",
                                "Seven thousand eight hundred forty three "
                                "million seventy four",
                                "bubble baths");
    PvlKeyword keyZRead;
    stringstream streamZ;
    streamZ << keyZ;
    streamZ >> keyZRead;
    cerr << keyZRead << endl;

    Isis::PvlKeyword keyU("ARRAY_TEST", 5.87, "lightyears");
    keyU.AddValue(5465.6, "lightyears");
    keyU.AddValue(574.6, "lightyears");

    PvlKeyword keyURead;
    stringstream streamU;
    streamU << keyU;
    streamU >> keyURead;
    cerr << keyURead << endl;

    const Isis::PvlKeyword keyV("FIRST_100_DIGITS_OF_PI", "3.141592653589793238"
      "462643383279502884197169399375105820974944592307816406286208998628034825"
      "3421170679");
    PvlKeyword keyVRead;
    stringstream streamV;
    streamV << keyV;
    streamV >> keyVRead;
    cerr << keyVRead << endl;
    cerr << "Raw Data -->" << endl;
    cerr << keyVRead[0] << endl << endl;


    const Isis::PvlKeyword keyJ("A", "XXXXXXXXXXxxxxxxxxxxXXXXXXXXXXxxxxxxxxxx"
      "XXXXXXXXXXxxxxxxxxxxXXXXXXXXXXxxxx");
    PvlKeyword keyJRead;
    stringstream streamJ;
    streamJ << keyJ;
    streamJ >> keyJRead;
    cerr << keyJRead << endl;

    string keyB = "TREE = {   \"MAPLE\"   ,\n \"ELM\" \n, \"PINE\"   }";
    PvlKeyword keyBRead;
    stringstream streamB;
    streamB << keyB;
    streamB >> keyBRead;
    cerr << keyBRead << endl;

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
    cerr << keyWRead << endl;

    const Isis::PvlKeyword key("NAME", 5.2, "meters");

    cerr << key << endl;

    Isis::PvlKeyword key2("KEY");
    cerr << key2 << endl;

    key2 += 5;
    key2 += string("");
    key2.AddValue(3.3, "feet");
    key2.AddValue("Hello World!");
    string str = "Hello World! This is a really really long comment that needs to"
                 " be wrapped onto several different lines to make the PVL file "
                 "look really pretty!";
    key2.AddCommentWrapped(str);
    cerr << key2 << endl;

    cerr << key2[1] << endl;
    key2[1] = 88;
    cerr << key2 << endl;

    Isis::PvlSequence seq;
    cerr .flush();
    seq += "(a,b,c)";
    cerr .flush();
    seq += "(\"Hubba Hubba\",\"Bubba\")";
    cerr .flush();
    Isis::PvlKeyword k("key");
    k = seq;
    cerr << k << endl;

    // Test SetUnits methods
    k = Isis::PvlKeyword("k", "radius", "meters");
    k.AddValue("circumference", "meters");
    cerr << "\n\n"
         << "Test SetUnits methods:\n\n"
         << "  original condition of Keyword k :\n"
         << "    " << k << "\n\n"
         << "  after k.SetUnits(\"circumference\", \"Fathoms\") :\n";
    k.SetUnits("circumference", "Fathoms");
    cerr << "    " << k << "\n\n"
         << "  after k.SetUnits(\"TeraFathoms\") :\n";
    k.SetUnits("TeraFathoms");
    cerr << "    " << k << "\n\n\n";

    //Test casting operators
    cerr << "----------------------------------------" << endl;
    cerr << "Testing cast operators" << endl;
    Isis::PvlKeyword cast01("cast1", "I'm being casted");
    Isis::PvlKeyword cast02("cast2", "465721");
    Isis::PvlKeyword cast03("cast3", "131.2435");
    cerr << "string     = " << (string)cast01 << endl;
    cerr << "int     = " << (int)cast02 << endl;
    cerr << "BigInt     = " << (Isis::BigInt)cast02 << endl;
    cerr << "double     = " << (double)cast03 << endl;

  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
  catch(...) {
    cerr << "Unknown error" << endl;
  }

  try {
    Isis::PvlKeyword key(" Test_key_2 ", "Might work");
    cerr << key << endl;
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
    e.Report(false);
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
    cerr << "Invalid Keyword Value: Expected values \"value1\", \"value2\", "
            "\"value3\"" << endl;
    e.Report(false);
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
    cerr << "Invalid Keyword Type: Expected  Boolean values "
            "\"true\", \"false\", \"null\"" << endl;
    e.Report(false);
  }

  try {
    stringstream s;
    s.str("/* This is a comment");
    Pvl testPvl;
    s >> testPvl;
  }
  catch(iException &e) {
    e.Report(false);
  }
}
