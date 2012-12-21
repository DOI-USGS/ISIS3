#include <fstream>

#include <QFile>

#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "FileName.h"
#include "IException.h"
#include "Constants.h"
#include "PvlFormatPds.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);

  try {

    PvlFormatPds *pdsFormatter;

    // Create a temp file for the keyword to type map
    {
      FileName fname = FileName::createTempFile("tempPvlFormatPDSunitTest_.tmp");
      QString pdsFile = fname.expanded();

      ofstream out;
      out.open(pdsFile.toAscii().data(), std::ios::out);

      {
        PvlKeyword key("skey", "string");
        out << key << endl;
      }
      {
        PvlKeyword key("ikey", "integer");
        out << key << endl;
      }
      {
        PvlKeyword key("fkey2", "rEaL");
        key.AddValue(toString(2));
        out << key << endl;
      }
      {
        PvlKeyword key("bkey", "bool");
        out << key << endl;
      }
      {
        PvlKeyword key("fkey0", "real");
        key.AddValue(toString(0));
        out << key << endl;
      }
      {
        PvlKeyword key("fkey", "real");
        out << key << endl;
      }
      {
        PvlKeyword key("ekey", "enum");
        out << key << endl;
      }
      {
        PvlKeyword key("hkey0", "hEX");
        out << key << endl;
      }
      {
        PvlKeyword key("hkey2", "hEX");
        key.AddValue(toString(2));
        out << key << endl;
      }
      {
        PvlKeyword key("hkey4", "hEX");
        key.AddValue(toString(4));
        out << key << endl;
      }
      {
        PvlKeyword key("binkey", "binary");
        key.AddValue(toString(7));
        out << key << endl;
      }
      {
        PvlKeyword key("binkey16", "binary");
        key.AddValue(toString(16));
        out << key << endl;
      }
      {
        PvlKeyword key("intkeyarray", "integer");
        out << key << endl;
      }
      {
        PvlKeyword key("dblkeyarray", "rEaL");
        key.AddValue(toString(2));
        out << key << endl;
      }
      {
        PvlKeyword key("wrapword", "string");
        out << key << endl;
      }
      {
        PvlKeyword key("array", "integer");
        out << key << endl;
      }

      out.close();

      pdsFormatter = new PvlFormatPds(pdsFile);
      QFile::remove(pdsFile);
    }

    // Test Keywords
    {
      PvlKeyword key("skey", "somestringval");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("skey", "string val", "chars");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("sNAstring", "N/A");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("sUNKquote", "\"UNK\"");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("ssinglequote", "\'NA\'");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("notinmap", "junk string");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("myint", "12345");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("myfloat", toString(-12345.67e+89), "degrees");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey", toString(-12345.6789));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey0", toString(-9876.543));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey0", toString(-9876.543e-99));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey2", toString(0.123456));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey2", toString(0.123456), "goofys");
      key.AddValue(toString(987.123), "goofys");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("fkey2", toString(0.123456), "goofys");
      key.AddValue(toString(987.123));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("ekey", "unsigned");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("myarray", "(12345,\"a short string\",1.234)");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("hkey0", toString((BigInt)0x123456789abcdeffLL));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("hkey2", toString(0x7a8b));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("hkey4", toString(0x1a2b3c4d));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("binkey", toString(0xA));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("binkey16", toString(0xffff));
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("intkeyarray", toString(1));
      key.AddValue("NULL");
      key.AddValue("3");
      key.AddValue("NULL");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("intkeyarray", toString(1), "m");
      key.AddValue("NULL", "m");
      key.AddValue("3", "m");
      key.AddValue("N/A");
      key.AddValue("UNK");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }

    {
      PvlKeyword key("dblkeyarray", toString(1.01));
      key.AddValue("NULL");
      key.AddValue("3.4");
      key.AddValue("UNK");
      cout << key << endl;
      key.SetFormat(pdsFormatter);
      cout << key << pdsFormatter->FormatEOL();
    }


    // Test Groups
    {
      PvlGroup grp("Group1");
      grp += PvlKeyword("skey", "stringval");
      grp += PvlKeyword("mystring", "string val");
      grp += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      grp += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      grp += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      grp += PvlKeyword("myint", toString(12345));
      grp += PvlKeyword("myfloat", toString(12345.67e+89));
      grp += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      cout << "=============================== Before" << endl;
      cout << grp << endl;
      grp.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << grp << pdsFormatter->FormatEOL();
    }


    // Test Objects
    {
      PvlGroup grp("Group1");
      grp += PvlKeyword("skey", "stringval");
      grp += PvlKeyword("mystring", "string val");
      grp += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      grp += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      grp += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      grp += PvlKeyword("myint", toString(12345));
      grp += PvlKeyword("myfloat", toString(12345.67e+89));
      grp += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      PvlObject obj("Object1");
      obj.AddGroup(grp);

      PvlObject obj2("Object2");
      obj2 += PvlKeyword("skey", "stringval");
      obj2 += PvlKeyword("mystring", "string val");
      obj2 += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      obj2 += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      obj2 += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      obj2 += PvlKeyword("myint", toString(12345));
      obj2 += PvlKeyword("myfloat", toString(12345.67e+89));
      obj2 += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      obj.AddObject(obj2);

      obj += PvlKeyword("skey", "stringval");
      obj += PvlKeyword("mystring", "string val");
      obj += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      obj += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      obj += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      obj += PvlKeyword("myint", toString(12345));
      obj += PvlKeyword("myfloat", toString(12345.67e+89));
      obj += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");


      cout << "=============================== Before" << endl;
      cout << obj << endl;
      obj.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << obj << pdsFormatter->FormatEOL();
    }


    // Test Pvl
    {
      Pvl pvl;

      PvlObject obj("Object1");

      PvlGroup grp("Group1");
      grp += PvlKeyword("skey", "stringval");
      grp += PvlKeyword("mystring", "string val");
      grp += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      grp += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      grp += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      grp += PvlKeyword("myint", toString(12345));
      grp += PvlKeyword("myfloat", toString(12345.67e+89));
      grp += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      obj.AddGroup(grp);

      PvlObject obj2("Object2");
      obj2 += PvlKeyword("skey", "stringval");
      obj2 += PvlKeyword("mystring", "string val");
      obj2 += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      obj2 += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      obj2 += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      obj2 += PvlKeyword("myint", toString(12345));
      obj2 += PvlKeyword("myfloat", toString(12345.67e+89));
      obj2 += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      obj2 += PvlKeyword("binkey16", toString(0x01f0));
      obj.AddObject(obj2);

      obj += PvlKeyword("skey", "stringval");
      obj += PvlKeyword("mystring", "string val");
      obj += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      obj += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      obj += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      obj += PvlKeyword("myint", toString(12345));
      obj += PvlKeyword("myfloat", toString(12345.67e+89));
      obj += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");

      pvl += PvlKeyword("skey", "stringval");
      pvl += PvlKeyword("mystring", "string val");
      pvl += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      pvl += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      pvl += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      pvl += PvlKeyword("myint", toString(12345));
      pvl += PvlKeyword("myfloat", toString(12345.67e+89));
      pvl += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");

      pvl.AddObject(obj);

      PvlGroup grp2("Group2");
      grp2 += PvlKeyword("skey", "stringval");
      grp2 += PvlKeyword("mystring", "string val");
      grp2 += PvlKeyword("sNULLstring", "NULL");   // should add quotes after format set
      grp2 += PvlKeyword("sUNKquote", "\"UNK\"");   // should not add more quotes
      grp2 += PvlKeyword("sNAsingle", "\'N/A\'");   // should not add more quotes
      grp2 += PvlKeyword("myint", toString(12345));
      grp2 += PvlKeyword("myfloat", toString(12345.67e+89));
      grp2 += PvlKeyword("myarray", "(12345,\"a short string\",1.234)");
      grp2 += PvlKeyword("binkey16", toString(0x8001));
      grp2 += PvlKeyword("wrapword", "The quick brown fox jumped over the lazy duck. "
                               "Repunzel Repunzel let down your hair. The little toy dog is covered with dust,"
                               " but sturdy and staunch he stands; and the little toy soldier is red with rust.");
      PvlKeyword key(PvlKeyword("array", toString(12345)));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      key.AddValue(toString(67890));
      key.AddValue(toString(12345));
      grp2 += key;
      pvl.AddGroup(grp2);


      cout << "=============================== Before" << endl;
      cout << pvl << endl;
      pvl.SetFormat(pdsFormatter);
      cout << "=============================== After" << endl;
      cout << pvl << pdsFormatter->FormatEOL();
    }


  }
  catch(IException &e) {
    e.print();
  }


}

