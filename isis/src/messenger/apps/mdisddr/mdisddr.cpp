#include "Isis.h"

#include <cmath>
#include <QString>
#include <sstream>
#include <fstream>

#include "UserInterface.h"
#include "Progress.h"
#include "FileName.h"
#include "IException.h"
#include "ProcessExportPds.h"
#include "Cube.h"
#include "Histogram.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "PvlTranslationManager.h"
#include "ProgramLauncher.h"
#include "BufferManager.h"
#include "LineManager.h"
#include "CollectorMap.h"
#include "Kernels.h"

using namespace std;
using namespace Isis;


typedef CollectorMap<IString, int> BandMap;

inline QString Quote(const QString &value, const char qChar = '"') {
  if(value.isEmpty()) return (value);
  if(value[0] == qChar) return (value);
  return (QString(qChar + value + qChar));
}

inline double SetRound(double value, const int precision) {
  double scale = pow(10.0, precision);
  value = rint(value * scale) / scale;
  return (value);
}

inline void ValidateUnit(PvlKeyword &key, const QString &kunit) {
  PvlKeyword temp = key;
  key.clear();
  for(int i = 0 ; i < temp.size() ; i++) {
    try {
      //  If this works, check unit, otherwise an exception is thrown
      (void) toDouble(temp[i]);
      QString unit = temp.unit(i);
      if(unit.isEmpty()) unit = kunit;
      key.addValue(temp[i], unit);
    }
    catch(...) {
      key.addValue(temp[i]);
    }
  }
  return;
}

inline void FixUnit(PvlObject &obj, const QString &key, const QString &unit) {
  if(obj.hasKeyword(key, PvlObject::Traverse)) {
    ValidateUnit(obj.findKeyword(key, PvlObject::Traverse), unit);
  }
  return;
}

inline void FixQuotes(PvlContainer &kcont, const QString &value = "N/A") {
  PvlContainer::PvlKeywordIterator kiter;
  for(kiter = kcont.begin() ; kiter != kcont.end() ; ++kiter) {
    for(int nv = 0 ; nv < kiter->size() ; nv++) {
      if((*kiter)[nv] == value)(*kiter)[nv] = Quote((*kiter)[nv]);
    }
  }
}

inline void FixLabels(PvlObject &obj) {
  // Current object-owned keywords
  FixQuotes(obj);

  // Fix all nested objects
  PvlObject::PvlObjectIterator o;
  for(o = obj.beginObject() ; o != obj.endObject() ; ++o) {
    FixLabels(*o);
  }

  // Fix local groups
  PvlObject::PvlGroupIterator g;
  for(g = obj.beginGroup() ; g != obj.endGroup() ; ++g) {
    FixQuotes(*g);
  }
  return;
}

inline void WriteBand(ProcessExportPds &process, ofstream &out,
                      const QString &fname, int band)  {
  process.ClearInputCubes();
  (void) process.SetInputCube(fname, CubeAttributeInput("+" + toString(band)));
  process.StartProcess(out);
  return;
}


