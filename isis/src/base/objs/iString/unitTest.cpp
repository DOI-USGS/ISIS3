#include <iostream>
#include <sstream>
#include "iString.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

inline string yesOrNo(bool b) {
  if (b) return (string("Yes"));
  return (string("No"));
}

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  cout.precision(14);
  // Tests for non-static functions
  try {
    Isis::iString str0;
    str0 = "Test string";
    cout << "No arg construtor : " << str0 << endl;

    Isis::iString str1 = str0 + " again";
    cout << "Constructor Isis::iString: " << str1 << endl;

    Isis::iString str1_5 = 'A';
    cout << "Constructor char : " << str1_5 << endl;

    Isis::iString str2 = 999;
    cout << "Constructor int : " << str2 << endl;

    Isis::iString str2a = Isis::BigInt(9999999999LL);
    cout << "Constructor BigInt : " << str2a << endl;

    Isis::iString str3 = 999.999;
    cout << "Constructor double :" << str3 << endl;

    Isis::iString str4 = "ABCDefghijkBCAD";
    cout << "Before Trim : " << str4 << endl;
    cout << "Return Trim : " << str4.Trim ("ABCD") << endl;
    cout << "After Trim  : " << str4 << endl;

    Isis::iString str5 = "ABCDefghijkBCAD";
    cout << "Before TrimHead : " << str5 << endl;
    cout << "Return TrimHead : " << str5.TrimHead ("DBCA") << endl;
    cout << "After TrimHead  : " << str5 << endl;
    cout << "Middle test     : " << str5.TrimHead ("g") << endl;

    Isis::iString str6 = "ABCDefghijkBCAD";
    cout << "Before TrimTail : " << str6 << endl;
    cout << "Return TrimTail : " << str6.TrimTail ("DBCA") << endl;
    cout << "After TrimTail  : " << str6 << endl;
    cout << "Middle test     : " << str6.TrimTail ("f") << endl;

    std::string str35 = "ABCDefghijkBCAD";
    cout << "Before TrimHead : " << str35 << endl;
    str35 = Isis::iString::TrimHead("DBCA",str35);
    cout << "After TrimHead  : " << str35 << endl;
    str35 = Isis::iString::TrimHead("g",str35);
    cout << "Middle test     : " << str35 << endl;

    std::string str36 = "ABCDefghijkBCAD";
    cout << "Before TrimTail : " << str36 << endl;
    str36 = Isis::iString::TrimTail("DBCA",str36);
    cout << "After TrimTail  : " << str36 << endl;
    str36 = Isis::iString::TrimTail("f",str36);
    cout << "Middle test     : " << str36 << endl;

    Isis::iString str7 = "abcdefghijklmnopqrstuvwxyzABC!@#$%^&*()";
    cout << "Before Upcase : " << str7 << endl;
    cout << "After Upcase  : " << str7.UpCase () << endl;

    cout << "Before DownCase : " << str7 << endl;
    cout << "After DownCase  : " << str7.DownCase () << endl;

    Isis::iString str8 = 987654321;
    cout << "Integer : " << str8 << endl;
    cout << "Integer : " << str8.ToInteger () << endl;
    str8 = "987trew";
    try {
      str8.ToInteger ();
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }

    Isis::iString str8a = Isis::BigInt(9876543210LL);
    cout << "BigInteger : " << str8a << endl;
    cout << "BigInteger : " << str8a.ToBigInteger () << endl;
    str8a = "987trew";
    try {
      str8a.ToBigInteger ();
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }

    Isis::iString str9 = 9876.54321;
    cout << "Double : " << str9.c_str() << endl;
    cout << "Double : " << str9.ToDouble () << endl;
    str9 = "123$987";
    try {
      str9.ToDouble ();
    }
    catch (Isis::iException &error) {
      error.Report (false);
    }

    Isis::iString str10 = "123.0E45";
    cout << "Exponent : " << str10 << endl;
    cout << "Exponent : " << str10.ToDouble () << endl;

    Isis::iString str11 = "25:255 35:15";
    cout << str11 << endl;
    while (str11.length() > 0) {
      cout << str11.Token(": ") << endl;
    }

    str11 = "key1=tok1 key2=\"t o k 2\" key3=(1,2,3,4)";
    cout << str11 << endl;
    while (str11.length() > 0) {
      cout << str11.Token("= ") << endl;
    }

    str11 = "\"abcd\",\"1234\"";
    cout << str11 << endl;
    while (str11.length() > 0) {
      cout << str11.Token(",") << endl;
    }

    str11 = "\",1234\",\"ab,cd\"";
    cout << str11 << endl;
    while (str11.length() > 0) {
      cout << str11.Token(",") << endl;
    }

    str11 = "/this/is/a/long/filename.jnk,seperated/by/a/comma/ending/with/\r\n";
    cout << str11.Trim("\r\n") << endl;
    while (str11.length() > 0) {
      cout << str11.Token(",") << endl;
    }

    str11 = "/this/is/another/long/filename.jnk   seperated/by/3/spaces/file2.tmp";
    cout << str11.Trim("\r\n") << endl;
    int m = 1;
    while (str11.length() > 0) {
      cout << "token# " << m++ << ">> " << str11.Token(" ") << endl;
    }

    cout << endl;

    Isis::iString str12 = "  \"    \"    ";
    cout << "Before compress >" << str12 << "<" << endl;
    str12.Compress ();
    cout << "After compress  >" << str12 << "<" << endl;

    Isis::iString str12b = "|  \"    \"    \'    \'   |";
    cout << "Before compress >" << str12b << "<" << endl;
    str12b.Compress ();
    cout << "After compress  >" << str12b << "<" << endl;

    Isis::iString str13 = "  AB  CD  ";
    cout << "Before compress >" << str13 << "<" << endl;
    str13.Compress ();
    cout << "After compress  >" << str13 << "<" << endl;

    Isis::iString str14 = "  \"  \"    ";
    cout << "Before force compress >" << str14 << "<" << endl;
    cout << "After force compress  >" << str14.Compress (true) << "<" << endl;

    Isis::iString str15 = "  \"AB  CD\"  ";
    cout << "Before force compress >" << str15 << "<" << endl;
    cout << "After force compress  >" << str15.Compress (true) << "<" << endl;

    Isis::iString str16 = " ";
    cout << "Before force compress >" << str16 << "<" << endl;
    cout << "After force compress  >" << str16.Compress (true) << "<" << endl;

    Isis::iString str17 = "ABCDEFG";
    cout << "Before convert >" << str17 << "<" << endl;
    cout << "After convert  >" << str17.Convert("BDFG", '-') << "<" << endl;

    Isis::iString str18 = "Thirteen is bigger than fourteen";
    cout << "Before replace >" << str18 << "<" << endl;
    cout << "After replace  >" << str18.Replace("bigger", "smaller") << "<" << endl;

    Isis::iString str19 = "a\tb\b\n\v\f\r  c";
    // Don't cout the before string, it will cause all sorts of problems
    //    cout << "Before convert >" << str19 << "<" << endl;
    cout << "After convert white space >" << str19.ConvertWhiteSpace () << "<" << endl;

    Isis::iString str20 = "xxxXXX0";
    cout << "Before Upcase >" << str20 << "<" << endl;
    cout << "After Upcase  >" << str20.UpCase () << "<" << endl;

    cout << "Before DownCase >" << str20 << "<" << endl;
    cout << "After DownCase  >" << str20.DownCase () << "<" << endl;

    Isis::iString str21 = "Test String";
    QString qstr1 = "Test String";
    cout << "Testing QT conversion: " << (str21.ToQt() == qstr1) << endl;

    Isis::iString str22(255.0);
    Isis::iString str23(0.333);
    Isis::iString str24(-255.0);
    Isis::iString str25(1.235E-20);
    Isis::iString str26(12345678901234567890.0);
    Isis::iString str27(0.0005);
    cout << str22 << endl;
    cout << str23 << endl;
    cout << str24 << endl;
    cout << str25 << endl;
    cout << str26 << endl;
    cout << str27 << endl;

    Isis::iString str28("a 1 b 2 c 3 d 4 e 5");
    cout << "Before Remove >" << str28 << "<" << endl;
    cout << "After Remove >" << str28.Remove("1245") << "<" << endl;

    Isis::iString str29("100");
    int i = str29;
    cout << i << endl;

    Isis::iString str29a("100000000000");
    Isis::BigInt j = str29a;
    cout << j << endl;

    Isis::iString str30("100.1");
    double d = str30;
    cout << d << endl;

    Isis::iString str31("I is a test string: 'I is' is \"is a string\"");
    Isis::iString str31a("I is a test string: 'I is' is \"is a string\"");
    cout << "Before Replace (honor quotes) >" << str31 <<  "<" << endl;
    str31.Replace("is","am",true);
    cout << "After Replace (honor quotes) >" << str31 <<  "<" << endl;
    cout << "Before Replace (dont honor quotes) >" << str31a <<  "<" << endl;
    str31a.Replace("is","am",false);
    cout << "After Replace (dont honor quotes) >" << str31a <<  "<" << endl;

  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  //Tests for static functions untested above
  try {
    //Split test
    string str1 = "This is a test string";
    vector<string> strVec;
    Isis::iString::Split(' ', str1, strVec);
    cout << "Before split >" << str1 << "<" << endl;
    cout << "After split >";
    for (unsigned int i=0 ; i < strVec.size() ; i++) {
      cout << endl << "Element " << i << ": " << strVec[i];
    }
    cout << endl << "<" << endl;

    QStringList qlist;
    qlist << "String1"
          << "String2"
          << "String3"
          << "String4";

    cout << "\nQt QStringList values:\n";
    for (int i = 0 ; i < qlist.size() ; i++) {
      cout << qlist.at(i).toLocal8Bit().constData() << endl;
    }

    cout << "\nConverted to std::vector<std::string>...\n";
    vector<string> slist = Isis::iString::ToStd(qlist);
    copy(slist.begin(), slist.end(), ostream_iterator<string>(cout, "\n"));

    cout << "Are they equivalent?\n";
    cout << "Counts? " << yesOrNo((int) slist.size() == qlist.size()) << endl;
    for (int j = 0 ; j < qlist.size() ; j++) {
       QString qs = qlist.at(j);
       string  ss = slist.at(j);
       cout << "string(" << ss << ") == QString(" 
            << qs.toLocal8Bit().constData() << ")? "
            << yesOrNo(ss == qs.toStdString()) << endl;
    }

  } catch (Isis::iException &error) {
    error.Report (false);
  }
}
