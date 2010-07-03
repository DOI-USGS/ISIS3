#include "Isis.h"

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

#include "UserInterface.h"
#include "Progress.h"
#include "Filename.h"
#include "iException.h"
#include "ProcessExportPds.h"
#include "Cube.h"
#include "Histogram.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "PvlTranslationManager.h"
#include "BufferManager.h"
#include "LineManager.h"

using namespace std; 
using namespace Isis;

inline std::string Quote(const std::string &value, const char qChar = '"') {
   if (value.empty()) return (value);
   if (value[0] == qChar) return (value);
   return (std::string(qChar+value+qChar));
 }

inline double SetRound(double value, const int precision) {
    double scale = pow(10.0, precision);
    value = rint(value * scale) / scale;
    return (value);
}

inline void ValidateUnit(PvlKeyword &key, const std::string &kunit) {
  PvlKeyword temp = key;
  key.Clear();
  for (int i = 0 ; i < temp.Size() ; i++) {
    try {
      //  If this works, check unit, otherwise an exception is thrown
      (void) temp[i].ToDouble();
      string unit = temp.Unit(i);
      if (unit.empty()) unit = kunit;
      key.AddValue(temp[i],unit);
    }
    catch (...) {
      key.AddValue(temp[i]);
    }
  }
  return;
 }

inline void FixUnit(PvlObject &obj, const string &key, const string &unit) {
  if (obj.HasKeyword(key, PvlObject::Traverse)) {
    ValidateUnit(obj.FindKeyword(key, PvlObject::Traverse), unit);
  }
  return;
}

inline void FixQuotes(PvlContainer &kcont, const string &value = "N/A") { 
  PvlContainer::PvlKeywordIterator kiter;
  for (kiter = kcont.Begin() ; kiter != kcont.End() ; ++kiter) {
    for (int nv = 0 ; nv < kiter->Size() ; nv++) {
      if ((*kiter)[nv] == value)  (*kiter)[nv] = Quote((*kiter)[nv]); 
    }
  }
}

inline void FixLabels(PvlObject &obj) {
  // Current object-owned keywords
  FixQuotes(obj);

  // Fix all nested objects
  PvlObject::PvlObjectIterator o;
  for (o = obj.BeginObject() ; o != obj.EndObject() ; ++o) {
    FixLabels(*o);
  }
  
  // Fix local groups
  PvlObject::PvlGroupIterator g;
  for (g = obj.BeginGroup() ; g != obj.EndGroup() ; ++g) {
    FixQuotes(*g);
  }
  return;
}