void IsisMain() {
  const QString mdisddr_program = "mdisddr";
  const QString mdisddr_version = "1.0";
  const QString mdisddr_revision = "$Revision$";
  const QString mdisddr_runtime = Application::DateTime();
  const QString dataSetID = "MESS-E/V/H-MDIS-6-DDR-GEOMDATA-V1.0";

  UserInterface &ui = Application::GetUserInterface();
  FileName input(ui.GetFileName("FROM"));
  QString to("");
  bool toEntered = ui.WasEntered("TO");
  if(toEntered) {
    to = ui.GetAsString("TO");
  }

  QString opath(".");    // Set default to local directory
  if(ui.WasEntered("OPATH")) {
    opath = ui.GetString("OPATH");
  }
  else {
    ui.PutAsString("OPATH", opath);
  }

  //  Generate the image cube that phocube produces for the DDR data
  FileName phoFile = FileName::createTempFile("$TEMPORARY/" + input.baseName() + "_phocube.cub");
  QString pfile = phoFile.expanded();
  QString parameters = "FROM=" + input.expanded() + " TO=" + pfile +
                      " LATITUDE=TRUE LONGITUDE=TRUE PHASE=TRUE EMISSION=TRUE INCIDENCE=TRUE";
  ProgramLauncher::RunIsisProgram("phocube", parameters);

  //  Wrap a try clause so that if anything goes wrong below, we can remove
  //  the phocube file.
  try {
    Pvl phoLabel(pfile);
    BandMap bandmap;
    PvlKeyword bn = phoLabel.findGroup("BandBin", Pvl::Traverse)["Name"];
    for(int i = 0 ; i < bn.size() ; i++) {
      bandmap.add(bn[i], i + 1);
    }

    // Set up the export.  Note that the attributes selects 5 bands from the
    // output of the phocube run.  It doesn't matter at this time which 5
    // bands it is, just so that we have this established so the right labels
    // and file size is created.
    ProcessExportPds processPds;
    (void) processPds.SetInputCube(pfile, CubeAttributeInput("+1-5"));

    //  Due to the nature of the phocube file, we cannot compute a histogram
    //  of the data (it includes lots of data we don't need).  So we will
    //  fix the range to the expected well defined angle ranges.
    double minmin = 0.0;
    double maxmax = 0.0;
    if(ui.GetString("TYPE").compare("AUTOMATIC") == 0) {
      minmin = -360.0;
      maxmax = 360.0;
    }
    else {
      minmin = ui.GetDouble("MIN");
      maxmax = ui.GetDouble("MAX");
    }

    processPds.SetOutputEndian(Isis::Msb);
    processPds.SetExportType(ProcessExportPds::Fixed);
    processPds.SetInputRange(minmin, maxmax);

    // Set the output pixel type and the special pixel values
    processPds.SetOutputType(Real);
    processPds.SetOutputRange(minmin, maxmax);
    processPds.SetOutputNull(NULL4);
    processPds.SetOutputLrs(LOW_REPR_SAT4);
    processPds.SetOutputLis(LOW_INSTR_SAT4);
    processPds.SetOutputHrs(HIGH_REPR_SAT4);
    processPds.SetOutputHis(HIGH_INSTR_SAT4);

    Progress p;
    p.SetText("Modifying Keywords");
    p.SetMaximumSteps(6);
    p.CheckStatus();

    // Get the PDS label from the process
    Pvl &pdsLabel = processPds.StandardPdsLabel(ProcessExportPds::Image);

    // Translate the keywords from the original EDR PDS label that go in
    // this DDR PDS label.  Note that we have to open the original (FROM)
    // cube as the phocube output goes into the specification of the
    // output PDS file (required for 5 band IMAGE object).
    Cube from;
    from.open(input.expanded());
    OriginalLabel origBlob;
    from.read(origBlob);
    Pvl origLabel;
    PvlObject origLabelObj = origBlob.ReturnLabels();
    origLabelObj.setName("OriginalLabelObject");
    origLabel.addObject(origLabelObj);
    p.CheckStatus();

    // Translates the ISIS labels along with the original EDR labels
    origLabel.addObject(*from.label());
    PvlTranslationManager labels(origLabel,
                                 "$messenger/translations/mdisDDRLabel.trn");
    labels.Auto(pdsLabel);
    p.CheckStatus();

    //  Add any new keywords
    QString lnote = "2007-12-20, S. Murchie (JHU/APL); "
                   "2008-01-02, S. Murchie (JHU/APL); "
                   "2008-01-11, J. Ward (GEO)";
    pdsLabel += PvlKeyword("LABEL_REVISION_NOTE", lnote);
    pdsLabel += PvlKeyword("SPACECRAFT_NAME", Quote("MESSENGER"));

    // Fixes bad keywords
    PvlKeyword &data_set_id = pdsLabel.findKeyword("DATA_SET_ID", Pvl::Traverse);
    data_set_id.setValue(dataSetID);
    QString prodid(input.baseName());
    PvlKeyword &product_id = pdsLabel.findKeyword("PRODUCT_ID", Pvl::Traverse);
    if((product_id.size() == 0) || ((product_id.size() > 0) && (product_id[0] == "N/A"))) {
      product_id.setValue(prodid);
    }
    else {
      QString pid = product_id[0];
      pid[0] = 'D';
      pid.remove(QRegExp("_.*"));
      pid.append("_DE_0");
      product_id.setValue(pid);
      prodid = pid;
    }

    // Now we have enough to establish output file name
    if(!toEntered) to = opath + "/" + prodid;
    FileName output(to);
    output = output.addExtension("IMG");
    if(!toEntered) ui.PutFileName("TO", output.expanded());

    PvlKeyword &product_creation_time = pdsLabel.findKeyword("PRODUCT_CREATION_TIME", Pvl::Traverse);
    product_creation_time.setValue(mdisddr_runtime);

    PvlKeyword &software_name = pdsLabel.findKeyword("SOFTWARE_NAME", Pvl::Traverse);
    software_name.setValue(mdisddr_program);

    PvlKeyword &software_version_id = pdsLabel.findKeyword("SOFTWARE_VERSION_ID", Pvl::Traverse);
    software_version_id.setValue(Quote(mdisddr_version));

    PvlKeyword &filter_number = pdsLabel.findKeyword("FILTER_NUMBER", Pvl::Traverse);
    if((filter_number.size() > 0)) {
      filter_number.setValue(Quote(filter_number[0]));
    }


    // Add quotes
    PvlKeyword &data_quality_id = pdsLabel.findKeyword("DATA_QUALITY_ID", Pvl::Traverse);
    data_quality_id.setValue(Quote(data_quality_id));
    PvlKeyword &sequence_name = pdsLabel.findKeyword("SEQUENCE_NAME", Pvl::Traverse);
    sequence_name.setValue(Quote(sequence_name));
    PvlKeyword &start_count = pdsLabel.findKeyword("SPACECRAFT_CLOCK_START_COUNT", Pvl::Traverse);
    start_count.setValue(Quote(start_count));
    PvlKeyword &stop_count = pdsLabel.findKeyword("SPACECRAFT_CLOCK_STOP_COUNT", Pvl::Traverse);
    stop_count.setValue(Quote(stop_count));

    //  For DDRs, the SOURCE_PRODUCT_ID is made up of SPICE kernels.  I need to
    //  go get em.
    Kernels kernels(from);
    QStringList kfiles = kernels.getKernelList();
    PvlKeyword &source_product_id = pdsLabel.findKeyword("SOURCE_PRODUCT_ID", Pvl::Traverse);
    source_product_id.clear();
    for(int i = 0; i < kfiles.size(); i++) {
      FileName kfile(kfiles[i]);
      source_product_id.addValue(Quote(kfile.name()));
    }

    //  Enforce parentheses for scalars
    if(source_product_id.size() == 1)
      source_product_id.setValue('(' + source_product_id[0] + ')');

    // Removes keywords
    PvlObject imageObject(pdsLabel.findObject("IMAGE"));
    if(imageObject.hasKeyword("CENTER_FILTER_WAVELENGTH")) imageObject.deleteKeyword("CENTER_FILTER_WAVELENGTH");
    if(imageObject.hasKeyword("BANDWIDTH")) imageObject.deleteKeyword("BANDWIDTH");
    if(imageObject.hasKeyword("UNIT")) imageObject.deleteKeyword("UNIT");
    if(imageObject.hasKeyword("DARK_STRIP_MEAN")) imageObject.deleteKeyword("DARK_STRIP_MEAN");
    if(imageObject.hasKeyword("OFFSET")) imageObject.deleteKeyword("OFFSET");
    if(imageObject.hasKeyword("SCALING_FACTOR")) imageObject.deleteKeyword("SCALING_FACTOR");
    if(imageObject.hasKeyword("SAMPLE_BIT_MASK")) imageObject.deleteKeyword("SAMPLE_BIT_MASK");

    // Add band names to image object
    PvlKeyword &bandNames = imageObject.findKeyword("FILTER_NAME");
    bandNames.setName("BAND_NAME");
    bandNames.clear();
    bandNames.addValue("Latitude, planetocentric, deg N");
    bandNames.addValue("Longitude, planetocentric, deg E");
    bandNames.addValue("Incidence angle at equipotential surface, deg");
    bandNames.addValue("Emission angle at equipotential surface, deg");
    bandNames.addValue("Phase angle at equipotential surface, deg");
    pdsLabel.deleteObject("IMAGE");
    pdsLabel.addObject(imageObject);

    p.CheckStatus();

    //  Fix all the hosed units upon ingest.  They are illformed.
    FixUnit(pdsLabel, "RETICLE_POINT_RA", "DEG");
    FixUnit(pdsLabel, "RETICLE_POINT_DECLINATION", "DEG");
    FixUnit(pdsLabel, "RETICLE_POINT_LATITUDE", "DEG");
    FixUnit(pdsLabel, "RETICLE_POINT_LONGITUDE", "DEG");

    //  Now address nested keywords in SUBFRAME groups
    for(int i = 1 ; i <= 5 ; i++) {
      QString n(toString(i));
      QString group = "SUBFRAME" + n + "_PARAMETERS";
      if(pdsLabel.hasGroup(group)) {
        PvlGroup &grp = pdsLabel.findGroup(group);
        ValidateUnit(grp.findKeyword("RETICLE_POINT_LATITUDE"), "DEG");
        ValidateUnit(grp.findKeyword("RETICLE_POINT_LONGITUDE"), "DEG");
      }
    }
    p.CheckStatus();


    //  Finally, fix keywords by Quoting missing N/A values
    FixLabels(pdsLabel);
    p.CheckStatus();

    // All done...write result.
    pdsLabel.setFormatTemplate("$messenger/templates/labels/mdisPdsDDR.pft");
    QString ofile(output.expanded());
    ofstream outstream(ofile.toAscii().data());
    processPds.OutputLabel(outstream);

    // Writing out the 5 bands is a bit tricky for this product.  The bands
    // must be ordered in a specific order, but phocube orders them in a
    // different order.  To make this approach work, determine the proper band
    // as ordered in the phocube output and select the desired bands one at a
    // time setting the input cube to the desired band and writing it out by
    // stream.

    //  Write latitude, longitude, incidence, emission, phase bands
    WriteBand(processPds, outstream, pfile, bandmap.get("Latitude"));
    WriteBand(processPds, outstream, pfile, bandmap.get("Longitude"));
    WriteBand(processPds, outstream, pfile, bandmap.get("Incidence Angle"));
    WriteBand(processPds, outstream, pfile, bandmap.get("Emission Angle"));
    WriteBand(processPds, outstream, pfile, bandmap.get("Phase Angle"));
    outstream.close();
    processPds.EndProcess();
    remove(pfile.toAscii().data());
  }
  catch(IException &) {
    remove(pfile.toAscii().data());
    throw;
  }
  catch(...) {
    remove(pfile.toAscii().data());
    throw IException(IException::Unknown, "Unexpected exception caught!",
                     _FILEINFO_);
  }

}
