#include "Isis.h"

#include "Application.h"
#include "IString.h"
#include "Portal.h"
#include "ProcessMosaic.h"
#include "Table.h"

using namespace Isis;
using namespace std;

void TestIn(int iss, int isl, int isb, int ins = 0, int inl = 0, int inb = 0);
void TestOut(int piSamples, int piLines, int piBands, int piPriority,
             int originBand);
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

  Preference::Preferences(true);

  cout << "Testing ProcessMosaic Class ... " << endl;

  // Create the default output cube
  Process p;
  p.SetOutputCube("TO", 5, 5, 2);
  p.EndProcess();

  // ***********************************************************
  // Drop a small area into the middle of the output
  cout << "Create output mosaic with Tracking set to True\n";
  cout << "1. Drop a small area into the middle of the output\n";
  ProcessMosaic m1;
  m1.SetTrackFlag(true);
  m1.SetCreateFlag(true);
  m1.SetImageOverlay(ProcessMosaic::PlaceImagesOnTop);

  m1.SetInputCube("FROM", 1, 1, 1, 10, 5, 1);

  Cube *mosaicCube1 = m1.SetOutputCube("TO");

  m1.StartProcess(5, 2, 1); // This should be overwritten by the next StartProcess call
  m1.StartProcess(2, 2, 1);

  // Test for Tracking Table "Input Images"
  try {
    Table trackTable(ProcessMosaic::TRACKING_TABLE_NAME);
    mosaicCube1->read(trackTable);
    cout << "\na. SUCCESS - Track Table Exists\n";
  }
  catch (IException &) {
    cout << "\na. FAILURE - Track Table does not Exist\n";
  }
  m1.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(2, 2, 1, ProcessMosaic::PlaceImagesOnTop, 2);

  remove("isisMosaic_01.cub");
  cout << "*************************************************************************************\n";

  // ***********************************************************
  // Drop 2,2,1 into the lower right corner of band 2
  cout << "2. Drop 2,2,1 into the lower right corner of band 2\n";
  cout << "Tracking is set to False\n";
  ProcessMosaic m2;
  m2.SetTrackFlag(false);
  m2.SetCreateFlag(true);
  m2.SetImageOverlay(ProcessMosaic::PlaceImagesOnTop);

  m2.SetInputCube("FROM", 2, 2, 1);

  p.SetOutputCube("TO", 5, 5, 2);
  p.EndProcess();
  m2.SetOutputCube("TO");

  m2.StartProcess(4, 4, 2);
  m2.EndProcess();

  TestIn(2, 2, 1, 5, 5, 1);
  TestOut(4, 4, 1, ProcessMosaic::PlaceImagesOnTop, 0);

  remove("isisMosaic_01.cub");
  cout << "*************************************************************************************\n";

  // ***********************************************************
  // Drop 3,3,1 into the upper right corner of band 1
  cout << "3. Drop 3,3,1 into the upper right corner of band 1\n";
  ProcessMosaic m3;
  m3.SetTrackFlag(true);
  m3.SetCreateFlag(true);
  m3.SetImageOverlay(ProcessMosaic::PlaceImagesBeneath);

  m3.SetInputCube("FROM", 3, 3, 1, 10, 1, 1);

  p.SetOutputCube("TO", 5, 5, 2);
  p.EndProcess();
  m3.SetOutputCube("TO");

  m3.StartProcess(5, 1, 1);
  m3.EndProcess();

  TestIn(3, 3, 1, 5, 5, 1);
  TestOut(5, 1, 1, ProcessMosaic::PlaceImagesBeneath, 2);

  remove("isisMosaic_01.cub");
  cout << "*************************************************************************************\n";

  // ***********************************************************
  // Drop the first 3x3x1  the upper left corner
  cout << "4. Drop the first 3x3x1 to the upper left corner\n";
  ProcessMosaic m4;
  m4.SetTrackFlag(true);
  m4.SetCreateFlag(true);
  m4.SetImageOverlay(ProcessMosaic::PlaceImagesBeneath);

  m4.SetInputCube("FROM", 1, 1, 1, 3, 3, 1);

  p.SetOutputCube("TO", 5, 5, 2);
  p.EndProcess();
  m4.SetOutputCube("TO");

  m4.StartProcess(1, 1, 1);
  m4.EndProcess();

  TestIn(1, 1, 1, 3, 3, 1);
  TestOut(1, 1, 1, ProcessMosaic::PlaceImagesBeneath, 2);

  cout << "************************************************************************************************\n";

  // Test for mosaic(beneath)  priority
  cout << "5. Test for mosaic priority with existing mosaic\n";
  ProcessMosaic m5;
  m5.SetImageOverlay(ProcessMosaic::PlaceImagesBeneath);

  m5.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);

  m5.SetOutputCube("TO");

  m5.StartProcess(1, 2, 1);
  m5.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 2, 1, ProcessMosaic::PlaceImagesBeneath, 2);

  cout << "************************************************************************************************\n";


  // ***********************************************************
  // Test for band priority using Keywords for band id
  cout << "6. Test for band priority with Keyname \"FilterName\" and value \"Red\" with Criteria \"Greater\" than in an existing mosaic\n";
  ProcessMosaic m6;
  m6.SetTrackFlag(true);
  m6.SetCreateFlag(true);
  m6.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m6.SetBandKeyword("FilterName", "red");
  m6.SetBandUseMaxValue(true);

  m6.SetInputCube("FROM", 3, 3, 1, 10, 1, 1);

  //p.SetOutputCube("TO", 5, 5, 3);
  //p.EndProcess();
  m6.SetOutputCube("TO");

  m6.StartProcess(1, 1, 1);
  m6.EndProcess();

  TestIn(3, 3, 1, 10, 1, 1);
  TestOut(1, 1, 1, ProcessMosaic::UseBandPlacementCriteria, 2);

  cout << "************************************************************************************************\n";

  cout << "7. Test for band priority for existing mosaic with Keyname \"OriginalBand\" and value \"1\" and Criteria \"Lesser\" than\n";
  ProcessMosaic m7;
  m7.SetTrackFlag(true);
  m7.SetCreateFlag(false);
  m7.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m7.SetBandKeyword("OriginalBand", "1");
  m7.SetBandUseMaxValue(false);
  m7.SetHighSaturationFlag(false);
  m7.SetLowSaturationFlag(false);
  m7.SetNullFlag(false);

  m7.SetInputCube("FROM", 1, 1, 1, 10, 1, 1);
  m7.SetOutputCube("TO");

  m7.StartProcess(1, 1, 1);
  m7.EndProcess();

  TestIn(1, 1, 1, 10, 1, 1);
  TestOut(1, 1, 1, ProcessMosaic::UseBandPlacementCriteria, 2);

  //remove("isisMosaic_01.cub");
  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Test for band priority using Band Number
  cout << "8. Test for band priority with BandNumber set\n";
  ProcessMosaic m8;
  m8.SetTrackFlag(true);
  m8.SetCreateFlag(false);
  m8.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m8.SetBandNumber(1);
  m8.SetBandUseMaxValue(false);
  m8.SetHighSaturationFlag(true);
  m8.SetLowSaturationFlag(false);
  m8.SetNullFlag(false);

  m8.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m8.SetOutputCube("TO");

  m8.StartProcess(1, 3, 1);
  m8.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 3, 1, ProcessMosaic::UseBandPlacementCriteria, 2);

  cout << "************************************************************************************************\n";

  // ***********************************************************
  // Test for HS value set with existing mosaic
  cout << "9. Test for Null flag set with existing mosaic\n";
  ProcessMosaic m9;
  m9.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m9.SetBandNumber(1);
  m9.SetBandUseMaxValue(true);
  m9.SetHighSaturationFlag(false);
  m9.SetLowSaturationFlag(false);
  m9.SetNullFlag(true);

  m9.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m9.SetOutputCube("TO");

  m9.StartProcess(1, 2, 1);
  m9.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 2, 1, ProcessMosaic::UseBandPlacementCriteria, 2);

  //remove("isisMosaic_01.cub");

  // ***********************************************************
  // Test Average Priority
  cout << "\n10. Test Average Priority" << endl;
  // Create the default output cube
  p.SetOutputCube("TO_AVG", 5, 5, 2);
  p.EndProcess();

  ProcessMosaic m10;
  m10.SetTrackFlag(false);
  m10.SetImageOverlay(ProcessMosaic::AverageImageWithMosaic);
  m10.SetCreateFlag(true);

  m10.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m10.SetOutputCube("TO_AVG");
  m10.StartProcess(1, 1, 1);
  m10.EndProcess();
  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 1, 1, ProcessMosaic::AverageImageWithMosaic, 0);

  m10.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m10.SetOutputCube("TO_AVG");
  m10.SetTrackFlag(false);
  m10.SetImageOverlay(ProcessMosaic::AverageImageWithMosaic);
  m10.SetCreateFlag(false);

  m10.StartProcess(-1, -1, 1);
  m10.EndProcess();

  TestOut(1, 1, 1, ProcessMosaic::AverageImageWithMosaic, 0);
  remove("isisMosaic_02.cub");

  cout << "****** End Average **********************\n";

 // Test for band priority using Band Number
  cout << "11. Test for band priority with Tracking Off and BandNumber set\n";
  ProcessMosaic m11;
  m11.SetTrackFlag(false);
  m11.SetCreateFlag(true);
  m11.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m11.SetBandNumber(1);
  m11.SetBandUseMaxValue(false);

  m11.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);

  p.SetOutputCube("TO", 5, 5, 2);
  p.EndProcess();
  m11.SetOutputCube("TO");

  m11.StartProcess(1, 3, 1);
  m11.EndProcess();

  TestIn(1, 1, 1, 5, 5, 1);
  TestOut(1, 3, 1, ProcessMosaic::UseBandPlacementCriteria, 0);

  ProcessMosaic m12;
  m12.SetTrackFlag(false);
  m12.SetCreateFlag(true);
  m12.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
  m12.SetBandNumber(1);
  m12.SetBandUseMaxValue(false);

  m12.SetInputCube("FROM", 1, 1, 1, 5, 5, 1);
  m12.SetOutputCube("TO");
  m12.StartProcess(1, 1, 1);
  m12.EndProcess();

  TestOut(1, 1, 1, ProcessMosaic::UseBandPlacementCriteria, 0);

  cout << "********* Test imagePositions() ********" << endl;
  for (int i = 0; i <= m11.imagePositions().groups() - 1; i++) {
    cout << "Name: " << m11.imagePositions().group(i).name() << endl;
    cout << "File: " << m11.imagePositions().group(i).findKeyword("File")[0] << endl;
    cout << "StartSample: " << m11.imagePositions().group(i).findKeyword("StartSample")[0] << endl;
    cout << "StartLine: " << m11.imagePositions().group(i).findKeyword("StartLine")[0] << endl;
  }
  cout << "*************************************************************************************\n";

  // ***********************************************************
  // Testing Errors

  // Try to open two input cubes
  cout << "\n*** Test Error Handling ***\n";
  cout << "Test multiple input error" << endl;
  try {
    ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Try to open two output cubes
  cout << "Test multiple output error" << endl;
  try {
    ProcessMosaic m;
    m.SetOutputCube("TO");
    m.SetOutputCube("TO");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Drop the input completly outside the output
  cout << "Test input does not overlap mosaic" << endl;
  try {
    ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetOutputCube("TO");
    m.StartProcess(-20, 0, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  cout << "Test input does not overlap mosaic" << endl;
  try {
    ProcessMosaic m;
    m.SetInputCube("FROM");
    m.SetOutputCube("TO");
    m.StartProcess(54, 23, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Don't open an input cube
  cout << "Test no input cube" << endl;
  try {
    ProcessMosaic m;
    m.SetOutputCube("TO");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  //***********************************************************
  // Don't open an output cube
  cout << "Test no output cube" << endl;
  try {
    ProcessMosaic m;
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  //***********************************************************
  // Band cannot be a priority if Track is not set
  cout << "Test Band cannot be a priority if Track is not set" << endl;
  try {
    ProcessMosaic m;
    m.SetTrackFlag(false);
    m.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
    m.SetBandNumber(1);

    m.SetOutputCube("TO");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // ***********************************************************
  // Test Band not found with Band as Priority
  cout << "Test Band not found with Band as Priority" << endl;
  try {
    ProcessMosaic m;
    m.SetTrackFlag(true);
    m.SetImageOverlay(ProcessMosaic::UseBandPlacementCriteria);
    m.SetBandNumber(10);

    m.SetOutputCube("TO");
    m.SetInputCube("FROM");
    m.StartProcess(1, 1, 1);
    m.EndProcess();
  }
  catch (IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }
  remove("isisMosaic_01.cub");
}


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
  Cube cInCube;
  UserInterface &ui = Isis::Application::GetUserInterface();
  QString sFrom = ui.GetFileName("FROM");
  cInCube.open(sFrom);

  cout << "\n***  Input Image  ***  ";
  if (ins == 0) ins = cInCube.sampleCount() - iss + 1;
  if (inl == 0) inl = cInCube.lineCount()   - isl + 1;
  if (inb == 0) inb = cInCube.bandCount()   - isb + 1;

  printf("Stats %d, %d, %d, %d, %d, %d\n", isl, iss, isb, inl, ins, inb);

  int iS;
  Portal ciPortal(ins, 1, cInCube.pixelType());
  for (int band = isb; band <= (isb + inb - 1); band++) {
    for (int line = isl; line <= (isl + inl - 1); line++) {
      iS = iss;
      ciPortal.SetPosition(iss, line, band);  //sample, line, band position
      cInCube.read(ciPortal);
      for (int iPixel = 0; iPixel < ciPortal.size(); iPixel++) {
        if (iPixel == 5) {
          cout << endl;
        }
        printf("(%d,%d,%d)=%-11d  ", line, iS++, band, (int)ciPortal[iPixel]);
      }
      cout << "\n";
    }
    cout << "\n";
  }
  cInCube.close();
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
void TestOut(int piSamples, int piLines,
             int piBands, int piPriority, int originBand) {
  Cube cOutCube;
  UserInterface &ui = Isis::Application::GetUserInterface();
  QString sTo;
  if (piPriority == ProcessMosaic::AverageImageWithMosaic)
    sTo = ui.GetFileName("TO_AVG");
  else
    sTo = ui.GetFileName("TO");
  cOutCube.open(sTo);

  int iBands = cOutCube.bandCount();

  cout << "\n***  Mosaic Image  ***  ";
  printf("Start Stats %d, %d, %d\nTotal Bands=%d\n", piLines, piSamples, piBands, iBands);
  Portal coPortal(5, 1, cOutCube.pixelType());
  int band = piBands;
  while (band <= iBands) {
    for (int line = 1; line <= 5; line++) {
      coPortal.SetPosition(1, line, band);  //sample, line, band position
      cOutCube.read(coPortal);
      for (int iPixel = 0; iPixel < coPortal.size(); iPixel++) {
        int iDefault = 0;
        int iFileIndexOffset = 0;

        switch (SizeOf(cOutCube.pixelType())) {
          case 1:
            iDefault = NULL1;
            iFileIndexOffset = -VALID_MIN1;
            break;

          case 2:
            iDefault = NULL2;
            iFileIndexOffset = -VALID_MIN2;
            break;

          case 4:
            iDefault = INULL4;
            iFileIndexOffset = -ProcessMosaic::FLOAT_STORE_INT_PRECISELY_MIN_VALUE;
            break;
        }

        int iFileIndex = 0;
        if (piPriority != ProcessMosaic::AverageImageWithMosaic && band == originBand &&
           coPortal[iPixel] != iDefault) {
          iFileIndex = (int)coPortal[iPixel] + iFileIndexOffset + 1;
        }
        if (band == originBand && piPriority != ProcessMosaic::AverageImageWithMosaic) //origin band
          printf("(%d,%d,%d)=%-9d,%-1d  ", line, (iPixel + 1), band, (int)coPortal[iPixel],
                 iFileIndex);
        else
          printf("(%d,%d,%d)=%-11d  ", line, (iPixel + 1), band, (int)coPortal[iPixel]);
      }
      cout << "\n";
    }
    cout << "\n";
    band++;
    if (band > iBands) {
      break;
    }
  }
  cOutCube.close();
}
