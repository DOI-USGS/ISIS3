#include <fstream>

#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "Filename.h"
#include "iException.h"
#include "Constants.h"
#include "PvlFormatPds.h"
#include "Preference.h"

using namespace std;
int main () {
  Isis::Preference::Preferences(true);

  try {

    Isis::PvlFormatPds *pdsFormatter;
  
    // Create a temp file for the keyword to type map
    {
      Isis::Filename fname;
      fname.Temporary("tempPvlFormatPDSunitTest_", "map");
      std::string pdsFile = fname.Expanded();

      ofstream out;
      out.open(pdsFile.c_str(), std::ios::out);
  
      {Isis::PvlKeyword key("skey", "string"); out<<key<<endl;}
      {Isis::PvlKeyword key("ikey", "integer"); out<<key<<endl;}
      {Isis::PvlKeyword key("fkey2", "rEaL"); key.AddValue(2); out<<key<<endl;}
      {Isis::PvlKeyword key("bkey", "bool"); out<<key<<endl;}
      {Isis::PvlKeyword key("fkey0", "real"); key.AddValue(0); out<<key<<endl;}
      {Isis::PvlKeyword key("fkey", "real"); out<<key<<endl;}
      {Isis::PvlKeyword key("ekey", "enum"); out<<key<<endl;}
      {Isis::PvlKeyword key("hkey0", "hEX"); out<<key<<endl;}
      {Isis::PvlKeyword key("hkey2", "hEX"); key.AddValue(2); out<<key<<endl;}
      {Isis::PvlKeyword key("hkey4", "hEX"); key.AddValue(4); out<<key<<endl;}
      {Isis::PvlKeyword key("binkey", "binary"); key.AddValue(7); out<<key<<endl;}
      {Isis::PvlKeyword key("binkey16", "binary"); key.AddValue(16); out<<key<<endl;}
      {Isis::PvlKeyword key("intkeyarray", "integer"); out<<key<<endl;}
      {Isis::PvlKeyword key("dblkeyarray", "rEaL"); key.AddValue(2); out<<key<<endl;}
      {Isis::PvlKeyword key("wrapword", "string"); out<<key<<endl;}
      {Isis::PvlKeyword key("array", "integer"); out<<key<<endl;}

      out.close();
  
      pdsFormatter = new Isis::PvlFormatPds(pdsFile);
      remove(pdsFile.c_str());
    }
  
    // Test Keywords
    {
      Isis::PvlKeyword key("skey", "somestringval");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("skey", "string val","chars");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }
  
    {
      Isis::PvlKeyword key("sNAstring", "N/A");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("sUNKquote", "\"UNK\"");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("ssinglequote", "\'NA\'");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("notinmap", "junk string");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }
  
    {
      Isis::PvlKeyword key("myint", 12345);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }
  
    {
      Isis::PvlKeyword key("myfloat", -12345.67e+89,"degrees");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey", -12345.6789);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey0", -9876.543);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey0", -9876.543e-99);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey2", 0.123456);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey2", 0.123456, "goofys");
      key.AddValue(987.123, "goofys");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("fkey2", 0.123456, "goofys");
      key.AddValue(987.123);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("ekey", "unsigned");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("myarray", "(12345,\"a short string\",1.234)");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("hkey0", (Isis::BigInt)0x123456789abcdeffLL);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }
  
    {
      Isis::PvlKeyword key("hkey2", 0x7a8b);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("hkey4", 0x1a2b3c4d);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("binkey", 0xA);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("binkey16", 0xffff);
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("intkeyarray", 1);
      key.AddValue("NULL");
      key.AddValue("3");
      key.AddValue("NULL");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("intkeyarray", 1,"m");
      key.AddValue("NULL","m");
      key.AddValue("3","m");
      key.AddValue("N/A");
      key.AddValue("UNK");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      Isis::PvlKeyword key("dblkeyarray", 1.01);
      key.AddValue("NULL");
      key.AddValue("3.4");
      key.AddValue("UNK");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

  
    // Test Groups
    {
      Isis::PvlGroup grp("Group1");
      grp += Isis::PvlKeyword ("skey", "stringval");
      grp += Isis::PvlKeyword ("mystring", "string val");
      grp += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      grp += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      grp += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      grp += Isis::PvlKeyword ("myint", 12345);
      grp += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      grp += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      cout << "=============================== Before" << endl;
      cout << grp << endl;
      grp.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << grp << pdsFormatter->FormatEOL();
    }
  
  
    // Test Objects
    {
      Isis::PvlGroup grp("Group1");
      grp += Isis::PvlKeyword ("skey", "stringval");
      grp += Isis::PvlKeyword ("mystring", "string val");
      grp += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      grp += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      grp += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      grp += Isis::PvlKeyword ("myint", 12345);
      grp += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      grp += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      Isis::PvlObject obj("Object1");
      obj.AddGroup(grp);
  
      Isis::PvlObject obj2("Object2");
      obj2 += Isis::PvlKeyword ("skey", "stringval");
      obj2 += Isis::PvlKeyword ("mystring", "string val");
      obj2 += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      obj2 += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      obj2 += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      obj2 += Isis::PvlKeyword ("myint", 12345);
      obj2 += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      obj2 += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      obj.AddObject(obj2);
  
      obj += Isis::PvlKeyword ("skey", "stringval");
      obj += Isis::PvlKeyword ("mystring", "string val");
      obj += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      obj += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      obj += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      obj += Isis::PvlKeyword ("myint", 12345);
      obj += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      obj += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
  
  
      cout << "=============================== Before" << endl;
      cout << obj << endl;
      obj.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << obj << pdsFormatter->FormatEOL();
    }
  
  
    // Test Pvl
    {
      Isis::Pvl pvl;
  
      Isis::PvlObject obj("Object1");
  
      Isis::PvlGroup grp("Group1");
      grp += Isis::PvlKeyword ("skey", "stringval");
      grp += Isis::PvlKeyword ("mystring", "string val");
      grp += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      grp += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      grp += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      grp += Isis::PvlKeyword ("myint", 12345);
      grp += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      grp += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      obj.AddGroup(grp);
  
      Isis::PvlObject obj2("Object2");
      obj2 += Isis::PvlKeyword ("skey", "stringval");
      obj2 += Isis::PvlKeyword ("mystring", "string val");
      obj2 += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      obj2 += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      obj2 += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      obj2 += Isis::PvlKeyword ("myint", 12345);
      obj2 += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      obj2 += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      obj2 += Isis::PvlKeyword ("binkey16", 0x01f0);
      obj.AddObject(obj2);
  
      obj += Isis::PvlKeyword ("skey", "stringval");
      obj += Isis::PvlKeyword ("mystring", "string val");
      obj += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      obj += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      obj += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      obj += Isis::PvlKeyword ("myint", 12345);
      obj += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      obj += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
  
      pvl += Isis::PvlKeyword ("skey", "stringval");
      pvl += Isis::PvlKeyword ("mystring", "string val");
      pvl += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      pvl += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      pvl += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      pvl += Isis::PvlKeyword ("myint", 12345);
      pvl += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      pvl += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
  
      pvl.AddObject(obj);
  
      Isis::PvlGroup grp2("Group2");
      grp2 += Isis::PvlKeyword ("skey", "stringval");
      grp2 += Isis::PvlKeyword ("mystring", "string val");
      grp2 += Isis::PvlKeyword ("sNULLstring", "NULL");  // should add quotes after format set
      grp2 += Isis::PvlKeyword ("sUNKquote", "\"UNK\"");  // should not add more quotes
      grp2 += Isis::PvlKeyword ("sNAsingle", "\'N/A\'");  // should not add more quotes
      grp2 += Isis::PvlKeyword ("myint", 12345);
      grp2 += Isis::PvlKeyword ("myfloat", 12345.67e+89);
      grp2 += Isis::PvlKeyword ("myarray", "(12345,\"a short string\",1.234)");
      grp2 += Isis::PvlKeyword ("binkey16", 0x8001);
      grp2 += Isis::PvlKeyword ("wrapword", "The quick brown fox jumped over the lazy duck. "
                                "Repunzel Repunzel let down your hair. The little toy dog is covered with dust,"
                                " but sturdy and staunch he stands; and the little toy soldier is red with rust.");
      Isis::PvlKeyword key( Isis::PvlKeyword ("array", 12345) );
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      key.AddValue(67890);
      key.AddValue(12345);
      grp2 += key;
      pvl.AddGroup(grp2);
  
  
      cout << "=============================== Before" << endl;
      cout << pvl << endl;
      pvl.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << pvl << pdsFormatter->FormatEOL();
    }


  }
  catch (Isis::iException &e) {
    e.Report(false);
  }


}

