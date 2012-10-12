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
*                             PLEASE NOTE                              *
* This unit test modifies the truth file that it is meant to be tested *
* against. The reason for this is that the output of the unit test is  *
* dependent upon the system architecture, so an unchanged truth file   *
* will only be correct on LSB xor MSB machines. To see the code that   *
* changes CubeAttriube.truth, go to lines 254 - ###                    *
*                                                                      *
***********************************************************************/

#include <iostream>
#include <string>

#include "CubeAttribute.h"
#include "EndianSwapper.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "Pvl.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void reportOutput(const CubeAttributeOutput &att, IString oh);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "Unit test for CubeAttribute and its subclasses" << endl << endl;

  cout << "Test of invalid attribute \"sometext\"" << endl;
  try {
    CubeAttributeInput att("sometext");
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;

  cout << "Test of attribute \"+sometext\"" << endl;
  try {
    CubeAttributeInput att("+sometext");
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of system default output cube attributes" << endl;
  try {
    CubeAttributeOutput att;
    reportOutput(att, "SYS");
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+8bit+Tile+0.0:100.1+MSB\"" << endl;
  try {
    CubeAttributeOutput att("+8bit+Tile+0.0:100.1+MSB");
    reportOutput(att, "MSB");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+16bit+Bsq+-10000.0:-100.1+lsb\"" << endl;
  try {
    CubeAttributeOutput att("+16bit+Bsq+-100000.0:-100.1+lsb");
    reportOutput(att, "LSB");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+32bit+tile+999:9999\"" << endl;
  try {
    CubeAttributeOutput att("+32bit+tile+999:9999");
    reportOutput(att, "SYS");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+0.0:100.1+detached\"" << endl;
  try {
    CubeAttributeOutput att("+0.0:100.1+detached");
    reportOutput(att, "SYS");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of output attribute \"+8bit+Tile\"" << endl;
  try {
    CubeAttributeOutput att("+8bit+Tile");
    reportOutput(att, "SYS");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;

  cout << "Test of output attribute \"Defaults\" with Set" << endl;
  try {
    CubeAttributeOutput att;
    att.setAttributes("+8-bit+Detached");
    reportOutput(att, "SYS");
    cout << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Test of input attribute \"+3\"" << endl;
  try {
    CubeAttributeInput att("+3");
    cout << att.toString() << endl;

    vector<string> bands = att.bands();
    cout << "vector[" << bands.size() << "]:" << endl;

    for (unsigned int i = 0; i < bands.size(); i++)
      cout << "\t" << bands[i] << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;

  cout << "Test of input attribute \"+3,5-9,99\"" << endl;
  try {
    CubeAttributeInput att("+3,5-9,99");
    cout << att.toString() << endl;

    vector<string> bands = att.bands();
    cout << "vector[" << bands.size() << "]:" << endl;

    for (unsigned int i = 0; i < bands.size(); i++)
      cout << "\t" << bands[i] << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;

  cout << "Test of input attribute \"+7-10\"" << endl;
  try {
    CubeAttributeInput att("+7-10");
    cout << att.toString() << endl;

    vector<string> bands = att.bands();
    cout << "vector[" << bands.size() << "]:" << endl;

    for (unsigned int i = 0; i < bands.size(); i++)
      cout << "\t" << bands[i] << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Testing CubeAttributeOutput mutators" << endl;
  try {
    CubeAttributeOutput att;
    att.setFileFormat(Cube::Bsq);
    att.addAttributes(IString("8bit"));
    att.addAttributes("msb");
    att.setByteOrder(Msb);
    att.addAttributes(FileName("+dETacHEd"));
    att.setMinimum(1.0);
    att.setMaximum(2.0);
    att.setPixelType(UnsignedByte);
    cout << att.toString() << endl;

    att.addAttributes("Attached");
    att.setMaximum(12.0);
    att.setPixelType(Real);
    cout << att.toString() << endl;

    att.setLabelAttachment(DetachedLabel);
    cout << att.toString() << endl;

    att.setLabelAttachment(ExternalLabel);
    cout << att.toString() << endl;
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl << endl;


  cout << "Testing CubeAttributeInput mutators" << endl;
  try {
    CubeAttributeInput att("+1-3,4,5,6,99-32");
    vector<string> bandsVector(1, "+1-99");
    att.setBands(bandsVector);
    cout << att.toString() << endl;
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl << endl;

  cout << "Regression Testing" << endl;
  {
    CubeAttributeOutput att;
    att.setAttributes("+real");
    att.addAttributes(FileName("output/makecubeTruth5.cub"));
    cout << att.toString() << endl;
  }

  // This is a correct result: The +'s are part of the path. This was suspected of being a bug.
  CubeAttributeOutput att;
  att.setAttributes("+real+output/makecubeTruth5.cub");
  if (att.toString() != "") {
    cout << "Failed to differentiate +'s in path versus +'s in file name" << endl;
  }
  
  try {
    CubeAttributeOutput att;
    att.setMinimum(Null);
    att.setMaximum(52.0);
    cout << att.toString() << endl;
  }
  catch (IException &e) {
    e.print();
  }
}


// Function to report everything about an output cube attribute
void reportOutput(const CubeAttributeOutput &att, IString orderHint) {
  cout << att.toString() << endl;

//   Pvl pvl;
//   att.Write(pvl);
//   cout << pvl << endl;
//   cout << endl;
  cout << "Propagate Pixel Type = " << att.propagatePixelType() << endl;
  try {
    string tmp =   PixelTypeName(att.pixelType());
    cout << "PixelType            = " << tmp << endl;
  }
  catch(IException &e) {
    e.print();
  }
  cout << "Propagate Min/Max    = " << att.propagateMinimumMaximum() << endl;
  cout << "Minimum              = " << att.minimum() << endl;
  cout << "Maximum              = " << att.maximum() << endl;
  cout << "FileFormatStr        = " << att.fileFormatString() << endl;

//  cout << "ByteOrderStr         = " << att.ByteOrderStr() << endl;
//  cout << "ByteOrder enum       = " << ByteOrderName(att.ByteOrder()) << endl;
  ByteOrder oh;
  if(orderHint == "SYS") {
    if(IsLsb()) {
      oh = Lsb;
    }
    else {
      oh = Msb;
    }
  }
  else {
    oh = ByteOrderEnumeration(orderHint);
  }
  ByteOrder order =  att.byteOrder();
  if(order == oh) {
    cout << "ByteOrder            = ok" << endl;
  }
  else  {
    cout << "ByteOrder            = wrong" << endl;
  }

  cout << "Label attachment     = ";
  if(att.labelAttachment() == AttachedLabel)  cout << LabelAttachmentName(AttachedLabel) << endl;
  if(att.labelAttachment() == DetachedLabel) cout << LabelAttachmentName(DetachedLabel) << endl;

#if 0
  fstream stream("CubeAttribute.truth", ios::in | ios::out);
  int positions[] = {203, 1520, 1877, 2388, 2767, 3213};
  if(stream.is_open()) {
    for(int i = 0 ; i < 6 ; i++) {
      stream.seekg(positions[i]);
      if(IsLsb())
        stream.put('L');
      else
        stream.put('M');
    }
  }
  stream.close();
#endif
}



