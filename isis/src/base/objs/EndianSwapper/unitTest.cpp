#include <iostream>
#include <cmath>
#include "EndianSwapper.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  Isis::EndianSwapper lsb ("LSB");
  Isis::EndianSwapper msb ("MSB");

  double DoubleValue;
  float FloatValue;
  short int ShortIntValue;
  unsigned short int UShortIntValue;
  int IntValue;
  long long LongLongIntValue;
  
  DoubleValue = 0x12345678;
  FloatValue = 0x1234;
  ShortIntValue = 0x1234;
  UShortIntValue = 0x1234;
  IntValue = 0x12345678;
  LongLongIntValue = 0x12345600;

  // Do the conversions from lsb to host first if this is a little endian machine
  if (ISIS_LITTLE_ENDIAN) {
    cout << "Size of Double:  " << sizeof (double);
    cout << "   Double 0x12345678 to HOST:  " << lsb.Double (&DoubleValue) << endl;
    cout << "Size of Float:  " << sizeof (float);
    cout << "   Float 0x1234 to HOST:  " << lsb.Float (&FloatValue) << endl;
    cout << "Size of Short Int:  " << sizeof (short int);
    cout << "   Short Int 0x1234 to HOST:  " << lsb.ShortInt (&ShortIntValue) << endl;
    cout << "Size of Unsigned Short Int:  " << sizeof (unsigned short int);
    cout << "   Unsigned Short Int 0x1234 to HOST:  " << lsb.UnsignedShortInt (&UShortIntValue) << endl;

    cout << "Size of Double:  " << sizeof (double);
    cout << "   Double 0x12345678 to HOST:  " << msb.Double (&DoubleValue) << endl;
    cout << "Size of Float:  " << sizeof (float);
    cout << "   Float 0x1234 to HOST:  " << msb.Float (&FloatValue) << endl;
    cout << "Size of Short Int:  " << sizeof (short int);
    cout << "   Short Int 0x1234 to HOST:  " << msb.ShortInt (&ShortIntValue) << endl;
    cout << "Size of Unsigned Short Int:  " << sizeof (unsigned short int);
    cout << "   Unsigned Short Int 0x1234 to HOST:  " << msb.UnsignedShortInt (&UShortIntValue) << endl;
    cout << "Size of Int:  " << sizeof (int);
    cout << "   Int 0x12345678 to HOST:  " << msb.Int (&IntValue) << endl;
    cout << "Size of Long Long Int:  " << sizeof (long long int);
    cout << "   Long Long Int 0x0000000012345600 to HOST:  " << msb.LongLongInt (&LongLongIntValue) << endl;

    // Test valid floats that when swapped become nan
    cout << "Testing nan:  " << endl;
    union {
      float myfloat;
      int myint;
      char mychar[4];
    } myunion;

    //myunion.myfloat = 0.0350742; Do NOT use this line, it is not the same as the bit pattern the following 4 lines produce
    myunion.mychar[0] = (char)-1;
    myunion.mychar[1] = (char)-87;
    myunion.mychar[2] = (char)15;
    myunion.mychar[3] = (char)61;
    cout << "PreSwap:  " << (int)(myunion.mychar[0]) << " " << (int)(myunion.mychar[1]);
    cout << " " << (int)(myunion.mychar[2]) << " " << (int)(myunion.mychar[3]) << " " << endl;
    myunion.myint = msb.ExportFloat( (void *)(&myunion.myfloat) );
    cout << "PostSwap: " << (int)(myunion.mychar[0]) << " " << (int)(myunion.mychar[1]);
    cout << " " << (int)(myunion.mychar[2]) << " " << (int)(myunion.mychar[3]) << " " << endl;

  }
  // Do the conversions from msb to host first if this is a big endian machine
  else  {
    cout << "Size of Double:  " << sizeof (double);
    cout << "   Double 0x12345678 to HOST:  " << msb.Double (&DoubleValue) << endl;
    cout << "Size of Float:  " << sizeof (float);
    cout << "   Float 0x1234 to HOST:  " << msb.Float (&FloatValue) << endl;
    cout << "Size of Short Int:  " << sizeof (short int);
    cout << "   Short Int 0x1234 to HOST:  " << msb.ShortInt (&ShortIntValue) << endl;
    cout << "Size of Unsigned Short Int:  " << sizeof (unsigned short int);
    cout << "   Unsigned Short Int 0x1234 to HOST:  " << msb.UnsignedShortInt (&UShortIntValue) << endl;

    cout << "Size of Double:  " << sizeof (double);
    cout << "   Double 0x12345678 to HOST:  " << lsb.Double (&DoubleValue) << endl;
    cout << "Size of Float:  " << sizeof (float);
    cout << "   Float 0x1234 to HOST:  " << lsb.Float (&FloatValue) << endl;
    cout << "Size of Short Int:  " << sizeof (short int);
    cout << "   Short Int 0x1234 to HOST:  " << lsb.ShortInt (&ShortIntValue) << endl;
    cout << "Size of Unsigned Short Int:  " << sizeof (unsigned short int);
    cout << "   Unsigned Short Int 0x1234 to HOST:  " << lsb.UnsignedShortInt (&UShortIntValue) << endl;
    cout << "Size of Int:  " << sizeof (int);
    cout << "   Int 0x12345678 to HOST:  " << lsb.Int (&IntValue) << endl;
    cout << "Size of Long Long Int:  " << sizeof (long long int);
    cout << "   Long Long Int 0x0000000012345600 to HOST:  " << lsb.LongLongInt (&LongLongIntValue) << endl;

    // Test valid floats that when swapped become nan
    cout << "Testing nan:  " << endl;
    union {
      float myfloat;
      int myint;
      char mychar[4];
    } myunion;

    //myunion.myfloat = 0.0350742; Do NOT use this line, it is not the same as the bit pattern the following 4 lines produce
    myunion.mychar[3] = (char)-1;
    myunion.mychar[2] = (char)-87;
    myunion.mychar[1] = (char)15;
    myunion.mychar[0] = (char)61;
    cout << "PreSwap:  " << (int)(myunion.mychar[3]) << " " << (int)(myunion.mychar[2]);
    cout << " " << (int)(myunion.mychar[1]) << " " << (int)(myunion.mychar[0]) << " " << endl;
    myunion.myint = lsb.ExportFloat( (void *)(&myunion.myfloat) );
    cout << "PostSwap: " << (int)(myunion.mychar[3]) << " " << (int)(myunion.mychar[2]);
    cout << " " << (int)(myunion.mychar[1]) << " " << (int)(myunion.mychar[0]) << " " << endl;

}

//  Test wrong parameter
  try {
    Isis::EndianSwapper invalid ("INV");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

}           
