#include "Isis.h"

#include <string>

#include "ProcessMosaic.h"
#include "Portal.h"
#include "Application.h"

using namespace std;

int giDefault = 0;

void TestIn(int iss, int isl, int isb, int ins = 0, int inl = 0, int inb = 0);
void TestOut(int piSamples, int piLines, int piBands, int piOffset, int piPriority);

/**
 * Display the contents of Input image with starting and number of
 * samples, lines,bands
 *
 * @author sprasad (10/14/2009)
 *
 * @param iss  - input starting sample
 * @param isl  - input starting line
 * @param isb  - input starting band
 * @param ins  - input number of samples
 * @param inl  - input number of lines
 * @param inb  - input number of bands
 */
void TestIn(int iss, int isl, int isb, int ins, int inl, int inb) {
  Isis::Cube cInCube;
  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  string sFrom = ui.GetFilename("FROM");
  cInCube.Open(sFrom);

  cout << "\n***  Input Image  ***  ";
  if(ins == 0) ins = cInCube.Samples() - iss + 1;
  if(inl == 0) inl = cInCube.Lines()   - isl + 1;
  if(inb == 0) inb = cInCube.Bands()   - isb + 1 ;

  printf("Stats %d, %d, %d, %d, %d, %d\n", isl, iss, isb, inl, ins, inb);

  int iS;
  Isis::Portal ciPortal(ins, 1, cInCube.PixelType());
  for(int band = isb; band <= (isb + inb - 1); band++) {
    for(int line = isl; line <= (isl + inl - 1); line++) {
      iS = iss;
      ciPortal.SetPosition(iss, line, band);  //sample, line, band position
      cInCube.Read(ciPortal);
      for(int iPixel = 0; iPixel < ciPortal.size(); iPixel++) {
        if(iPixel == 5) {
          cout << endl;
        }
        printf("(%d,%d,%d)=%-11d  ", line, iS++, band, (int)ciPortal[iPixel]);
      }
      cout << "\n";
    }
    cout << "\n";
  }
  cInCube.Close();
}

/**
 * Display the contents of Ouput image and display the sample, line and band
 * stas for which it the mosaic is tested
 *
 * @author sprasad (10/14/2009)
 *
 * @param iss  - input starting sample
 * @param isl  - input starting line
 * @param isb  - input starting band
 */
void TestOut(int piSamples, int piLines, int piBands, int piOffset, int piPriority) {
  int iFileIndex;

  Isis::Cube cOutCube;
  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  string sTo;
  if (piPriority == Isis::ProcessMosaic::average)
    sTo = ui.GetFilename("TO_AVG");
  else
    sTo = ui.GetFilename("TO");
  cOutCube.Open(sTo);

  int iBands=cOutCube.Bands();
  
  cout << "\n***  Mosaic Image  ***  ";
  printf("Start Stats %d, %d, %d\nTotal Bands=%d\n", piLines, piSamples, piBands, iBands);
  Isis::Portal coPortal(5, 1, cOutCube.PixelType());
  int band = piBands;
  while(band <= iBands) {
    for(int line = 1; line <= 5; line++) {
      coPortal.SetPosition(1, line, band);  //sample, line, band position
      cOutCube.Read(coPortal);
      for(int iPixel = 0; iPixel < coPortal.size(); iPixel++) {
        iFileIndex = 0;
        if(piPriority != Isis::ProcessMosaic::average && band == 3 && coPortal[iPixel] != giDefault) {
          iFileIndex = (int)coPortal[iPixel] - piOffset + 1;
        }
        if(band == 3 && piPriority != Isis::ProcessMosaic::average) { //origin band
          printf("(%d,%d,%d)=%-9d,%-1d  ", line, (iPixel + 1), band, (int)coPortal[iPixel], iFileIndex);
        }
        else
          printf("(%d,%d,%d)=%-11d  ", line, (iPixel + 1), band, (int)coPortal[iPixel]);
      }
      cout << "\n";
    }
    cout << "\n";
    band++;
    if(band > iBands) {
      break;
    }
    band = (piPriority == Isis::ProcessMosaic::average ? 2 : 3);
  }
  cOutCube.Close();
}
/**
 * unitTest for ProcessMosaic
 * tests for correct area drop, tracking origin,  origin band,
 * priorities input, mosaic and band, options to allow HS, LS and NULL
 * pixels from input to mosaic, each time displaying the contents of the
 * input and mosaic pixels for the area under consideration
 *
 * Also tests for exceptions like number of input and output images to
 * be exactly one each, band cannot be priority if Track is set off and
 * more
 *
 * @author sprasad (10/14/2009)
 */
