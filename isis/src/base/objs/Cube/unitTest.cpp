#include <iostream>
#include "iException.h"
#include "Cube.h"
#include "LineManager.h"
#include "Pvl.h"
#include "Preference.h"
#include "Histogram.h"
#include "Statistics.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

try {
  void Report(Isis::Cube &c);
  cout << "Unit test for Isis::Cube" << endl;

  cout << "Constructing cube ... " << endl;
  Isis::Cube out;
  Report(out);

  // Test create and write methods
  cout << "Creating 32-bit cube ... " << endl;
  out.SetDimensions(150,200,2);
  out.Create("/tmp/IsisCube_01");
  Report(out);

  cout << "Write cube ... " << endl;
  Isis::LineManager line(out);
  int j = 0;
  for (line.begin(); !line.end(); line++) {
    for (int i=0; i<line.size(); i++) {
      line[i] = (double) j;
      j++;
    }
    j--;
    out.Write(line);
  }
  out.Close();

  // Test the open and read methods
  cout << "Opening cube ... " << endl;
  Isis::Cube in;
  in.Open("/tmp/IsisCube_01");
  Report(in);
  
  cout << "Comparing cube ... " << endl;
  Isis::LineManager inLine(in);
  j = 0;
  for (inLine.begin(); !inLine.end(); inLine++) {
    in.Read(inLine);
    for (int i=0; i<inLine.size(); i++) {
      if (inLine[i] != (double) j) {
        cout << "Problem at line " << inLine.Line() 
             << " sample " << i + 1 << ":  "
             << inLine[i] << " != " << double(j) << endl;
      }
      j++;
    }
    j--;
  }
  in.Close();
  cout << endl;

  // Test other options for output
  cout << "Creating 8-bit cube ... " << endl;
  Isis::Cube out2;
  out2.SetDimensions(150,200,1);
  out2.SetDetached();
  out2.SetBaseMultiplier(200.0,-1.0);
//  out2.SetByteOrder(Isis::Msb);
  out2.SetByteOrder(ISIS_LITTLE_ENDIAN ? Isis::Msb : Isis::Lsb);
  out2.SetCubeFormat(Isis::Bsq);
  out2.SetLabelBytes(1000);
  out2.SetPixelType(Isis::UnsignedByte);
  out2.Create("/tmp/IsisCube_02");

  j = 0;
  Isis::LineManager oline(out2);
  for (oline.begin(); !oline.end(); oline++) {
    for (int i=0; i<oline.size(); i++) {
      oline[i] = (double) j;
    }
    out2.Write(oline);
    j++;
  }
  out2.Close();

  cout << "Comparing cube ... " << endl;
  Isis::Cube in2;
  in2.Open("/tmp/IsisCube_02");
  Report(in2);
  j = 0;
  Isis::LineManager inLine2(in2);
  for (inLine2.begin(); !inLine2.end(); inLine2++) {
    in2.Read(inLine2);
    for (int i=0; i<inLine2.size(); i++) {
      if (inLine2[i] != (double) j) {
        cout << "Problem at line " << inLine2.Line() 
             << " sample " << i + 1 << ":  "
             << inLine2[i] << " != " << double(j) << endl;
      }
    }
    j++;
  }
  in2.Close();


  // Test other options for output
  cout << "Creating 16-bit cube ... " << endl;
  Isis::Cube out3;
  out3.SetDimensions(150,200,2);
  out3.SetBaseMultiplier(30000.0,-1.0);
//  out3.SetByteOrder(Isis::Msb);
  out2.SetByteOrder(ISIS_LITTLE_ENDIAN ? Isis::Msb : Isis::Lsb);
  out3.SetPixelType(Isis::SignedWord);
  out3.Create("/tmp/IsisCube_03");

  j = 0;
  Isis::LineManager oline3(out3);
  for (oline3.begin(); !oline3.end(); oline3++) {
    for (int i=0; i<oline3.size(); i++) {
      oline3[i] = (double) j;
      j++;
    }
    out3.Write(oline3);
  }
  out3.Close();

  cout << "Comparing cube ... " << endl;
  Isis::Cube in3;
  in3.Open("/tmp/IsisCube_03");
  Report(in3);
  j = 0;
  Isis::LineManager inLine3(in3);
  for (inLine3.begin(); !inLine3.end(); inLine3++) {
    in3.Read(inLine3);
    for (int i=0; i<inLine3.size(); i++) {
      if (inLine3[i] != (double) j) {
        cout << "Problem at line " << inLine3.Line() 
             << " sample " << i + 1 << ":  "
             << inLine3[i] << " != " << double(j) << endl;
      }
      j++;
    }
  }
  in3.Close();


  in.Open("/tmp/IsisCube_01");

  // Test Histogram object on a single band, 1 by default
  cout << "Testing histogram method, band 1 ... " << endl;
  Isis::Histogram *bandOneHist = in.Histogram();
  cout << "Average:        " << bandOneHist->Average() << endl;
  cout << "Standard Dev:   " << bandOneHist->StandardDeviation() << endl;
  cout << "Mode:           " << bandOneHist->Mode() << endl;
  cout << "Total Pixels:   " << bandOneHist->TotalPixels() << endl;
  cout << "Null Pixels:    " << bandOneHist->NullPixels() << endl;
  cout << endl;
  
  // Test histogram object on all bands
  cout << "Testing histogram method, all bands ... " << endl;
  Isis::Histogram *allBandsHistogram = in.Histogram(0);
  cout << "Average:        " << allBandsHistogram->Average() << endl;
  cout << "Standard Dev:   " << allBandsHistogram->StandardDeviation() << endl;
  cout << "Mode:           " << allBandsHistogram->Mode() << endl;
  cout << "Total Pixels:   " << allBandsHistogram->TotalPixels() << endl;
  cout << "Null Pixels:    " << allBandsHistogram->NullPixels() << endl;
  cout << endl;
  
  // Check error for too few (negative) bands
  try {
    in.Histogram(-1);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  
  // Test statistics object on a single band, 1 by default
  cout << "Testing statistics method, band 1 ... " << endl;
  Isis::Statistics *bandOneStats = in.Statistics();
  cout << "Average:        " << bandOneStats->Average() << endl;
  cout << "Standard Dev:   " << bandOneStats->StandardDeviation() << endl;
  cout << "Total Pixels:   " << bandOneStats->TotalPixels() << endl;
  cout << "Null Pixels:    " << bandOneStats->NullPixels() << endl;
  cout << endl;
  
  // Test statistics object on all bands
  cout << "Testing statistics method, all bands ... " << endl;
  Isis::Statistics *allBandsStats = in.Statistics(0);
  cout << "Average:        " << allBandsStats->Average() << endl;
  cout << "Standard Dev:   " << allBandsStats->StandardDeviation() << endl;
  cout << "Total Pixels:   " << allBandsStats->TotalPixels() << endl;
  cout << "Null Pixels:    " << allBandsStats->NullPixels() << endl;
  cout << endl;

  // Check error for too few (negative) bands
  try {
    in.Statistics(-1);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  cout << endl;

  cout << "Virtual band tests" << endl;  // Virtual Band tests

  cout << "Nbands = " << in.Bands() << endl;
  cout << "Band 1 = " << in.PhysicalBand(1) << endl;
  cout << "Band 2 = " << in.PhysicalBand(2) << endl;
  in.Close();
  cout << endl;

  vector<string> vbands; vbands.push_back("2");
  in.SetVirtualBands(vbands);
  in.Open("/tmp/IsisCube_01");
  cout << "Nbands = " << in.Bands() << endl;
  cout << "Band 1 = " << in.PhysicalBand(1) << endl;
  cout << endl;


  //  Test ReOpen
  cout << "ReOpen tests" << endl;
  Report(in);
  in.ReOpen("rw");
  Report(in);
  in.ReOpen("r");
  Report(in);

  // Check errors
  cout << "Testing errors ... " << endl;
  try {
    in.Open("blah");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    in.Create("blah");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    in.Write(inLine3);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    Isis::Cube in;
    in.Open("blah");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    in.PhysicalBand(2);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  
  try {
    in.PhysicalBand(0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    Isis::Cube in;
    in.Read(inLine3);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  try {
    Isis::Cube in;
    in.Write(inLine3);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  
  try {
    Isis::Cube out;
    out.SetLabelBytes(15);
    out.Create("/tmp/IsisCube_04");
    out.Close();
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    Isis::Cube out;
    out.SetDimensions(1000000,1000000,9);
    out.Create("/tmp/IsisCube_05");
    out.Close();
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    Isis::Cube in;
    in.Open("/tmp/IsisCube_01","a");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    Isis::Cube in;
    in.SetDimensions(0,0,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    Isis::Cube in;
    in.SetDimensions(1,0,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    Isis::Cube in;
    in.SetDimensions(1,1,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  Isis::Cube in4;
  try {
    in4.Open("$base/testData/isisTruth.cub");
  } 
  catch (Isis::iException &e) {
    e.Report(false);
  }
  try {
    in4.ReOpen("rw");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

}
catch (Isis::iException &e) {
  e.Report();
}

  remove ("/tmp/IsisCube_01.cub"); 
  remove ("/tmp/IsisCube_02.cub"); 
  remove ("/tmp/IsisCube_02.lbl"); 
  remove ("/tmp/IsisCube_03.cub"); 
  remove ("/tmp/IsisCube_04.cub"); 
  remove ("/tmp/IsisCube_05.cub"); 
  return 0;
}


void Report (Isis::Cube &c) {
  cout << "File   = " << c.Filename() << endl;
  cout << "Samps  = " << c.Samples() << endl;
  cout << "Lines  = " << c.Lines() << endl;
  cout << "Bands  = " << c.Bands() << endl;
  cout << "Base   = " << c.Base() << endl;
  cout << "Mult   = " << c.Multiplier() << endl;
  cout << "Type   = " << c.PixelType() << endl;
//  cout << "Order  = " << c.ByteOrder() << endl; // Needs to be system independent
  cout << "Atchd  = " << c.IsAttached() << endl;
  cout << "Dtchd  = " << c.IsDetached() << endl;
  cout << "Format = " << c.CubeFormat() << endl;
  cout << "Open   = " << c.IsOpen() << endl;
  cout << "R/O    = " << c.IsReadOnly() << endl;
  cout << "R/W    = " << c.IsReadWrite() << endl;
  cout << "Lbytes = " << c.LabelBytes() << endl;
  cout << endl;
}
