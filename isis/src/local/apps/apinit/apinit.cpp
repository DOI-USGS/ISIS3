#include "Isis.h"

#include <vector>

#include <QDate>
#include <QString>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Brick.h"
#include "Cube.h"
#include "FileName.h"
#include "History.h"
#include "IString.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"
#include "Table.h"
#include "TProjection.h"
#include "NaifStatus.h"

using namespace Isis;
using namespace std;

PvlKeyword getArrayValues(const QString &parameter, const QString &keyword, int count);


void IsisMain() {
  // Fire up the user interface
  UserInterface &ui = Application::GetUserInterface();

  // Get the name of the cube to initialize
  Cube cube(ui.GetFileName("FROM"),"rw");
  
  // Create archive group with the following information:
  //   Roll Number
  //   Frame Number 
  //   Camera Identification
  //   Lens Identification
  //   Camera calibration date
  PvlGroup archiveGroup("Archive");

  PvlKeyword keyword;
  keyword.setName("RollNumber");
  keyword.setValue(ui.GetString("RollNumber") );
  archiveGroup.addKeyword(keyword);

  keyword.setName("FrameNumber");
  keyword.setValue(ui.GetString("FrameNumber") );
  archiveGroup.addKeyword(keyword);

  keyword.setName("CameraSerialNumber");
  keyword.setValue(ui.GetString("CameraSN") );
  archiveGroup.addKeyword(keyword);

  keyword.setName("LenseSerialNumber");
  keyword.setValue(ui.GetString("LenseSN") );
  archiveGroup.addKeyword(keyword);

  keyword.setName("CalibrationReport");
  keyword.setValue(ui.GetString("CalReport") );
  archiveGroup.addKeyword(keyword);

  cube.putGroup(archiveGroup);

  // Create instrument group with the following information:
  //   Mission/spacecraft name
  //   Target name
  //   Date and time of observation
  //   Ephemeris time
  //   Center longitude/latitude
  //   Altitude above mean radius
  //   Focal length
  //   Fidicual measurements in image
  //   Fidicual locations in focal plane
  //   Optical distortion information
  PvlGroup instrumentGroup("Instrument");

  keyword.setName("SpacecraftName");
  keyword.setValue("Aircraft");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("InstrumentId");
  keyword.setValue("AerialPhoto");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("TargetName");
  keyword.setValue("Earth");
  instrumentGroup.addKeyword(keyword);

  QString startTime = ui.GetString("DateTime");
  // Validate the observation date format.  See naif required reading for time format
  // http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/str2et_c.html
  ConstSpiceChar *spiceStartTime = startTime.toLatin1().data();
  SpiceDouble junk;
  SpiceChar errmsg[500];
  tparse_c (spiceStartTime, 500, &junk, errmsg);
  if (errmsg[0] != 0) {
    QString msg = "Invalid date/time format [" + startTime + "].  See NAIF required reading "
                  "for acceptable formats at http://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/time.html";
    msg += ".  Naif error message is [" + QString((char*) errmsg) + "]";
                        
    throw IException(IException::User, msg, _FILEINFO_);
  }
  keyword.setName("StartTime");
  keyword.setValue(startTime);
  instrumentGroup.addKeyword(keyword);

  FileName lsk("$base/kernels/lsk/naif0010.tls");

  NaifStatus::CheckErrors(); 
  furnsh_c(lsk.expanded().toLatin1().constData());

  double et;
  spiceStartTime = startTime.toLatin1().data();
  str2et_c(spiceStartTime, &et);

  keyword.setName("EphemerisTime");
  keyword.setValue(toString(et),"seconds");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("EstimatedAltitude");
  keyword.setValue(toString(ui.GetDouble("Altitude")), "meters");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("EstimatedCenterLatitude");
  keyword.setValue(toString(ui.GetDouble("CenterLat")), "degrees");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("EstimatedCenterLongitude");
  keyword.setValue(toString(ui.GetDouble("CenterLon")), "degrees");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("FocalLength");
  keyword.setValue(toString(ui.GetDouble("FocalLength")), "millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("FiducialX", "FiducialX", 8);
  keyword.setUnits("millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("FiducialY", "FiducialY", 8);
  keyword.setUnits("millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("FiducialSample", "FiducialSample", 8);
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("FiducialLine", "FiducialLine", 8);
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("RadialDistortionCoefficients", "KCoefs", 5);
  instrumentGroup.addKeyword(keyword);

  keyword = getArrayValues("DecenteringDistortionCoefficients", "PCoefs", 4);
  instrumentGroup.addKeyword(keyword);

  keyword.setName("XCalibratedPrincipalPoint");
  keyword.setValue(toString(ui.GetDouble("XP")), "millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("YCalibratedPrincipalPoint");
  keyword.setValue(toString(ui.GetDouble("YP")), "millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("XIndicatedPrincipalPoint");
  keyword.setValue(toString(ui.GetDouble("XIPP")), "millimeters");
  instrumentGroup.addKeyword(keyword);

  keyword.setName("YIndicatedPrincipalPoint");
  keyword.setValue(toString(ui.GetDouble("YIPP")), "millimeters");
  instrumentGroup.addKeyword(keyword);

  cube.putGroup(instrumentGroup);

  // Create kernels group
  PvlGroup kernelsGroup("Kernels");

  keyword.setName("NaifFrameCode");
  keyword.setValue("-2000001");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("LeapSecond");
  keyword.setValue("$base/kernels/lsk/naif0010.tls");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("TargetAttitudeShape");
  keyword.setValue("$base/kernels/pck/pck00009.tpc");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("TargetPosition");
  keyword.setValue("Table");
  keyword.addValue("$base/kernels/spk/de405.bsp");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("InstrumentPointing");
  keyword.setValue("Table");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("Instrument");
  keyword.clear();
  kernelsGroup.addKeyword(keyword);

  keyword.setName("SpacecraftClock");
  keyword.clear();
  kernelsGroup.addKeyword(keyword);

  keyword.setName("InstrumentPosition");
  keyword.setValue("Table");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("InstrumentAddendum");
  keyword.clear();
  kernelsGroup.addKeyword(keyword);

  keyword.setName("ShapeModel");
  keyword.setValue(ui.GetAsString("ShapeModel"));
  kernelsGroup.addKeyword(keyword);

  keyword.setName("InstrumentPositionQuality");
  keyword.setValue("Predict");
  kernelsGroup.addKeyword(keyword);
  
  keyword.setName("InstrumentPointingQuality");
  keyword.setValue("Predict");
  kernelsGroup.addKeyword(keyword);

  keyword.setName("CameraVersion");
  keyword.setValue("1");
  kernelsGroup.addKeyword(keyword);

  cube.putGroup(kernelsGroup);

// Write out the naif keywords group.  This must be done because the camera model
// needs to know the earth's frame number and radii.  This is typically done by the spiceinit program
// but there are no SPICE kernels for aerial photos other than those that define the
// rotation matrix from j2000 to body-fixed for the earth.   Our group should look something
// like this in the labels
// 
// Object = NaifKeywords
//  BODY_FRAME_CODE                    = 10013
//  BODY399_RADII                      = (6378.14, 6378.14, 6356.75)
// End_Object

  PvlObject naifKeywords;
  naifKeywords.setName("NaifKeywords");
  
  keyword.setName("BODY_FRAME_CODE");
  keyword.setValue("10013");  
  naifKeywords.addKeyword(keyword);
    
  keyword.setName("BODY399_RADII");
  keyword.setValue("6378.14");  
  keyword.addValue("6378.14");  
  keyword.addValue("6356.75");
  keyword.setUnits("kilometers"); 
  naifKeywords.addKeyword(keyword);

  cube.label()->addObject(naifKeywords);

  // Create table with rotation matrices from bodyfixed to J2000 and write to the labels.  
  // Need to load the appropriatespk and pck kernels for this to work.  
  FileName pck("$base/kernels/pck/pck00009.tpc");
  furnsh_c(pck.expanded().toLatin1().constData());
  FileName tspk("$base/kernels/spk/de405.bsp");
  furnsh_c(tspk.expanded().toLatin1().constData());

  SpiceRotation bodyRotation(10013);
  bodyRotation.LoadCache(et-1.0, et+1.0, 2); // Get a small window about the observation time

  Table bodyTable = bodyRotation.Cache("BodyRotation");
  bodyTable.Label() += PvlKeyword("Description", "Created by apinit");
  bodyTable.Label() += PvlKeyword("Kernels");
  bodyTable.Label()["Kernels"].addValue("$base/kernels/spk/de405.bsp");
  bodyTable.Label()["Kernels"].addValue("$base/kernels/pck/pck00009.tpc");
  bodyTable.Label() += PvlKeyword("SolarLongitude","-9999"); // TODO: Need to compute the solar longitude

  cube.write(bodyTable);


  // Create table with position of sun relative to earth.  
  SpicePosition sunPosition(10, 399);
  sunPosition.LoadCache(et-1.0, et+1.0, 2); // Get a small window about the observation time

  Table sunTable = sunPosition.Cache("SunPosition");
  sunTable.Label() += PvlKeyword("Description", "Created by apinit");
  sunTable.Label() += PvlKeyword("Kernels");
  bodyTable.Label()["Kernels"].addValue("$base/kernels/spk/de405.bsp");

  cube.write(sunTable);

  // Create a table with apriori position of aircraft relative to earth in J2000.
  // This will take several steps.   First we need the radius for the aerial photo.   
  // The user supplied a lat/lon and DTM.   We will use the lat/lon to get a radius from
  // the DTM.  Those three values will be converted from polar coordinates (lat,lon,rad) to
  // P(x,y,z).  Then we will get a unit vector of P and multiply it by the
  // altitude to get Pa = (xhat*alt, yhat*alt, zhat*alt).   Then we will add P and Pa to 
  // estimate the aircraft position in body fixed.  Finally we will rotation Pa into J2000 
  // using the bodyRotation object (SpiceRotation class).  The velocity is unknown so we will not
  // write it out to our table.  We could improve the estimate of the spacecraft position by 
  // computing the surface normal at P off of an ellipsoid.  We won't go to this level of accuracy
  // because using jigsaw will be a requirement to improvement the pointing of the camera and the 
  // aircraft position should also be refined at that time.

  Cube dtm;
  dtm.open(ui.GetAsString("ShapeModel"));
  TProjection *mapproj = (TProjection *) dtm.projection();
  mapproj->SetGround(ui.GetDouble("CenterLat"), ui.GetDouble("CenterLon"));
  int line = mapproj->WorldY() + 0.5;
  int samp = mapproj->WorldX() + 0.5;
  Brick buf(1,1,1,dtm.pixelType());  
  buf.SetBasePosition(samp, line, 1);
  dtm.read(buf);

  if (IsSpecial(buf[0]) || buf[0] < 6300000) {
    QString msg = "DTM [" + ui.GetAsString("ShapeModel") + "] does not contain valid radius "
                  " at user specified latitude/longitude.  Read [" + toString(buf[0]) + "] at "
                  "sample/line [" + toString(samp) + "," + toString(line) + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  SpiceDouble radius = buf[0] / 1000.0;
  SpiceDouble ulat = mapproj->UniversalLatitude() * Isis::PI / 180.0;
  SpiceDouble ulon = mapproj->UniversalLongitude() * Isis::PI / 180.0;
    
  SpiceDouble xyz[3];
  latrec_c(radius, ulon, ulat, xyz);
  
  SpiceDouble xyzhat[3];
  vhat_c(xyz, xyzhat);

  std::vector<double> apposB;  // Position of aircraft in body fixed
  apposB.push_back(xyz[0] + xyzhat[0] * ui.GetDouble("Altitude") / 1000.0); 
  apposB.push_back(xyz[1] + xyzhat[1] * ui.GetDouble("Altitude") / 1000.0); 
  apposB.push_back(xyz[2] + xyzhat[2] * ui.GetDouble("Altitude") / 1000.0); 

  bodyRotation.SetEphemerisTime(et);      
  std::vector<double> apposJ = bodyRotation.J2000Vector(apposB);

  TableRecord spkRecord;
  TableField x("J2000X", TableField::Double);
  TableField y("J2000Y", TableField::Double);
  TableField z("J2000Z", TableField::Double);
  TableField t("ET", TableField::Double);
  spkRecord += x;
  spkRecord += y;
  spkRecord += z;
  spkRecord += t;

  Table spkTable("InstrumentPosition", spkRecord);
  spkRecord[0] = apposJ[0];
  spkRecord[1] = apposJ[1];
  spkRecord[2] = apposJ[2];
  spkRecord[3] = et - 1.0;
  spkTable += spkRecord;

  spkRecord[3] = et + 1.0;
  spkTable += spkRecord;

  spkTable.Label() += PvlKeyword("Description", "Created by apinit");
  spkTable.Label() += PvlKeyword("Kernels");

  cube.write(spkTable);
  dtm.close();

  // Now we must write out the camera to J2000 rotation matrices.  Unfortunately
  // nothing exists that defines this information.   However, we can create the camera
  // pointing using photogramettric techniques by finding tie points between an aerial 
  // photo and a basemap that covers the same area of the photo.  
  // A basemap example is a DOQQ image that is map projected.  We won't do that work 
  // here since we already have programs that can help determine the tie points and pointing 
  // (findfeatures and jigsaw).  Instead we will just write out the identity rotation
  // such that we assume the aircraft is pointing straight down (roll and pitch of 0) which is
  // probably correct.   What we really lack is the yaw/twist to know if the aircraft is
  // flying north-south, south-north, or along some other track.   We will just assume 
  // the aircraft has no twist too.  It will be wrong but findfeatures and jigsaw will
  // figure it out.  (CJ is the J2000 to camera rotation matrix.  We'll set it to the 
  // identity and then convert to a quaternion so we can write out our table file.

  SpiceDouble CJ[9] = { 1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0 };
  SpiceDouble minusApposJ[3];
  //minusApposJ[0] = -apposJ[0];
  //minusApposJ[0] = -apposJ[0];
  //minusApposJ[1] = -apposJ[1];
  minusApposJ[0] = apposJ[0];
  minusApposJ[1] = apposJ[1];
  minusApposJ[2] = apposJ[2];

  SpiceDouble perpVec[3];
  SpiceDouble axis[3] = {0.0, 1.0, 0.0};
  vrotv_c(minusApposJ, axis, 90.0, perpVec);
  
  twovec_c (minusApposJ, 3, perpVec, 2, (SpiceDouble( *)[3]) &CJ[0]);

  SpiceDouble quat[4];
  m2q_c(CJ, quat);
  NaifStatus::CheckErrors(); 

  TableField q0("J2000Q0", TableField::Double);
  TableField q1("J2000Q1", TableField::Double);
  TableField q2("J2000Q2", TableField::Double);
  TableField q3("J2000Q3", TableField::Double);
  TableField qt("ET", TableField::Double);

  TableRecord ckRecord;
  ckRecord += q0;
  ckRecord += q1;
  ckRecord += q2;
  ckRecord += q3;
  ckRecord += qt;

  Table ckTable("InstrumentPointing", ckRecord);
  
  ckRecord[0] = quat[0];
  ckRecord[1] = quat[1];
  ckRecord[2] = quat[2];
  ckRecord[3] = quat[3];
  ckRecord[4] = et - 1.0;
  ckTable += ckRecord;

  ckTable.Label() += PvlKeyword("Description", "Created by apinit");
  ckTable.Label() += PvlKeyword("Kernels");

  cube.write(ckTable);  


  // Write out the history to the cube so we have a record of what the user ran
  History hist = History("IsisCube");
  try {
    cube.read(hist);
  }
  catch(IException &e) {
    // No history exist but don't throw an error. just add a history object
  }
  hist.AddEntry();
  cube.write(hist);

  // clean up
  cube.close();
}



PvlKeyword getArrayValues(const QString &parameter, const QString &uiKeyword, int count) {

  UserInterface &ui = Application::GetUserInterface();
  QString value = ui.GetString(uiKeyword);

  PvlKeyword keyword;
  keyword.setName(parameter);

  QStringList tokens = value.split(",");
  if (tokens.size() != count) {
    QString msg = "Invalid value for [" + uiKeyword.toUpper() + "].  Expecting exactly " +
                  QString::number(count) + " comma "
                  "separated values and found only [" + QString::number(tokens.size()) + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  for (int i=0; i<count; i++) {
    try {
      toDouble(tokens[i].trimmed()); 
    } 
    catch (IException &e) {
      QString msg = "Invalid value for [" + uiKeyword.toUpper() + "] at index [" + QString::number(i+1) + 
                    "].  Found [" + tokens[i] + "]";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
    keyword.addValue(tokens[i].trimmed());
  }

  return keyword;
}

