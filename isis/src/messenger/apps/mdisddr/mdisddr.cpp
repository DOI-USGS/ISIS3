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
  key.Clear();
  for(int i = 0 ; i < temp.Size() ; i++) {
    try {
      //  If this works, check unit, otherwise an exception is thrown
      (void) toDouble(temp[i]);
      QString unit = temp.Unit(i);
      if(unit.isEmpty()) unit = kunit;
      key.AddValue(temp[i], unit);
    }
    catch(...) {
      key.AddValue(temp[i]);
    }
  }
  return;
}

inline void FixUnit(PvlObject &obj, const QString &key, const QString &unit) {
  if(obj.HasKeyword(key, PvlObject::Traverse)) {
    ValidateUnit(obj.FindKeyword(key, PvlObject::Traverse), unit);
  }
  return;
}

inline void FixQuotes(PvlContainer &kcont, const QString &value = "N/A") {
  PvlContainer::PvlKeywordIterator kiter;
  for(kiter = kcont.Begin() ; kiter != kcont.End() ; ++kiter) {
    for(int nv = 0 ; nv < kiter->Size() ; nv++) {
      if((*kiter)[nv] == value)(*kiter)[nv] = Quote((*kiter)[nv]);
    }
  }
}

inline void FixLabels(PvlObject &obj) {
  // Current object-owned keywords
  FixQuotes(obj);

  // Fix all nested objects
  PvlObject::PvlObjectIterator o;
  for(o = obj.BeginObject() ; o != obj.EndObject() ; ++o) {
    FixLabels(*o);
  }

  // Fix local groups
  PvlObject::PvlGroupIterator g;
  for(g = obj.BeginGroup() ; g != obj.EndGroup() ; ++g) {
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
    PvlKeyword bn = phoLabel.FindGroup("BandBin", Pvl::Traverse)["Name"];
    for(int i = 0 ; i < bn.Size() ; i++) {
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
    origLabelObj.SetName("OriginalLabelObject");
    origLabel.AddObject(origLabelObj);
    p.CheckStatus();

    // Translates the ISIS labels along with the original EDR labels
    origLabel.AddObject(*from.label());
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
    PvlKeyword &data_set_id = pdsLabel.FindKeyword("DATA_SET_ID", Pvl::Traverse);
    data_set_id.SetValue(dataSetID);
    QString prodid(input.baseName());
    PvlKeyword &product_id = pdsLabel.FindKeyword("PRODUCT_ID", Pvl::Traverse);
    if((product_id.Size() == 0) || ((product_id.Size() > 0) && (product_id[0] == "N/A"))) {
      product_id.SetValue(prodid);
    }
    else {
      QString pid = product_id[0];
      pid[0] = 'D';
      pid.remove(QRegExp("_.*"));
      pid.append("_DE_0");
      product_id.SetValue(pid);
      prodid = pid;
    }

    // Now we have enough to establish output file name
    if(!toEntered) to = opath + "/" + prodid;
    FileName output(to);
    output = output.addExtension("IMG");
    if(!toEntered) ui.PutFileName("TO", output.expanded());

    PvlKeyword &product_creation_time = pdsLabel.FindKeyword("PRODUCT_CREATION_TIME", Pvl::Traverse);
    product_creation_time.SetValue(mdisddr_runtime);

    PvlKeyword &software_name = pdsLabel.FindKeyword("SOFTWARE_NAME", Pvl::Traverse);
    software_name.SetValue(mdisddr_program);

    PvlKeyword &software_version_id = pdsLabel.FindKeyword("SOFTWARE_VERSION_ID", Pvl::Traverse);
    software_version_id.SetValue(Quote(mdisddr_version));

    PvlKeyword &filter_number = pdsLabel.FindKeyword("FILTER_NUMBER", Pvl::Traverse);
    if((filter_number.Size() > 0)) {
      filter_number.SetValue(Quote(filter_number[0]));
    }


    // Add quotes
    PvlKeyword &data_quality_id = pdsLabel.FindKeyword("DATA_QUALITY_ID", Pvl::Traverse);
    data_quality_id.SetValue(Quote(data_quality_id));
    PvlKeyword &sequence_name = pdsLabel.FindKeyword("SEQUENCE_NAME", Pvl::Traverse);
    sequence_name.SetValue(Quote(sequence_name));
    PvlKeyword &start_count = pdsLabel.FindKeyword("SPACECRAFT_CLOCK_START_COUNT", Pvl::Traverse);
    start_count.SetValue(Quote(start_count));
    PvlKeyword &stop_count = pdsLabel.FindKeyword("SPACECRAFT_CLOCK_STOP_COUNT", Pvl::Traverse);
    stop_count.SetValue(Quote(stop_count));

    //  For DDRs, the SOURCE_PRODUCT_ID is made up of SPICE kernels.  I need to
    //  go get em.
    Kernels kernels(from);
    QStringList kfiles = kernels.getKernelList();
    PvlKeyword &source_product_id = pdsLabel.FindKeyword("SOURCE_PRODUCT_ID", Pvl::Traverse);
    source_product_id.Clear();
    for(int i = 0; i < kfiles.size(); i++) {
      FileName kfile(kfiles[i]);
      source_product_id.AddValue(Quote(kfile.name()));
    }

    //  Enforce parentheses for scalars
    if(source_product_id.Size() == 1)
      source_product_id.SetValue('(' + source_product_id[0] + ')');

    // Removes keywords
    PvlObject imageObject(pdsLabel.FindObject("IMAGE"));
    if(imageObject.HasKeyword("CENTER_FILTER_WAVELENGTH")) imageObject.DeleteKeyword("CENTER_FILTER_WAVELENGTH");
    if(imageObject.HasKeyword("BANDWIDTH")) imageObject.DeleteKeyword("BANDWIDTH");
    if(imageObject.HasKeyword("UNIT")) imageObject.DeleteKeyword("UNIT");
    if(imageObject.HasKeyword("DARK_STRIP_MEAN")) imageObject.DeleteKeyword("DARK_STRIP_MEAN");
    if(imageObject.HasKeyword("OFFSET")) imageObject.DeleteKeyword("OFFSET");
    if(imageObject.HasKeyword("SCALING_FACTOR")) imageObject.DeleteKeyword("SCALING_FACTOR");
    if(imageObject.HasKeyword("SAMPLE_BIT_MASK")) imageObject.DeleteKeyword("SAMPLE_BIT_MASK");

    // Add band names to image object
    PvlKeyword &bandNames = imageObject.FindKeyword("FILTER_NAME");
    bandNames.SetName("BAND_NAME");
    bandNames.Clear();
    bandNames.AddValue("Latitude, planetocentric, deg N");
    bandNames.AddValue("Longitude, planetocentric, deg E");
    bandNames.AddValue("Incidence angle at equipotential surface, deg");
    bandNames.AddValue("Emission angle at equipotential surface, deg");
    bandNames.AddValue("Phase angle at equipotential surface, deg");
    pdsLabel.DeleteObject("IMAGE");
    pdsLabel.AddObject(imageObject);

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
      if(pdsLabel.HasGroup(group)) {
        PvlGroup &grp = pdsLabel.FindGroup(group);
        ValidateUnit(grp.FindKeyword("RETICLE_POINT_LATITUDE"), "DEG");
        ValidateUnit(grp.FindKeyword("RETICLE_POINT_LONGITUDE"), "DEG");
      }
    }
    p.CheckStatus();


    //  Finally, fix keywords by Quoting missing N/A values
    FixLabels(pdsLabel);
    p.CheckStatus();

    // All done...write result.
    pdsLabel.SetFormatTemplate("$messenger/templates/labels/mdisPdsDDR.pft");
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