void IsisMain () {
  const std::string mdis2pds_program = "mdis2pds";
  const std::string mdis2pds_version = "1.0";
  const std::string mdis2pds_revision = "$Revision: 1.10 $";
  const std::string mdis2pds_runtime = Application::DateTime();

  UserInterface &ui = Application::GetUserInterface();
  Filename input(ui.GetFilename ("FROM"));
  Filename output = ui.GetFilename("TO");
  output.AddExtension("IMG");

  // Set up the export
  ProcessExportPds processPds;
  Cube *incube = processPds.SetInputCube( "FROM" );

  Histogram *hist = incube->Histogram(0);
  double minmin = 0.0;
  double maxmax = 0.0;
  if ( ui.GetString("TYPE").compare("AUTOMATIC") == 0 ) {
    minmin = (ui.GetDouble("MINPER") <= 0.000001) ? 
                  hist->Minimum() :
                  hist->Percent(ui.GetDouble("MINPER")); 

    maxmax = (ui.GetDouble("MAXPER") >= 99.999999) ?
                   hist->Maximum() :
                   hist->Percent(ui.GetDouble("MAXPER"));
  } else {
    minmin = ui.GetDouble("MIN");
    maxmax = ui.GetDouble("MAX");
  }

  processPds.SetOutputEndian(Isis::Msb);
  processPds.SetExportType( ProcessExportPds::Fixed );
  processPds.SetInputRange(minmin, maxmax);

  // Set the output pixel type and the special pixel values
  string dataSetID = "MESS-E/V/H-MDIS-";
  int nbits = ui.GetInteger("BITS");
  if (nbits == 8) {
    processPds.SetOutputType(UnsignedByte);
    processPds.SetOutputRange(VALID_MIN1, VALID_MAX1);
    processPds.SetOutputNull(NULL1);
    processPds.SetOutputLis(LOW_INSTR_SAT1);
    processPds.SetOutputLrs(LOW_REPR_SAT1);
    processPds.SetOutputHis(HIGH_INSTR_SAT1);
    processPds.SetOutputHrs(HIGH_REPR_SAT1);
    dataSetID += "1";
  }
  else if (nbits == 16) {
    processPds.SetOutputType(UnsignedWord);
    processPds.SetOutputRange(VALID_MINU2, VALID_MAXU2);
    processPds.SetOutputNull(NULLU2);
    processPds.SetOutputLis(LOW_INSTR_SATU2);
    processPds.SetOutputLrs(LOW_REPR_SATU2);
    processPds.SetOutputHis(HIGH_INSTR_SATU2);
    processPds.SetOutputHrs(HIGH_REPR_SATU2);
    dataSetID += "2";
  }
  else if (nbits == 32 ) {
    processPds.SetOutputType(Real);
    processPds.SetOutputRange(minmin, maxmax);
    processPds.SetOutputNull(NULL4);
    processPds.SetOutputLrs(LOW_REPR_SAT4);
    processPds.SetOutputLis(LOW_INSTR_SAT4);
    processPds.SetOutputHrs(HIGH_REPR_SAT4);
    processPds.SetOutputHis(HIGH_INSTR_SAT4);
    dataSetID += "4";
  }
  else if ( nbits > 8  &&  nbits < 16 ) {
    processPds.SetOutputType(UnsignedWord);
    processPds.SetOutputRange(3.0, pow(2.0, (double)(nbits))-3.0);
    processPds.SetOutputNull(0);
    processPds.SetOutputLrs(1);
    processPds.SetOutputLis(2);
    processPds.SetOutputHis(pow(2.0, (double)(nbits))-2.0);
    processPds.SetOutputHrs(pow(2.0, (double)(nbits))-1.0);
    dataSetID += "0";
  }
  else {
    string msg = "[" + iString(nbits) + "] is not a supported bit length.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  dataSetID += "-CDR-CALDATA-V1.0";

  Progress p;
  p.SetText("Modifying Keywords");
  p.SetMaximumSteps(7);
  p.CheckStatus();

  // Get the PDS label from the process
  Pvl &pdsLabel = processPds.StandardPdsLabel(ProcessExportPds::Image);

  // Translate the keywords from the original EDR PDS label that go in 
  // this RDR PDS label
  OriginalLabel origBlob;
  incube->Read(origBlob);
  Pvl origLabel;
  PvlObject origLabelObj = origBlob.ReturnLabels();
  origLabelObj.SetName("OriginalLabelObject");
  origLabel.AddObject(origLabelObj);

  p.CheckStatus();

  // Translates the ISIS labels along with the original EDR labels
  origLabel.AddObject( *(incube->Label()) );
  PvlTranslationManager labels( origLabel,
                                "$messenger/translations/mdisCdrLabel.trn");
  labels.Auto(pdsLabel);

  p.CheckStatus();

  // Add keyword comments
  PvlKeyword &recordType( pdsLabel.FindKeyword("RECORD_TYPE") );
  recordType.AddComment("/*** FILE FORMAT ***/");

  PvlKeyword &image( pdsLabel.FindKeyword("^IMAGE") );
  image.AddComment("/*** POINTERS TO START BYTE OFFSET OF OBJECTS IN IMAGE FILE ***/");

  PvlKeyword &missionName( pdsLabel.FindKeyword("MISSION_NAME") );
  missionName.AddComment("/*** GENERAL DATA DESCRIPTION PARAMETERS ***/");

  PvlKeyword &startTime( pdsLabel.FindKeyword("START_TIME") );
  startTime.AddComment("/*** TIME PARAMETERS ***/");

  PvlKeyword &instrumentName( pdsLabel.FindKeyword("INSTRUMENT_NAME") );
  instrumentName.AddComment("/*** INSTRUMENT ENGINEERING PARAMETERS ***/");

  PvlKeyword &messMetExp( pdsLabel.FindKeyword("MESS:MET_EXP") );
  messMetExp.AddComment("/*** INSTRUMENT RAW PARAMETERS ***/");

  PvlKeyword &geometry( pdsLabel.FindKeyword("RIGHT_ASCENSION") );
  geometry.AddComment("/*** GEOMETRY INFORMATION ***/");

  PvlKeyword &target( pdsLabel.FindKeyword("SC_TARGET_POSITION_VECTOR") );
  target.AddComment("/*** TARGET PARAMETERS ***/");

  PvlKeyword &sensor( pdsLabel.FindKeyword("SLANT_DISTANCE") );
  sensor.AddComment("/*** TARGET WITHIN SENSOR FOV ***/");

  PvlKeyword &spacecraftPosition( pdsLabel.FindKeyword("SUB_SPACECRAFT_LATITUDE") );
  spacecraftPosition.AddComment("/*** SPACECRAFT POSITION WITH RESPECT TO CENTRAL BODY ***/");

  PvlKeyword &spacecraftLocation( pdsLabel.FindKeyword("SPACECRAFT_SOLAR_DISTANCE") );
  spacecraftLocation.AddComment("/*** SPACECRAFT LOCATION ***/");

  PvlKeyword &solarDistance( pdsLabel.FindKeyword("SOLAR_DISTANCE") );
  solarDistance.AddComment("/*** VIEWING AND LIGHTING GEOMETRY (SUN ON TARGET) ***/");

  PvlGroup &subframe( pdsLabel.FindGroup("SUBFRAME1_PARAMETERS") );
  subframe.AddComment("/*** GEOMETRY FOR EACH SUBFRAME ***/");

  p.CheckStatus();

  // Creates keywords from the input's hist above
  PvlKeyword minDn( "MINIMUM", SetRound(hist->Minimum(),16) );
  PvlKeyword maxDn( "MAXIMUM", SetRound(hist->Maximum(),16) );
  PvlKeyword meanDn( "MEAN", SetRound(hist->Average(),16) );
  PvlKeyword stddev( "STANDARD_DEVIATION", SetRound(hist->StandardDeviation(),16) );

  PvlKeyword saturated( "SATURATED_PIXEL_COUNT", hist->HisPixels() );

  PvlObject &imageObj = pdsLabel.FindObject("IMAGE");

  minDn.AddComment("/*** IMAGE STATISTICS ***/");
  imageObj.AddKeyword( minDn );
  imageObj.AddKeyword( maxDn );
  imageObj.AddKeyword( meanDn );
  imageObj.AddKeyword( stddev );
  saturated.AddComment("/*** PIXEL COUNTS ***/");
  imageObj.AddKeyword( saturated );
  if (imageObj.HasKeyword("DARK_STRIP_MEAN") ) {
    PvlKeyword &darkStripMean = imageObj.FindKeyword("DARK_STRIP_MEAN");
    if (darkStripMean.Size() > 0) {
      darkStripMean[0] = iString(SetRound(darkStripMean[0],16));
    }
  }

  p.CheckStatus();

  // Fixes bad keywords
  PvlKeyword &data_set_id = pdsLabel.FindKeyword( "DATA_SET_ID", Pvl::Traverse );
  data_set_id.SetValue( dataSetID );
  PvlKeyword &product_id = pdsLabel.FindKeyword( "PRODUCT_ID", Pvl::Traverse );
  if ((product_id.Size() == 0) || ((product_id.Size() > 0) && (product_id[0] == "N/A"))) {
    product_id.SetValue( output.Basename() );
  }
  PvlKeyword &product_creation_time = pdsLabel.FindKeyword( "PRODUCT_CREATION_TIME", Pvl::Traverse );
  product_creation_time.SetValue( mdis2pds_runtime );

  PvlKeyword &software_name = pdsLabel.FindKeyword( "SOFTWARE_NAME", Pvl::Traverse );
  if ( (software_name.Size() > 0) && (software_name[0] == "N/A")) {
    software_name.SetValue( mdis2pds_program );
  }

  PvlKeyword &software_version_id = pdsLabel.FindKeyword( "SOFTWARE_VERSION_ID", Pvl::Traverse );
  if ( software_version_id.Size() > 0 ) {
     if ( software_version_id[0] == "N/A") { 
       software_version_id.SetValue( Quote(mdis2pds_version) );
     }
     else {
       software_version_id.SetValue( software_version_id[0] );
     }
  }

  PvlKeyword &filter_number = pdsLabel.FindKeyword( "FILTER_NUMBER",Pvl::Traverse );
  if ( (filter_number.Size() > 0) ) {
    filter_number.SetValue( Quote(filter_number[0]) );
  }


  // Add quotes
  PvlKeyword &data_quality_id = pdsLabel.FindKeyword( "DATA_QUALITY_ID", Pvl::Traverse );
  data_quality_id.SetValue(Quote(data_quality_id));
  PvlKeyword &sequence_name = pdsLabel.FindKeyword( "SEQUENCE_NAME", Pvl::Traverse );
  sequence_name.SetValue(Quote(sequence_name));
  PvlKeyword &start_count = pdsLabel.FindKeyword( "SPACECRAFT_CLOCK_START_COUNT", Pvl::Traverse );
  start_count.SetValue(Quote(start_count));
  PvlKeyword &stop_count = pdsLabel.FindKeyword( "SPACECRAFT_CLOCK_STOP_COUNT", Pvl::Traverse );
  stop_count.SetValue(Quote(stop_count));
  PvlKeyword &source_product_id = pdsLabel.FindKeyword( "SOURCE_PRODUCT_ID", Pvl::Traverse );
  for ( int i = 0; i < source_product_id.Size(); i++ ) {
    source_product_id[i] = Quote(source_product_id[i]);
  }

  //  Enforce parentheses for scalars
  if (source_product_id.Size() == 1) 
    source_product_id.SetValue('('+source_product_id[0]+')');

  // Removes keywords
  PvlObject &imageObject( pdsLabel.FindObject("IMAGE") );
  imageObject.DeleteKeyword("FILTER_NAME");
  imageObject.DeleteKeyword("CENTER_FILTER_WAVELENGTH");
  imageObject.DeleteKeyword("BANDWIDTH");

  p.CheckStatus();

  //  Fix all the hosed units upon ingest.  They are illformed.
  FixUnit(pdsLabel, "RETICLE_POINT_RA", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_DECLINATION", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_LATITUDE", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_LONGITUDE", "DEG");

  //  Now address nested keywords in SUBFRAME groups
  for (int i = 1 ; i <= 5 ; i++) {
    iString n(i);
    string group = "SUBFRAME" + n + "_PARAMETERS";
    if (pdsLabel.HasGroup(group)) {
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
  ofstream outstream( output.Expanded().c_str() );
  processPds.OutputLabel(outstream);

  processPds.StartProcess(outstream);
  outstream.close();
  processPds.EndProcess();
}
