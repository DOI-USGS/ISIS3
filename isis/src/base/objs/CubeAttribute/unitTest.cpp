/**
 * @file
 *
 * Test driver that tests this Object for accuracy and correct behavior.
 * $Revision: 1.1.1.1 $
 * $Id: unitTest.cpp,v 1.1.1.1 2006/10/31 23:18:06 isis3mgr Exp $
 * $Author: isis3mgr $
 * $Date: 2006/10/31 23:18:06 $
 *
 */
 
 /***********************************************************************
 *				PLEASE NOTE				*
 * This unit test modifies the truth file that it is meant to be tested *
 * against. The reason for this is that the output of the unit test is  *
 * dependent upon the system architecture, so an unchanged truth file   *
 * will only be correct on LSB xor MSB machines. To see the code that   *
 * changes CubeAttriube.truth, go to lines 254 - ###			*
 *									*
 ***********************************************************************/

#include <iostream>
#include <string>
#include "iException.h"
#include "Preference.h"
#include "CubeAttribute.h"
#include "EndianSwapper.h"
#include "Pvl.h"

using namespace std;

int main (int argc, char *argv[]) {

  void ReportOutput (Isis::CubeAttributeOutput *att, string oh);

  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::CubeAttribute and its subclasses" << endl << endl;

  cout << "Test of invalid attribute \"sometext\"" << endl;
  try {
    Isis::CubeAttribute *att = new Isis::CubeAttribute("sometext");
    delete att;
    cout << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;

  cout << "Test of attribute \"+sometext\"" << endl;
  try {
    Isis::CubeAttribute *att = new Isis::CubeAttribute("+sometext");
    att->Write(cout);
    cout << endl;
    string str;
    att->Write(str);
    cout << str << endl;
    Isis::Pvl pvl;
    att->Write(pvl);
    cout << pvl << endl;
    cout << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;


  cout << "Test of system default output cube attributes" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput();
    ReportOutput (att, "SYS");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (true);
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+8bit+Tile+0.0:100.1+MSB\"" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput("+8bit+Tile+0.0:100.1+MSB");
    ReportOutput (att, "MSB");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (true);
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+16bit+Bsq+-10000.0:-100.1+lsb\"" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput ("+16bit+Bsq+-100000.0:-100.1+lsb");
    ReportOutput (att, "LSB");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+32bit\"" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput("+32bit+tile+999:9999");
    ReportOutput (att,"SYS");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+0.0:100.1+detached\"" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput ("+0.0:100.1+detached");
    ReportOutput (att,"SYS");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+8bit+Tile\"" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput ("+8bit+Tile");
    ReportOutput (att, "SYS");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;

  cout << "Test of output attribute \"Defaults\" with Set" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput ();
    att->Set("+8-bit+Detached");
    ReportOutput (att, "SYS");
    cout << endl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;


  cout << "Test of input attribute \"+3\"" << endl;
  try {
    Isis::CubeAttributeInput *att = new Isis::CubeAttributeInput("+3");
    att->Write(cout);
    cout << endl;
    string str;
    att->Write(str);
    cout << str << endl;
    Isis::Pvl pvl;
    att->Write(pvl);
    cout << pvl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;

  cout << "Test of input attribute \"+3,5-9,99\"" << endl;
  try {
    Isis::CubeAttributeInput *att = new Isis::CubeAttributeInput("+3,5-9,99");
    att->Write(cout);
    cout << endl;
    string str;
    att->Write(str);
    cout << str << endl;
    Isis::Pvl pvl;
    att->Write(pvl);
    cout << pvl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;

  cout << "Test of input attribute \"+7-10\"" << endl;
  try {
    Isis::CubeAttributeInput *att = new Isis::CubeAttributeInput("+7-10");
    att->Write(cout);
    cout << endl;
    string str;
    att->Write(str);
    cout << str << endl;
    Isis::Pvl pvl;
    att->Write(pvl);
    cout << pvl << endl;
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;
  

  cout << "Testing put members (strings)" << endl;
  try {
    Isis::CubeAttributeOutput *att = new Isis::CubeAttributeOutput();
    att->Set("bsq");
    att->Set("8bit");
    att->Set("msb");
    att->Set("dETacHEd");
    att->Minimum(1.0);
    att->Maximum(2.0);
    att->Write(cout);
    delete att;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
  cout << endl << endl;

}


// Function to report everything about an output cube attribute
void ReportOutput (Isis::CubeAttributeOutput *att, string orderHint) {
  att->Write(cout);
  cout << endl;
  Isis::Pvl pvl;
  att->Write(pvl);
  cout << pvl << endl;
  cout << endl;
  cout << "Propagate Pixel Type = " << att->PropagatePixelType() << endl;
  try {
    string tmp =   Isis::PixelTypeName(att->PixelType());
    cout << "PixelType            = " << tmp << endl;
  }
  catch (Isis::iException &error) {
    error.Report(false);
  }
  cout << "Propagate Min/Max    = " << att->PropagateMinimumMaximum() << endl;
  cout << "Minimum              = " << att->Minimum() << endl;
  cout << "Maximum              = " << att->Maximum() << endl;
  cout << "FileFormatStr        = " << att->FileFormatStr() << endl;
  cout << "FileFormat enum      = " << CubeFormatName(att->FileFormat()) << endl;

//  cout << "ByteOrderStr         = " << att->ByteOrderStr() << endl;
//  cout << "ByteOrder enum       = " << Isis::ByteOrderName(att->ByteOrder()) << endl;
  Isis::ByteOrder oh;
  if (orderHint == "SYS") {
    if (Isis::IsLsb()) {
      oh = Isis::Lsb;
    }
    else {
      oh = Isis::Msb;
    }
  }
  else {
    oh = Isis::ByteOrderEnumeration(orderHint);
  }
  Isis::ByteOrder order =  att->ByteOrder();
  if (order == oh) {
    cout << "ByteOrder            = ok" << endl;
  }
  else  {
    cout << "ByteOrder            = wrong" << endl;
  }

  cout << "Label attachment     = ";
  if (att->AttachedLabel()) cout << LabelAttachmentName(Isis::AttachedLabel) << endl;
  if (att->DetachedLabel()) cout << LabelAttachmentName(Isis::DetachedLabel) << endl;

#if 0
  fstream stream("CubeAttribute.truth", ios::in| ios::out);
  int positions[] = {203, 1520, 1877, 2388, 2767, 3213};
  if (stream.is_open()){
    for (int i = 0 ; i < 6 ; i++){
      stream.seekg(positions[i]);
      if (Isis::IsLsb())
        stream.put('L');
      else
        stream.put('M');
    }
  }
  stream.close();
#endif
}