void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessMosaic Class ... " << endl;

  // Create the default output cube
  Isis::Process p;
  p.SetOutputCube("TO", 5, 5, 3);
  p.EndProcess();

  // ***********************************************************
  // Drop a small area into the middle of the output
  cout << "Create output mosaic with Tracking set to True\n";
  cout << "1. Drop a small area into the middle of the output\n";
  Isis::ProcessMosaic m1;
  m1.SetTrackFlag(true);
  m1.SetCreateFlag(true);
  m1.SetPriority(Isis::ProcessMosaic::input);

  m1.SetInputCube("FROM", 1, 1, 1, 10, 5, 1);

  m1.SetOutputCube("TO");

  int iDefault = m1.GetIndexOffsetByPixelType();
  giDefault = m1.GetOriginDefaultByPixelType();

  m1.StartProcess(2, 2, 1);

  // Test for Tracking Table "Input Images"
  if(m1.GetTrackStatus()) {
    cout << "a. SUCCESS - Track Table Exists\n";
  }
  else {
    cout << "a. FAILURE - Track Table does not Exist\n";
  }

  // Test for Tracking Band
  FileType eBandFile   = outFile; //output
  m1.SetBandKeyWord("OriginalBand", "TRACKING");
  if(m1.GetBandIndex(eBandFile) == 3) {
    cout << "b. SUCCESS - Tracking Band Exists\n\n";
  }
  else {
    cout << "b. FAILURE - Tracking Band does not Exist\n\n";
  }
  m1.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(2, 2, 1, iDefault, Isis::ProcessMosaic::input);

  remove("isisMosaic_01.cub");

  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Drop 2,2,1 into the lower right corner of band 2
  cout << "2. Drop 2,2,1 into the lower right corner of band 2\n";
  cout << "Tracking is set to False\n";
  Isis::ProcessMosaic m2;
  m2.SetTrackFlag(false);
  m2.SetCreateFlag(true);
  m2.SetPriority(Isis::ProcessMosaic::input);

  m2.SetInputCube("FROM", 2, 2, 1);

  p.SetOutputCube("TO", 5, 5, 3);
  p.EndProcess();
  m2.SetOutputCube("TO");

  m2.StartProcess(4, 4, 2);
  m2.EndProcess();

  TestIn(2, 2, 1, 5, 5, 1);
  TestOut(4, 4, 2, iDefault, Isis::ProcessMosaic::input);
  remove("isisMosaic_01.cub");
  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Drop 3,3,1 into the upper right corner of band 1
  cout << "3. Drop 3,3,1 into the upper right corner of band 1\n";
  Isis::ProcessMosaic m3;
  m3.SetTrackFlag(true);
  m3.SetCreateFlag(true);
  m3.SetPriority(Isis::ProcessMosaic::mosaic);

  m3.SetInputCube("FROM", 3, 3, 1, 10, 1, 1);

  p.SetOutputCube("TO", 5, 5, 3);
  p.EndProcess();
  m3.SetOutputCube("TO");

  m3.StartProcess(5, 1, 1);
  m3.EndProcess();

  TestIn(3, 3, 1, 5, 5, 1);
  TestOut(5, 1, 1, iDefault, Isis::ProcessMosaic::mosaic);

  remove("isisMosaic_01.cub");
  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Drop the first 3x3x1  the upper left corner
  cout << "4. Drop the first 3x3x1 to the upper left corner\n";
  Isis::ProcessMosaic m4;
  m4.SetTrackFlag(true);
  m4.SetCreateFlag(true);
  m4.SetPriority(Isis::ProcessMosaic::mosaic);

  m4.SetInputCube("FROM", 1, 1, 1, 3, 3, 1);

  p.SetOutputCube("TO", 5, 5, 3);
  p.EndProcess();
  m4.SetOutputCube("TO");

  m4.StartProcess(1, 1, 1);
  m4.EndProcess();

  TestIn(1, 1, 1, 3, 3, 1);
  TestOut(1, 1, 1, iDefault, Isis::ProcessMosaic::mosaic);

  //remove("isisMosaic_01.cub");
  cout << "************************************************************************************************\n";

  // Test for mosaic(beneath)  priority
  cout << "5. Test for mosaic priority with existing mosaic\n";
  Isis::ProcessMosaic m5;
  //m5.SetTrackFlag (true);
  //m5.SetCreateFlag(false);
  m5.SetPriority(Isis::ProcessMosaic::mosaic);

  m5.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);

  m5.SetOutputCube("TO");

  m5.StartProcess(1, 2, 1);
  m5.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 2, 1, iDefault, Isis::ProcessMosaic::mosaic);

  cout << "************************************************************************************************\n";


  // ***********************************************************
  // Test for band priority using Keywords for band id
  cout << "6. Test for band priority with Keyname \"FilterName\" and value \"Red\" with Criteria \"Greater\" than in an existing mosaic\n";
  Isis::ProcessMosaic m6;
  m6.SetTrackFlag(true);
  m6.SetCreateFlag(true);
  m6.SetPriority(Isis::ProcessMosaic::band);
  m6.SetBandKeyWord("FilterName", "red");
  m6.SetBandCriteria(Greater);

  m6.SetInputCube("FROM", 3, 3, 1, 10, 1, 1);

  //p.SetOutputCube("TO", 5, 5, 3);
  //p.EndProcess();
  m6.SetOutputCube("TO");

  m6.StartProcess(1, 1, 1);
  m6.EndProcess();

  TestIn(3, 3, 1, 10, 1, 1);
  TestOut(1, 1, 1, iDefault, Isis::ProcessMosaic::band);

  cout << "************************************************************************************************\n";

  cout << "7. Test for band priority for existing mosaic with Keyname \"OriginalBand\" and value \"1\" and Criteria \"Lesser\" than\n";
  Isis::ProcessMosaic m7;
  m7.SetTrackFlag(true);
  m7.SetCreateFlag(false);
  m7.SetPriority(Isis::ProcessMosaic::band);
  m7.SetBandKeyWord("OriginalBand", "1");
  m7.SetBandCriteria(Lesser);
  m7.SetHighSaturationFlag(false) ;
  m7.SetLowSaturationFlag(false) ;
  m7.SetNullFlag(false) ;

  m7.SetInputCube("FROM", 1, 1, 1, 10, 1, 1);

  //p.SetOutputCube("TO", 5, 5, 3);
  //p.EndProcess();
  m7.SetOutputCube("TO");

  m7.StartProcess(1, 1, 1);
  m7.EndProcess();

  TestIn(1, 1, 1, 10, 1, 1);
  TestOut(1, 1, 1, iDefault, Isis::ProcessMosaic::band);

  //remove("isisMosaic_01.cub");
  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Test for band priority using Band Number
  cout << "8. Test for band priority with BandNumber set\n";
  Isis::ProcessMosaic m8;
  m8.SetTrackFlag(true);
  m8.SetCreateFlag(true);
  m8.SetPriority(Isis::ProcessMosaic::band);
  m8.SetBandNumber(1);
  m8.SetBandCriteria(Lesser);
  m8.SetHighSaturationFlag(true) ;
  m8.SetLowSaturationFlag(false) ;
  m8.SetNullFlag(false) ;

  m8.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);

  //p.SetOutputCube("TO", 5, 5, 3);
  //p.EndProcess();
  m8.SetOutputCube("TO");

  m8.StartProcess(1, 3, 1);
  m8.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 3, 1, iDefault, Isis::ProcessMosaic::band);

  cout << "************************************************************************************************\n";
  
  // ***********************************************************
  // Test for HS value set with existing mosaic
  cout << "9. Test for Null flag set with existing mosaic\n";
  Isis::ProcessMosaic m9;
  m9.SetPriority(Isis::ProcessMosaic::band);
  m9.SetBandNumber(1);
  m9.SetBandCriteria(Greater);
  m9.SetHighSaturationFlag(false) ;
  m9.SetLowSaturationFlag(false) ;
  m9.SetNullFlag(true) ;

  m9.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m9.SetOutputCube("TO");

  m9.StartProcess(1, 2, 1);
  m9.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 2, 1, iDefault, Isis::ProcessMosaic::band);
  
  //remove("isisMosaic_01.cub");
  
  // ***********************************************************
  // Test Average Priority
  cout << "\n10. Test Average Priority" << endl;
  // Create the default output cube
  p.SetOutputCube("TO_AVG", 5, 5, 2);
  p.EndProcess();

  Isis::ProcessMosaic m10;
  m10.SetTrackFlag(false);
  m10.SetPriority(Isis::ProcessMosaic::average);
  m10.SetCreateFlag(true);

  m10.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m10.SetOutputCube("TO_AVG");
  m10.StartProcess(1, 1, 1);
  m10.EndProcess();
  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 1, 1, iDefault, Isis::ProcessMosaic::average);

  m10.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m10.SetOutputCube("TO_AVG");
  m10.SetTrackFlag(false);
  m10.SetPriority(Isis::ProcessMosaic::average);
  m10.SetCreateFlag(false);

  m10.StartProcess(-1, -1, 1);
  m10.EndProcess();

  TestOut(1, 1, 1, iDefault, Isis::ProcessMosaic::average);
  remove("isisMosaic_02.cub");

  cout << "****** End Average **********************\n";
  // ***********************************************************
  // Testing Errors

  // Try to open two input cubes
  cout << "\n*** Test Error Handling ***\n";
  cout << "Test multiple input error" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Try to open two output cubes
  cout << "Test multiple output error" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetOutputCube("TO");
    m.SetOutputCube("TO");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Drop the input completly outside the output
  cout << "Test input does not overlap mosaic" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetOutputCube("TO");
    m.StartProcess(-20, 0, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  cout << "Test input does not overlap mosaic" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetOutputCube("TO");
    m.StartProcess(54, 23, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Don't open an input cube
  cout << "Test no input cube" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetOutputCube("TO");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  //***********************************************************
  // Don't open an output cube
  cout << "Test no output cube" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  //***********************************************************
  // Band cannot be a priority if Track is not set
  cout << "Test Band cannot be a priority if Track is not set" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetTrackFlag(false);
    m.SetPriority(Isis::ProcessMosaic::band);
    m.SetBandNumber(1);

    m.SetOutputCube("TO");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Test Band not found with Band as Priority
  cout << "Test Band not found with Band as Priority" << endl;
  try {
    Isis::ProcessMosaic m;
    m.SetTrackFlag(true);
    m.SetPriority(Isis::ProcessMosaic::band);
    m.SetBandNumber(10);

    m.SetOutputCube("TO");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    //m.Test();
    m.EndProcess();
  }
  catch(Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }
  remove("isisMosaic_01.cub");
}

