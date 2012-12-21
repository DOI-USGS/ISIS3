#include "Isis.h"

#include <fstream>
#include <vector>

#include "Histogram.h"
#include "iTime.h"
#include "IString.h"
#include "JP2Encoder.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "PixelType.h"
#include "ProcessExportPds.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "PvlFormatPds.h"
#include "PvlTranslationManager.h"

using namespace Isis;
using namespace std;

double *g_min, *g_max;

PixelType g_oType;
char **g_jp2buf;
JP2Encoder *g_jp2Encoder;
int g_jp2ns;
int g_jp2nb;
int g_jp2band;

void writeJP2Image(Buffer &in);

void IsisMain() {

  QString projName;

  Process pHist;
  Cube *icube = pHist.SetInputCube("FROM");

  // Check to see if the input cube looks like a HiRISE RDR
  if (icube->getBandCount() > 3) {
    QString msg = "Input file [" +
                 Application::GetUserInterface().GetFileName("FROM") +
                 "] does not appear to be a HiRISE RDR product. Number of " +
                 "bands is greater than 3";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  // Setup to get a histogram for each band
  g_min = new double[icube->getBandCount()];
  g_max = new double[icube->getBandCount()];

  UserInterface &ui = Application::GetUserInterface();

  // Determine if the data is to be converted to JPEG2000
  IString enctype = ui.GetString("ENCODING_TYPE");
  enctype.DownCase();

  for (int band = 1; band <= icube->getBandCount(); ++band) {

    if (ui.GetString("TYPE").compare("AUTOMATIC") == 0) {
      // Set up a histogram for this band. This call sets the input range
      // by making an initial stats pass to find the data min and max
      Histogram hist(*icube, band, pHist.Progress());

      // Loop and accumulate histogram
      pHist.Progress()->SetText("Gathering Histogram");
      pHist.Progress()->SetMaximumSteps(icube->getLineCount());
      pHist.Progress()->CheckStatus();
      LineManager line(*icube);
      for (int i = 1; i <= icube->getLineCount(); i++) {
        line.SetLine(i, band);
        icube->read(line);
        hist.AddData(line.DoubleBuffer(), line.size());
        pHist.Progress()->CheckStatus();
      }

      // get the requested cumulative percentages
      g_min[band-1] = ui.GetDouble("MINPER") == 0.0 ? hist.Minimum() : hist.Percent(ui.GetDouble("MINPER"));
      g_max[band-1] = ui.GetDouble("MAXPER") == 100.0 ? hist.Maximum() : hist.Percent(ui.GetDouble("MAXPER"));
    }
    else {
      g_min[band-1] = ui.GetDouble("MIN");
      g_max[band-1] = ui.GetDouble("MAX");
    }
  }

  // Find the minimum min and maximum max for all bands
  double minmin = g_min[0];
  double maxmax = g_max[0];
  for (int band = 1; band < icube->getBandCount(); ++band) {
    if (g_min[band] < minmin) minmin = g_min[band];
    if (g_max[band] > maxmax) maxmax = g_max[band];
  }

  pHist.EndProcess();

  // Set up for writing the data to a PDS formatted file
  ProcessExportPds p;
  Cube *icube2 = p.SetInputCube("FROM");

  if (enctype.Equal("jp2")) {
    g_jp2buf = new char* [icube2->getBandCount()];
    FileName lblFile(ui.GetFileName("TO"));
    QString lblFileName = lblFile.path() + "/" + lblFile.baseName() + ".lbl";
    p.SetDetached(lblFileName);
    p.SetFormat(ProcessExport::JP2);
  }

  // Set the output pixel type and the special pixel values
  int nbits = ui.GetInteger("BITS");
  if (nbits == 8) {
    if (enctype.Equal("jp2")) {
      for (int i = 0; i < icube2->getBandCount(); i++) {
        g_jp2buf[i] = new char[icube2->getSampleCount()];
      }
    }
    g_oType = Isis::UnsignedByte;
    p.SetOutputType(g_oType);
    p.SetOutputRange(VALID_MIN1, VALID_MAX1);
    p.SetOutputNull(NULL1);
    p.SetOutputLis(LOW_INSTR_SAT1);
    p.SetOutputLrs(LOW_REPR_SAT1);
    p.SetOutputHis(HIGH_INSTR_SAT1);
    p.SetOutputHrs(HIGH_REPR_SAT1);
  }
  else if (nbits == 16) {
    if (enctype.Equal("jp2")) {
      for (int i = 0; i < icube2->getBandCount(); i++) {
        g_jp2buf[i] = new char[icube2->getSampleCount()*2];
      }
    }
    g_oType = UnsignedWord;
    p.SetOutputType(g_oType);
    p.SetOutputRange(VALID_MINU2, VALID_MAXU2);
    p.SetOutputNull(NULLU2);
    p.SetOutputLis(LOW_INSTR_SATU2);
    p.SetOutputLrs(LOW_REPR_SATU2);
    p.SetOutputHis(HIGH_INSTR_SATU2);
    p.SetOutputHrs(HIGH_REPR_SATU2);
  }
  else {
    if (enctype.Equal("jp2")) {
      for (int i = 0; i < icube2->getBandCount(); i++) {
        g_jp2buf[i] = new char[icube2->getSampleCount()*2];
      }
    }
    g_oType = UnsignedWord;
    p.SetOutputType(g_oType);
    p.SetOutputRange(3.0, pow(2.0, (double)(nbits)) - 1.0 - 2.0);
    p.SetOutputNull(0);
    p.SetOutputLrs(1);
    p.SetOutputLis(2);
    p.SetOutputHis(pow(2.0, (double)(nbits)) - 1.0 - 1.0);
    p.SetOutputHrs(pow(2.0, (double)(nbits)) - 1.0);
  }
  p.SetOutputEndian(Isis::Msb);
  p.SetInputRange(minmin, maxmax);

  // Get the PDS label from the process
  ProcessExportPds::PdsFileType type;
  if (enctype.Equal("jp2")) {
    type = ProcessExportPds::JP2Image;
  }
  else {
    type = ProcessExportPds::Image;
  }
  Pvl &pdsLabel = p.StandardPdsLabel(type);

  // Translate the keywords from the input cube label that go in the PDS label
  PvlTranslationManager cubeLab(*(icube2->getLabel()),
                                "$mro/translations/hirisePdsRdrCubeLabel.trn");
  cubeLab.Auto(pdsLabel);

  // Translate the keywords from the original EDR PDS label that go in
  // this RDR PDS label
  OriginalLabel origBlob;
  icube2->read(origBlob);
  Pvl origLabel;
  PvlObject origLabelObj = origBlob.ReturnLabels();
  origLabelObj.SetName("OriginalLabelObject");
  origLabel.AddObject(origLabelObj);
  PvlTranslationManager orig(origLabel,
                             "$mro/translations/hirisePdsRdrOriginalLabel.trn");
  orig.Auto(pdsLabel);

  // Add labels to the PDS product that could not be handled by the translater

  if (ui.WasEntered("RATIONALE_DESC")) {
    pdsLabel.AddKeyword(
        PvlKeyword("RATIONALE_DESC", ui.GetString("RATIONALE_DESC")),
        Pvl::Replace);
  }

  // Add PRODUCT_CREATION_TIME
  time_t startTime = time(NULL);
  struct tm *tmbuf = gmtime(&startTime);
  char timestr[80];
  strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
  QString dateTime = (QString) timestr;
  iTime tmpDateTime(dateTime);
  PvlGroup &timeParam = pdsLabel.FindGroup("TIME_PARAMETERS");
  timeParam += PvlKeyword("PRODUCT_CREATION_TIME", tmpDateTime.UTC());

  // Add the N/A constant keyword to the ROOT
  pdsLabel += PvlKeyword("NOT_APPLICABLE_CONSTANT", toString(-9998));

  // Add SOFTWARE_NAME to the ROOT
  QString sfname;
  sfname.clear();
  sfname += "Isis " + Application::Version() + " " +
            Application::GetUserInterface().ProgramName();
  pdsLabel += PvlKeyword("SOFTWARE_NAME", sfname);

  // Add the PRODUCT_VERSION_ID from the user parameter VERSION
  pdsLabel += PvlKeyword("PRODUCT_VERSION_ID", ui.GetString("VERSION"));

  // Add MRO:CCD_FLAG, MRO:BINNING, MRO:TDI
  // As pulled from the input Isis cube, the values are in CPMM order, so
  // convert them to CCD order
  PvlKeyword ccdFlag("MRO:CCD_FLAG");
  PvlKeyword &cpmmFlag = origLabel.FindObject("OriginalLabelObject").
                         FindGroup("INSTRUMENT_SETTING_PARAMETERS").
                         FindKeyword("MRO:POWERED_CPMM_FLAG");
  PvlKeyword ccdBin("MRO:BINNING");
  PvlKeyword &cpmmBin = icube2->getLabel()->FindObject("IsisCube").
                        FindGroup("Mosaic")["cpmmSummingFlag"];
  PvlKeyword ccdTdi("MRO:TDI");
  PvlKeyword &cpmmTdi = icube2->getLabel()->FindObject("IsisCube").
                        FindGroup("Mosaic")["cpmmTdiFlag"];
  PvlKeyword ccdSpecial("MRO:SPECIAL_PROCESSING_FLAG");
  PvlKeyword &cpmmSpecial = icube2->getLabel()->FindObject("IsisCube").
                            FindGroup("Mosaic")["SpecialProcessingFlag"];
  for (int ccd = 0; ccd < 14; ++ccd) {
    const unsigned int cpmmByCcd[] = {0, 1, 2, 3, 5, 8, 10,
                                      11, 12, 13, 6, 7, 4, 9};
    ccdFlag.AddValue(cpmmFlag[cpmmByCcd[ccd]]);
    ccdBin.AddValue(cpmmBin[cpmmByCcd[ccd]] != "Null" ? cpmmBin[cpmmByCcd[ccd]] : "-9998");
    ccdTdi.AddValue(cpmmTdi[cpmmByCcd[ccd]] != "Null" ? cpmmTdi[cpmmByCcd[ccd]] : "-9998");
    IString tmp = cpmmSpecial[cpmmByCcd[ccd]];
    tmp.Trim("\"");
    ccdSpecial.AddValue(tmp.ToQt());
  }

  if (!pdsLabel.HasGroup("INSTRUMENT_SETTING_PARAMETERS")) {
    pdsLabel.AddGroup(PvlGroup("INSTRUMENT_SETTING_PARAMETERS"));
  }
  pdsLabel.FindGroup("INSTRUMENT_SETTING_PARAMETERS") += ccdFlag;
  pdsLabel.FindGroup("INSTRUMENT_SETTING_PARAMETERS") += ccdBin;
  pdsLabel.FindGroup("INSTRUMENT_SETTING_PARAMETERS") += ccdTdi;
  pdsLabel.FindGroup("INSTRUMENT_SETTING_PARAMETERS") += ccdSpecial;

  // Add/modify projection info if there is a projection
  if (pdsLabel.HasObject("IMAGE_MAP_PROJECTION")) {
    PvlObject &mapObject = pdsLabel.FindObject("IMAGE_MAP_PROJECTION");
    mapObject += PvlKeyword("^DATA_SET_MAP_PROJECTION", "DSMAP.CAT");

    // Add the HiRISE comment to the CENTER_LATITUDE keyword
    PvlKeyword &clat = mapObject["CENTER_LATITUDE"];
    clat.AddComment("/* NOTE:  CENTER_LATITUDE and CENTER_LONGITUDE describe the location  */");
    clat.AddComment("/* of the center of projection, which is not necessarily equal to the */");
    clat.AddComment("/* location of the center point of the image.                         */");

    if (mapObject.HasKeyword("CENTER_LATITUDE")) {
      PvlKeyword &centerLat = mapObject["CENTER_LATITUDE"];
      // if (centerLat[0] == "N/A") centerLat = -9998;
      if (centerLat[0] == "N/A") mapObject.DeleteKeyword("CENTER_LATITUDE");
    }
    if (mapObject.HasKeyword("CENTER_LONGITUDE")) {
      PvlKeyword &centerLon = mapObject["CENTER_LONGITUDE"];
      // if (centerLon[0] == "N/A") centerLon = -9998;
      if (centerLon[0] == "N/A") mapObject.DeleteKeyword("CENTER_LONGITUDE");
    }
    if (mapObject.HasKeyword("REFERENCE_LATITUDE")) {
      PvlKeyword &refLat = mapObject["REFERENCE_LATITUDE"];
      // if (refLat[0] == "N/A") refLat = -9998;
      if (refLat[0] == "N/A") mapObject.DeleteKeyword("REFERENCE_LATITUDE");
    }
    if (mapObject.HasKeyword("REFERENCE_LONGITUE")) {
      PvlKeyword &refLon = mapObject["REFERENCE_LONGITUDE"];
      // if (refLon[0] == "N/A") refLon = -9998;
      if (refLon[0] == "N/A") mapObject.DeleteKeyword("REFERENCE_LONGITUDE");
    }
    if (mapObject.HasKeyword("FIRST_STANDARD_PARALLEL")) {
      PvlKeyword &firstSP = mapObject["FIRST_STANDARD_PARALLEL"];
      // if (firstSP[0] == "N/A") firstSP = -9998;
      if (firstSP[0] == "N/A") mapObject.DeleteKeyword("FIRST_STANDARD_PARALLEL");
    }
    if (mapObject.HasKeyword("SECOND_STANDARD_PARALLEL")) {
      PvlKeyword &secondSP = mapObject["SECOND_STANDARD_PARALLEL"];
      // if (secondSP[0] == "N/A") secondSP = -9998;
      if (secondSP[0] == "N/A") mapObject.DeleteKeyword("SECOND_STANDARD_PARALLEL");
    }

    // For Equirectangular ONLY
    // Modify the radii in the pds label to use the radius at the center latitude
    // instead of the target radii from NAIF
    if (mapObject["MAP_PROJECTION_TYPE"][0] == "EQUIRECTANGULAR") {
      Projection *proj = ProjectionFactory::CreateFromCube(*icube2);
      PvlGroup &mapping = icube2->getLabel()->FindGroup("MAPPING", Pvl::Traverse);
      double radius = proj->LocalRadius((double)mapping["CenterLatitude"]) / 1000.0;
      mapObject["A_AXIS_RADIUS"].SetValue(toString(radius), "KM");
      mapObject["B_AXIS_RADIUS"].SetValue(toString(radius), "KM");
      mapObject["C_AXIS_RADIUS"].SetValue(toString(radius), "KM");
    }

    projName = mapObject["MAP_PROJECTION_TYPE"][0];
  }

  // Calculate the min/max per band keywords
  // These come from the input real DN and are converted to the PDS file DN
  // The input to output mapping is opposite from the one above
  double slope = (p.GetOutputMaximum() - p.GetOutputMinimum()) / (maxmax - minmin);
  double intercept = p.GetOutputMaximum() - slope * maxmax;
  PvlKeyword minimum("MRO:MINIMUM_STRETCH", toString(slope * g_min[0] + intercept));
  PvlKeyword maximum("MRO:MAXIMUM_STRETCH", toString(slope * g_max[0] + intercept));
  for (int band = 1; band < icube2->getBandCount(); ++band) {
    minimum += toString(slope * g_min[band] + intercept);
    maximum += toString(slope * g_max[band] + intercept);
  }

  if (enctype.Equal("jp2")) {
    // Add keywords to the PDS JP2 IMAGE object
    PvlObject &imagejp2 = pdsLabel.FindObject("UNCOMPRESSED_FILE").FindObject("IMAGE");

    // Add the HiRISE specific description of the IMAGE object
    imagejp2 += PvlKeyword("DESCRIPTION", "HiRISE projected and mosaicked product");

    // Add the SCALLING_FACTOR and OFFSET keywords
    imagejp2.AddKeyword(PvlKeyword("SCALING_FACTOR", toString(slope)), Pvl::Replace);
    imagejp2.AddKeyword(PvlKeyword("OFFSET", toString(intercept)), Pvl::Replace);

    // Reformat some keyword units in the image object
    // This is lame, but PDS units are difficult to work with, so for now???
    PvlKeyword &oldFilterNamejp2 = imagejp2["FILTER_NAME"];
    PvlKeyword newFilterName("FILTER_NAME");
    for (int val = 0; val < oldFilterNamejp2.Size(); ++val) {
      QString  filtname(oldFilterNamejp2[val].toUpper());
      if (filtname == "BLUEGREEN") filtname = "BLUE-GREEN";
      else if (filtname == "NEARINFRARED") filtname = "NEAR-INFRARED";
      newFilterName.AddValue(filtname);
    }
    imagejp2.AddKeyword(newFilterName, Pvl::Replace);

    PvlKeyword &oldCenterjp2 = imagejp2["CENTER_FILTER_WAVELENGTH"];
    PvlKeyword newCenter("CENTER_FILTER_WAVELENGTH");
    for (int val = 0; val < oldCenterjp2.Size(); ++val) {
      if (((IString)(oldCenterjp2.Unit(val))).UpCase() == "NANOMETERS") {
        newCenter.AddValue(oldCenterjp2[val], "NM");
      }
      else {
        newCenter.AddValue(oldCenterjp2[val], oldCenterjp2.Unit(val));
      }
    }
    imagejp2.AddKeyword(newCenter, Pvl::Replace);

    PvlKeyword &oldBandWidthjp2 = imagejp2["BAND_WIDTH"];
    PvlKeyword newBandWidth("BAND_WIDTH");
    for (int val = 0; val < oldBandWidthjp2.Size(); ++val) {
      if (((IString)(oldBandWidthjp2.Unit(val))).UpCase() == "NANOMETERS") {
        newBandWidth.AddValue(oldBandWidthjp2[val], "nm");
      }
      else {
        newBandWidth.AddValue(oldBandWidthjp2[val], oldBandWidthjp2.Unit(val));
      }
    }
    imagejp2.AddKeyword(newBandWidth, Pvl::Replace);

    // Add the min/max per band keywords
    imagejp2 += minimum;
    imagejp2 += maximum;

    // Modify the default SAMPLE_BIT_MASK keyword placed there by the
    // ProcessExportPds
    if (nbits != 8 && nbits != 16) {
      imagejp2.AddKeyword(PvlKeyword("SAMPLE_BIT_MASK",
                                     toString((int)pow(2.0, (double)ui.GetInteger("BITS")) - 1)),
                          Pvl::Replace);
    }
  }
  else {
    // Add keywords to the PDS IMAGE object
    PvlObject &image = pdsLabel.FindObject("IMAGE");

    // Add the HiRISE specific description of the IMAGE object
    image += PvlKeyword("DESCRIPTION", "HiRISE projected and mosaicked product");

    /**
     *  Calculate the SCALING_FACTOR and OFFSET keywords
     *  Set these so the unsigned 16bit PDS disk values can be converted back
     *  to the correct values Isis had
     *  These keywords are used to map stored/disk values to the correct values so,
     *  the input(x axis) values are the unsigned Xbit values from the PDS file
     */
    // ??? unneccessary calculation - this is done by ProcessExportPds class.
    double slope = (maxmax - minmin) / (p.GetOutputMaximum() - p.GetOutputMinimum()); 
    double intercept = maxmax - slope * p.GetOutputMaximum();
    image.AddKeyword(PvlKeyword("SCALING_FACTOR", toString(slope)), Pvl::Replace);
    image.AddKeyword(PvlKeyword("OFFSET", toString(intercept)), Pvl::Replace);

    // Reformat some keyword units in the image object
    // This is lame, but PDS units are difficult to work with, so for now
    PvlKeyword &oldFilterName = image["FILTER_NAME"];
    PvlKeyword newFilterName("FILTER_NAME");
    for (int val = 0; val < oldFilterName.Size(); ++val) {
      QString  filtname(oldFilterName[val].toUpper());
      if (filtname == "BLUEGREEN") filtname = "BLUE-GREEN";
      else if (filtname == "NEARINFRARED") filtname = "NEAR-INFRARED";
      newFilterName.AddValue(filtname);
    }
    image.AddKeyword(newFilterName, Pvl::Replace);

    PvlKeyword &oldCenter = image["CENTER_FILTER_WAVELENGTH"];
    PvlKeyword newCenter("CENTER_FILTER_WAVELENGTH");
    for (int val = 0; val < oldCenter.Size(); ++val) {
      if (((IString)(oldCenter.Unit(val))).UpCase() == "NANOMETERS") {
        newCenter.AddValue(oldCenter[val], "NM");
      }
      else {
        newCenter.AddValue(oldCenter[val], oldCenter.Unit(val));
      }
    }
    image.AddKeyword(newCenter, Pvl::Replace);

    PvlKeyword &oldBandWidth = image["BAND_WIDTH"];
    PvlKeyword newBandWidth("BAND_WIDTH");
    for (int val = 0; val < oldBandWidth.Size(); ++val) {
      if (((IString)(oldBandWidth.Unit(val))).UpCase() == "NANOMETERS") {
        newBandWidth.AddValue(oldBandWidth[val], "NM");
      }
      else {
        newBandWidth.AddValue(oldBandWidth[val], oldBandWidth.Unit(val));
      }
    }
    image.AddKeyword(newBandWidth, Pvl::Replace);

    // Add the min/max per band keywords
    image += minimum;
    image += maximum;

    // Modify the default SAMPLE_BIT_MASK keyword placed there by the
    // ProcessExportPds
    if (nbits != 8 && nbits != 16) {
      image.AddKeyword(PvlKeyword("SAMPLE_BIT_MASK",
                                  toString((int)pow(2.0, (double)ui.GetInteger("BITS")) - 1)),
                       Pvl::Replace);
    }
  }

  // Modify the units in the viewing_parameters group
//  if (pdsLabel.HasGroup("VIEWING_PARAMETERS")) {
//    PvlGroup &viewGroup = pdsLabel.FindGroup("VIEWING_PARAMETERS");

//    PvlKeyword &incidence = viewGroup["INCIDENCE_ANGLE"];
//    IString tstr = incidence.Unit();
//    if (tstr.UpCase() == "DEG") incidence.SetValue((QString)incidence, "deg");

//    PvlKeyword &emission = viewGroup["EMISSION_ANGLE"];
//    tstr = emission.Unit();
//    if (tstr.UpCase() == "DEG") emission.SetValue((QString)emission, "deg");

//    PvlKeyword &phase = viewGroup["PHASE_ANGLE"];
//    tstr = phase.Unit();
//    if (tstr.UpCase() == "DEG") phase.SetValue((QString)phase, "deg");

//    PvlKeyword &solarLon = viewGroup["SOLAR_LONGITUDE"];
//    tstr = solarLon.Unit();   q
//    if (tstr.UpCase() == "DEG") solarLon.SetValue((QString)solarLon, "deg");

//    PvlKeyword &localTime = viewGroup["LOCAL_TIME"];
//    tstr = localTime.Unit();
//    if (tstr.UpCase() == "LOCALDAY/24") localTime.SetValue((QString)localTime, "local day/24");
//  }

  // Add a keyword type (i.e., QString, bool, int...) file to the PDS label Pvl
  PvlFormat *formatter = pdsLabel.GetFormat();
  formatter->Add("$mro/translations/hirisePdsRdrExtras.typ");

  // Add an output format template (group, object, & keyword output order) to
  // the PDS PVL
  if (projName == "EQUIRECTANGULAR") {
    if (enctype.Equal("jp2")) {
      pdsLabel.SetFormatTemplate("$mro/templates/labels/hirisePdsRdrEquiJP2.pft");
    }
    else {
      pdsLabel.SetFormatTemplate("$mro/templates/labels/hirisePdsRdrEqui.pft");
    }
  }
  else {
    if (enctype.Equal("jp2")) {
      pdsLabel.SetFormatTemplate("$mro/templates/labels/hirisePdsRdrPolarJP2.pft");
    }
    else {
      pdsLabel.SetFormatTemplate("$mro/templates/labels/hirisePdsRdrPolar.pft");
    }
  }

  // Open the output PDS file and dump the label and cube data
  if (enctype.Equal("jp2")) {
    p.OutputDetachedLabel();
    g_jp2Encoder = new JP2Encoder(ui.GetFileName("TO"), icube2->getSampleCount(),
                                 icube2->getLineCount(), icube2->getBandCount(), g_oType);
    g_jp2Encoder->OpenFile();
    g_jp2ns = icube2->getSampleCount();
    g_jp2nb = icube2->getBandCount();
    g_jp2band = 0;
    p.StartProcess(writeJP2Image);
    p.EndProcess();
    delete g_jp2Encoder;
    for (int i = 0; i < icube2->getBandCount(); i++) {
      delete [] g_jp2buf[i];
    }
  }
  else {
    FileName outFile(ui.GetFileName("TO"));
    ofstream oCube(outFile.expanded().toAscii().data());
    p.OutputLabel(oCube);
    p.StartProcess(oCube);
    oCube.close();
    p.EndProcess();
  }

  delete [] g_min;
  delete [] g_max;
}

void writeJP2Image(Buffer &in) {
  for (int i = 0; i < g_jp2ns; i++) {
    if (g_oType == Isis::UnsignedByte) {
      ((unsigned char *)g_jp2buf[g_jp2band])[i] = (unsigned char)((int)in[i]);
    }
    else {
      ((short unsigned int *)g_jp2buf[g_jp2band])[i] = (short unsigned int)((int)in[i]);
    }
  }
  g_jp2band++;
  if (g_jp2band == g_jp2nb) {
    g_jp2band = 0;
    if (g_oType == Isis::UnsignedByte) {
      g_jp2Encoder->Write((unsigned char **)g_jp2buf);
    }
    else {
      g_jp2Encoder->Write((short int **)g_jp2buf);
    }
  }
}
