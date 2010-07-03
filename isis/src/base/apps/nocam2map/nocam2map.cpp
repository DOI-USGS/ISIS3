#define GUIHELPERS

#include "Isis.h"
#include "Constants.h"
#include "naif/SpiceUsr.h"
#include "Brick.h"
#include "PolynomialBivariate.h"
#include "LeastSquares.h"
#include "ProcessRubberSheet.h"
#include "iString.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Statistics.h"
#include "TextFile.h"
#include "nocam2map.h"

using namespace std;
using namespace Isis;

void PrintMap ();
void ComputePixRes();
void LoadMapRes();
void ComputeInputRange();
void LoadMapRange();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["PrintMap"] = (void*) PrintMap;
  helper ["ComputePixRes"] = (void*) ComputePixRes;
  helper ["LoadMapRes"] = (void*) LoadMapRes;
  helper ["ComputeInputRange"] = (void*) ComputeInputRange;
  helper ["LoadMapRange"] = (void*) LoadMapRange;
  return helper;
}


void IsisMain() {
  //Create a process to create the input cubes
  Process p;
  //Create the input cubes, matching sample/lines
  Cube *inCube = p.SetInputCube ("FROM");
  Cube *latCube = p.SetInputCube("LATCUB", SpatialMatch);
  Cube *lonCube = p.SetInputCube("LONCUB", SpatialMatch);

  //A 1x1 brick to read in the latitude and longitude DN values from
  //the specified cubes
  Brick latBrick(1,1,1, latCube->PixelType());
  Brick lonBrick(1,1,1, lonCube->PixelType());

  UserInterface &ui = Application::GetUserInterface();

  //Set the sample and line increments
  int sinc = (int)(inCube->Samples() * 0.10);
  if(ui.WasEntered("SINC")) {
    sinc = ui.GetInteger("SINC");
  }

  int linc = (int)(inCube->Lines() * 0.10);
  if(ui.WasEntered("LINC")) {
    linc = ui.GetInteger("LINC");
  }

  //Set the degree of the polynomial to use in our functions
  int degree = ui.GetInteger("DEGREE");

  //We are using a polynomial with two variables
  PolynomialBivariate sampFunct(degree); 
  PolynomialBivariate lineFunct(degree);

  //We will be solving the function using the least squares method
  LeastSquares sampSol(sampFunct);
  LeastSquares lineSol(lineFunct);

  //Setup the variables for solving the stereographic projection
  //x = cos(latitude) * sin(longitude - lon_center)
  //y = cos(lat_center) * sin(latitude) - sin(lat_center) * cos(latitude) * cos(longitude - lon_center)

  //Get the center lat and long from the input cubes
  double lat_center = latCube->Statistics()->Average() * PI/180.0;
  double lon_center = lonCube->Statistics()->Average() * PI/180.0;


  /**
   * Loop through lines and samples projecting the latitude and longitude at those
   * points to stereographic x and y and adding these points to the LeastSquares 
   * matrix. 
   */
  for(int i = 1; i <= inCube->Lines(); i+= linc) {
    for(int j = 1; j <= inCube->Samples(); j+= sinc) {
      latBrick.SetBasePosition(j, i, 1);
      latCube->Read(latBrick);
      if(IsSpecial(latBrick.at(0))) continue;
      double lat = latBrick.at(0) * PI/180.0;
      lonBrick.SetBasePosition(j, i, 1);
      lonCube->Read(lonBrick);
      if(IsSpecial(lonBrick.at(0))) continue;
      double lon = lonBrick.at(0) * PI/180.0;

      //Project lat and lon to x and y using a stereographic projection
      double k = 2/(1 + sin(lat_center) * sin(lat) + cos(lat_center)*cos(lat)*cos(lon - lon_center));
      double x = k * cos(lat) * sin(lon - lon_center);
      double y = k * (cos(lat_center) * sin(lat)) - (sin(lat_center) * cos(lat) * cos(lon - lon_center));

      //Add x and y to the least squares matrix
      vector<double> data;
      data.push_back(x);
      data.push_back(y);
      sampSol.AddKnown(data, j);
      lineSol.AddKnown(data, i);

      //If the sample increment goes past the last sample in the line, we want to
      //always read the last sample..
      if(j != inCube->Samples() && j + sinc > inCube->Samples()) {
        j = inCube->Samples() - sinc;
      }
    }
    //If the line increment goes past the last line in the cube, we want to
    //always read the last line..
    if(i != inCube->Lines() && i + linc > inCube->Lines()) {    
      i = inCube->Lines() - linc;
    }
  }

  //Solve the least squares functions using QR Decomposition
  sampSol.Solve(LeastSquares::QRD);
  lineSol.Solve(LeastSquares::QRD);

  //If the user wants to save the residuals to a file, create a file and write
  //the column titles to it.
  TextFile oFile;
  if(ui.WasEntered("RESIDUALS")) {
    oFile.Open(ui.GetFilename("RESIDUALS"), "overwrite");
    oFile.PutLine("Sample,\tLine,\tX,\tY,\tSample Error,\tLine Error\n");
  }

  //Gather the statistics for the residuals from the least squares solutions
  Statistics sampErr;
  Statistics lineErr;
  vector<double> sampResiduals = sampSol.Residuals();
  vector<double> lineResiduals = lineSol.Residuals();
  for(int i = 0; i < (int)sampResiduals.size(); i++) {
    sampErr.AddData(sampResiduals[i]);
    lineErr.AddData(lineResiduals[i]);
  }

  //If a residuals file was specified, write the previous data, and the errors to the file.
  if(ui.WasEntered("RESIDUALS")) {
    for(int i = 0; i < sampSol.Rows(); i++) {
      vector<double> data = sampSol.GetInput(i);
      iString tmp = "";
      tmp += iString(sampSol.GetExpected(i));
      tmp += ",\t";
      tmp += iString(lineSol.GetExpected(i));
      tmp += ",\t";
      tmp += iString(data[0]);
      tmp += ",\t";
      tmp += iString(data[1]);
      tmp += ",\t";
      tmp += iString(sampResiduals[i]);
      tmp += ",\t";
      tmp += iString(lineResiduals[i]);
      oFile.PutLine(tmp + "\n");
    }
  }
  oFile.Close();

  //Records the error to the log
  PvlGroup error( "Error" );
  error += PvlKeyword( "Degree", degree );
  error += PvlKeyword( "NumberOfPoints", (int)sampResiduals.size() );
  error += PvlKeyword( "SampleMinimumError", sampErr.Minimum() );
  error += PvlKeyword( "SampleAverageError", sampErr.Average() );
  error += PvlKeyword( "SampleMaximumError", sampErr.Maximum() );
  error += PvlKeyword( "SampleStdDeviationError", sampErr.StandardDeviation() );
  error += PvlKeyword( "LineMinimumError", lineErr.Minimum() );
  error += PvlKeyword( "LineAverageError", lineErr.Average() );
  error += PvlKeyword( "LineMaximumError", lineErr.Maximum() );
  error += PvlKeyword( "LineStdDeviationError", lineErr.StandardDeviation() );
  Application::Log( error );

  //Close the input cubes for cleanup
  p.EndProcess();

  //If we want to warp the image, then continue, otherwise return
  if(!ui.GetBoolean("NOWARP")) {
    //Creates the mapping group
    Pvl mapFile;
    mapFile.Read(ui.GetFilename("MAP"));
    PvlGroup &mapGrp = mapFile.FindGroup("Mapping",Pvl::Traverse);

    //Reopen the lat and long cubes
    latCube = new Cube();
    latCube->SetVirtualBands(ui.GetInputAttribute("LATCUB").Bands());
    latCube->Open(ui.GetFilename("LATCUB"));

    lonCube = new Cube();
    lonCube->SetVirtualBands(ui.GetInputAttribute("LONCUB").Bands());
    lonCube->Open(ui.GetFilename("LONCUB"));

    PvlKeyword targetName;

    //If the user entered the target name
    if(ui.WasEntered("TARGET")) {
      targetName = PvlKeyword("TargetName", ui.GetString("TARGET"));
    }
    //Else read the target name from the input cube
    else {
      Pvl fromFile;
      fromFile.Read(ui.GetFilename("FROM"));
      targetName = fromFile.FindKeyword("TargetName", Pvl::Traverse);
    }

    mapGrp.AddKeyword(targetName, Pvl::Replace);

    PvlKeyword equRadius;
    PvlKeyword polRadius;


    //If the user entered the equatorial and polar radii
    if(ui.WasEntered("EQURADIUS") && ui.WasEntered("POLRADIUS")) {
      equRadius = PvlKeyword("EquatorialRadius", ui.GetDouble("EQURADIUS"));
      polRadius = PvlKeyword("PolarRadius", ui.GetDouble("POLRADIUS"));
    }
    //Else read them from the pck
    else {
      Filename pckFile("$base/kernels/pck/pck?????.tpc");
      pckFile.HighestVersion();

      string pckFilename = pckFile.Expanded();

      furnsh_c(pckFilename.c_str());

      string target = targetName[0];
      SpiceInt code;
      SpiceBoolean found;

      bodn2c_c (target.c_str(), &code, &found);

      if (!found) {
        string msg = "Could not convert Target [" + target +
                     "] to NAIF code";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }

      SpiceInt n;
      SpiceDouble radii[3];

      bodvar_c(code,"RADII",&n,radii);

      equRadius = PvlKeyword("EquatorialRadius", radii[0] * 1000);
      polRadius = PvlKeyword("PolarRadius", radii[2] * 1000);
    }

    mapGrp.AddKeyword(equRadius, Pvl::Replace);
    mapGrp.AddKeyword(polRadius, Pvl::Replace);


    //If the latitude type is not in the mapping group, copy it from the input
    if(!mapGrp.HasKeyword("LatitudeType")) {
      if(ui.GetString("LATTYPE") == "PLANETOCENTRIC") {
        mapGrp.AddKeyword(PvlKeyword("LatitudeType","Planetocentric"), Pvl::Replace);
      }
      else {
        mapGrp.AddKeyword(PvlKeyword("LatitudeType","Planetographic"), Pvl::Replace);
      }
    }

    //If the longitude direction is not in the mapping group, copy it from the input
    if(!mapGrp.HasKeyword("LongitudeDirection")) {
      if(ui.GetString("LONDIR") == "POSITIVEEAST") {
        mapGrp.AddKeyword(PvlKeyword("LongitudeDirection","PositiveEast"), Pvl::Replace);
      }
      else {
        mapGrp.AddKeyword(PvlKeyword("LongitudeDirection","PositiveWest"), Pvl::Replace);
      }
    }

    //If the longitude domain is not in the mapping group, assume it is 360
    if(!mapGrp.HasKeyword("LongitudeDomain")) {
      mapGrp.AddKeyword(PvlKeyword("LongitudeDomain","360"), Pvl::Replace);
    }

    //If the default range is to be computed, use the input lat/long cubes to determine the range
    if(ui.GetString("DEFAULTRANGE") == "COMPUTE") {
      //NOTE - When computing the min/max longitude this application does not account for the 
      //longitude seam if it exists. Since the min/max are calculated from the statistics of
      //the input longitude cube and then converted to the mapping group's domain they may be
      //invalid for cubes containing the longitude seam. 
    
      Statistics *latStats = latCube->Statistics();
      Statistics *lonStats = lonCube->Statistics();

      double minLat = latStats->Minimum();
      double maxLat = latStats->Maximum();

      bool isOcentric = ((std::string)mapGrp.FindKeyword("LatitudeType")) == "Planetocentric";
 
      if(isOcentric) {
        if(ui.GetString("LATTYPE") != "PLANETOCENTRIC") {
          minLat = Projection::ToPlanetocentric(minLat, (double)equRadius, (double)polRadius);
          maxLat = Projection::ToPlanetocentric(maxLat, (double)equRadius, (double)polRadius);
        }
      }
      else {
        if(ui.GetString("LATTYPE") == "PLANETOCENTRIC") {
          minLat = Projection::ToPlanetographic(minLat, (double)equRadius, (double)polRadius);
          maxLat = Projection::ToPlanetographic(maxLat, (double)equRadius, (double)polRadius);
        }
      }

      int lonDomain = (int)mapGrp.FindKeyword("LongitudeDomain");
      double minLon = lonDomain == 360 ? Projection::To360Domain(lonStats->Minimum()) : Projection::To180Domain(lonStats->Minimum());
      double maxLon = lonDomain == 360 ? Projection::To360Domain(lonStats->Maximum()) : Projection::To180Domain(lonStats->Maximum());

      bool isPosEast = ((std::string)mapGrp.FindKeyword("LongitudeDirection")) == "PositiveEast";
      
      if(isPosEast) {
        if(ui.GetString("LONDIR") != "POSITIVEEAST") {
          minLon = Projection::ToPositiveEast(minLon, lonDomain);
          maxLon = Projection::ToPositiveEast(maxLon, lonDomain);
        }
      }
      else {
        if(ui.GetString("LONDIR") == "POSITIVEEAST") {
          minLon = Projection::ToPositiveWest(minLon, lonDomain);
          maxLon = Projection::ToPositiveWest(maxLon, lonDomain);
        }
      }

      if(minLon > maxLon) {
        double temp = minLon;
        minLon = maxLon;
        maxLon = temp;
      }

      mapGrp.AddKeyword(PvlKeyword("MinimumLatitude", minLat),Pvl::Replace);
      mapGrp.AddKeyword(PvlKeyword("MaximumLatitude", maxLat),Pvl::Replace);
      mapGrp.AddKeyword(PvlKeyword("MinimumLongitude", minLon),Pvl::Replace);
      mapGrp.AddKeyword(PvlKeyword("MaximumLongitude", maxLon),Pvl::Replace);
    }

    //If the user decided to enter a ground range then override
    if (ui.WasEntered("MINLAT")) {
      mapGrp.AddKeyword(PvlKeyword("MinimumLatitude",
                                        ui.GetDouble("MINLAT")),Pvl::Replace);
    }
  
    if (ui.WasEntered("MAXLAT")) {
      mapGrp.AddKeyword(PvlKeyword("MaximumLatitude",
                                        ui.GetDouble("MAXLAT")),Pvl::Replace);
    }

    if (ui.WasEntered("MINLON")) {
      mapGrp.AddKeyword(PvlKeyword("MinimumLongitude",
                                        ui.GetDouble("MINLON")),Pvl::Replace);
    }
  
    if (ui.WasEntered("MAXLON")) {
      mapGrp.AddKeyword(PvlKeyword("MaximumLongitude",
                                        ui.GetDouble("MAXLON")),Pvl::Replace);
    }
  
    //If the pixel resolution is to be computed, compute the pixels/degree from the input
    if (ui.GetString("PIXRES") == "COMPUTE") {
      latBrick.SetBasePosition(1,1,1);
      latCube->Read(latBrick);

      lonBrick.SetBasePosition(1,1,1);
      lonCube->Read(lonBrick);

      //Read the lat and long at the upper left corner
      double a = latBrick.at(0) * PI/180.0;
      double c = lonBrick.at(0) * PI/180.0;
  
      latBrick.SetBasePosition(latCube->Samples(),latCube->Lines(),1);
      latCube->Read(latBrick);

      lonBrick.SetBasePosition(lonCube->Samples(),lonCube->Lines(),1);     
      lonCube->Read(lonBrick);

      //Read the lat and long at the lower right corner
      double b = latBrick.at(0) * PI/180.0;
      double d = lonBrick.at(0) * PI/180.0;

      //Determine the angle between the two points
      double angle = acos(cos(a) * cos(b) * cos(c - d) + sin(a) * sin(b));
      //double angle = acos((cos(a1) * cos(b1) * cos(b2)) + (cos(a1) * sin(b1) * cos(a2) * sin(b2)) + (sin(a1) * sin(a2)));
      angle *= 180/PI;

      //Determine the number of pixels between the two points
      double pixels = sqrt(pow(latCube->Samples() -1.0, 2.0) + pow(latCube->Lines() -1.0, 2.0));

      //Add the scale in pixels/degree to the mapping group
      mapGrp.AddKeyword(PvlKeyword("Scale",
                                        pixels/angle, "pixels/degree"),
                                        Pvl::Replace);
      if (mapGrp.HasKeyword("PixelResolution")) {
        mapGrp.DeleteKeyword("PixelResolution");
      }
    }


    // If the user decided to enter a resolution then override
    if (ui.GetString("PIXRES") == "MPP") {
      mapGrp.AddKeyword(PvlKeyword("PixelResolution",
                                        ui.GetDouble("RESOLUTION"), "meters/pixel"),
                                        Pvl::Replace);
      if (mapGrp.HasKeyword("Scale")) {
        mapGrp.DeleteKeyword("Scale");
      }
    }
    else if (ui.GetString("PIXRES") == "PPD") {
      mapGrp.AddKeyword(PvlKeyword("Scale",
                                        ui.GetDouble("RESOLUTION"), "pixels/degree"),
                                        Pvl::Replace);
      if (mapGrp.HasKeyword("PixelResolution")) {
        mapGrp.DeleteKeyword("PixelResolution");
      }
    }

    //Create a projection using the map file we created
    int samples,lines;
    Projection *outmap = ProjectionFactory::CreateForCube(mapFile,samples,lines,false);

    //Write the map file to the log
    Application::GuiLog(mapGrp);

    //Create a process rubber sheet
    ProcessRubberSheet r;

    //Set the input cube
    inCube = r.SetInputCube("FROM");

    double tolerance = ui.GetDouble("TOLERANCE") * outmap->Resolution();

    //Create a new transform object
    Transform *transform = new nocam2map (sampSol, lineSol, outmap,
                                          latCube, lonCube,
                                          ui.GetString("LATTYPE") == "PLANETOCENTRIC",
                                          ui.GetString("LONDIR") == "POSITIVEEAST",
                                          tolerance, ui.GetInteger("ITERATIONS"),
                                          inCube->Samples(), inCube->Lines(),
                                          samples, lines);
  
    //Allocate the output cube and add the mapping labels
    Cube *oCube = r.SetOutputCube ("TO", transform->OutputSamples(),
                                              transform->OutputLines(),
                                              inCube->Bands());
    oCube->PutGroup(mapGrp);

    //Determine which interpolation to use
    Interpolator *interp = NULL;
    if (ui.GetString("INTERP") == "NEARESTNEIGHBOR") {
      interp = new Interpolator(Interpolator::NearestNeighborType);
    }
    else if (ui.GetString("INTERP") == "BILINEAR") {
      interp = new Interpolator(Interpolator::BiLinearType);
    }
    else if (ui.GetString("INTERP") == "CUBICCONVOLUTION") {
      interp = new Interpolator(Interpolator::CubicConvolutionType);
    }
  
    //Warp the cube
    r.StartProcess(*transform, *interp);
    r.EndProcess();

    // add mapping to print.prt
    PvlGroup mapping = outmap->Mapping(); 
    Application::Log(mapping); 

    //Clean up
    delete latCube;
    delete lonCube;

    delete outmap;
    delete transform;
    delete interp;
  }
}


// Transform object constructor
nocam2map::nocam2map (LeastSquares sample, LeastSquares line, Projection *outmap,
                      Cube *latCube, Cube *lonCube,
                      bool isOcentric, bool isPosEast,
                      double tolerance, int iterations,
                      const int inputSamples, const int inputLines,
                      const int outputSamples, const int outputLines) {

  p_sampleSol = &sample;
  p_lineSol = &line;
  p_outmap = outmap;
  p_inputSamples = inputSamples;
  p_inputLines = inputLines;
  p_outputSamples = outputSamples;
  p_outputLines = outputLines;
  p_latCube = latCube;
  p_lonCube = lonCube;
  p_isOcentric = isOcentric;
  p_isPosEast = isPosEast;
  p_tolerance = tolerance;
  p_iterations = iterations;
  p_latCenter = p_latCube->Statistics()->Average() * PI/180.0;
  p_lonCenter = p_lonCube->Statistics()->Average() * PI/180.0;
  p_radius = p_outmap->LocalRadius(p_latCenter);
}



// Transform method mapping output line/samps to lat/lons to input line/samps
bool nocam2map::Xform (double &inSample, double &inLine,
                         const double outSample, const double outLine) {
  if (!p_outmap->SetWorld(outSample,outLine)) return false;

  if(outSample > p_outputSamples) return false;
  if(outLine > p_outputLines) return false;

  //Get the known latitude and longitudes from the projection
  //Convert to the input's latitude/longitude domain if necessary
  double lat_known, lon_known;

  if(p_outmap->IsPlanetocentric()) {
    if(!p_isOcentric) lat_known = p_outmap->ToPlanetographic(p_outmap->Latitude());
    else lat_known = p_outmap->Latitude();
  }
  else {
    if(p_isOcentric) lat_known = p_outmap->ToPlanetocentric(p_outmap->Latitude());
    else lat_known = p_outmap->Latitude();
  }

  if(p_outmap->IsPositiveEast()) {
    if(!p_isPosEast) lon_known = p_outmap->ToPositiveWest(p_outmap->Longitude(), 360);
    else lon_known = p_outmap->Longitude();
  }
  else {
    if(p_isPosEast) lon_known = p_outmap->ToPositiveEast(p_outmap->Longitude(), 360);
    else lon_known = p_outmap->Longitude();
  }

  lat_known *= PI/180.0;
  lon_known *= PI/180.0;

  //Project the known lat/long to x/y using the stereographic projection
  double k_known = 2/(1 + sin(p_latCenter) * sin(lat_known) + cos(p_latCenter)*cos(lat_known)*cos(lon_known - p_lonCenter));
  double x_known = k_known * cos(lat_known) * sin(lon_known - p_lonCenter);
  double y_known = k_known * (cos(p_latCenter) * sin(lat_known)) - (sin(p_latCenter) * cos(lat_known) * cos(lon_known - p_lonCenter));

  vector<double> data_known;
  data_known.push_back(x_known);
  data_known.push_back(y_known);

  //Get the sample/line guess from the least squares solutions
  double sample_guess = p_sampleSol->Evaluate(data_known);
  double line_guess = p_lineSol->Evaluate(data_known);

  //If the sample/line guess is out of bounds return false
  if (sample_guess < -1.5) return false;
  if (line_guess < -1.5) return false;
  if (sample_guess > p_inputSamples + 1.5) return false;
  if (line_guess > p_inputLines + 1.5) return false;

  if (sample_guess < 0.5) sample_guess = 1;
  if (line_guess < 0.5) line_guess = 1;
  if (sample_guess > p_inputSamples + 0.5) sample_guess = p_inputSamples; 
  if (line_guess > p_inputLines + 0.5) line_guess = p_inputLines;

  //Create a bilinear interpolator
  Interpolator interp(Interpolator::BiLinearType);

  //Create a 2x2 buffer to read the lat and long cubes
  Portal latPortal(interp.Samples(), interp.Lines(),
                        p_latCube->PixelType() ,
                        interp.HotSample(), interp.HotLine());

  Portal lonPortal(interp.Samples(), interp.Lines(),
                        p_lonCube->PixelType() ,
                        interp.HotSample(), interp.HotLine());

  //Set the buffers positions to the sample/line guess and read from the lat/long cubes
  latPortal.SetPosition (sample_guess, line_guess, 1);
  p_latCube->Read(latPortal);

  lonPortal.SetPosition (sample_guess, line_guess, 1);
  p_lonCube->Read(lonPortal);

  //Get the lat/long guess from the interpolator
  double lat_guess = interp.Interpolate(sample_guess, line_guess, latPortal.DoubleBuffer()) * PI/180.0;
  double lon_guess = interp.Interpolate(sample_guess, line_guess, lonPortal.DoubleBuffer()) * PI/180.0;

  //Project the lat/long guess to x/y using the stereographic projection
  double k_guess = 2/(1 + sin(p_latCenter) * sin(lat_guess) + cos(p_latCenter)*cos(lat_guess)*cos(lon_guess - p_lonCenter));
  double x_guess = k_guess * cos(lat_guess) * sin(lon_guess - p_lonCenter);
  double y_guess = k_guess * (cos(p_latCenter) * sin(lat_guess)) - (sin(p_latCenter) * cos(lat_guess) * cos(lon_guess - p_lonCenter));

  //Calculate the difference between the known x/y to the x/y from our least squares solutions
  double x_diff = abs(x_guess - x_known) * p_radius;
  double y_diff = abs(y_guess - y_known) * p_radius;

  //If the difference is above the tolerance, correct it until it is below the tolerance or we have iterated through a set amount of times
  int iteration = 0;
  while(x_diff > p_tolerance || y_diff > p_tolerance) {
    if(iteration++ >= p_iterations) return false;

    //Create a 1st order polynomial function
    PolynomialBivariate sampFunct(1); 
    PolynomialBivariate lineFunct(1);

    //Create a least squares solution 
    LeastSquares sampConverge(sampFunct);
    LeastSquares lineConverge(lineFunct);

    //Add the points around the line/sample guess point to the least squares matrix
    for(int i = (int)(line_guess + 0.5) -1; i <= (int)(line_guess + 0.5) + 1; i++) {
      //If the line is out of bounds, then skip it
      if (i < 1 || i > p_inputLines) continue;
      for(int j = (int)(sample_guess + 0.5) -1; j <= (int)(sample_guess + 0.5) + 1; j++) {
        //If the sample is out of bounds, then skip it
        if(j < 1 || j > p_inputSamples) continue;

        latPortal.SetPosition (j, i, 1);
        p_latCube->Read(latPortal);
        if(IsSpecial(latPortal.at(0))) continue;
        double n_lat = latPortal.at(0) * PI/180.0;

        lonPortal.SetPosition (j, i, 1);
        p_lonCube->Read(lonPortal);
        if(IsSpecial(lonPortal.at(0))) continue;   
        double n_lon = lonPortal.at(0) * PI/180.0;

        //Conver the lat/lon to x/y using the stereographic projection
        double n_k = 2/(1 + sin(p_latCenter) * sin(n_lat) + cos(p_latCenter)*cos(n_lat)*cos(n_lon - p_lonCenter));
        double n_x = n_k * cos(n_lat) * sin(n_lon - p_lonCenter);
        double n_y = n_k * (cos(p_latCenter) * sin(n_lat)) - (sin(p_latCenter) * cos(n_lat) * cos(n_lon - p_lonCenter));

        //Add the points to the least squares solution
        vector<double> data;
        data.push_back(n_x);
        data.push_back(n_y);
        sampConverge.AddKnown(data, j);
        lineConverge.AddKnown(data, i);
      }
    }
    //TODO: What if solve can't and throws an error?

    //Solve the least squares functions
    sampConverge.Solve(LeastSquares::QRD);
    lineConverge.Solve(LeastSquares::QRD);

    //Try to solve the known data with our new function
    sample_guess = sampConverge.Evaluate(data_known);
    line_guess = lineConverge.Evaluate(data_known);

    //If the new sample/line is out of bounds return false
    if (sample_guess < -1.5) return false;
    if (line_guess < -1.5) return false;
    if (sample_guess > p_inputSamples + 1.5) return false;
    if (line_guess > p_inputLines + 1.5) return false;
  
    if (sample_guess < 0.5) sample_guess = 1;
    if (line_guess < 0.5) line_guess = 1;
    if (sample_guess > p_inputSamples + 0.5) sample_guess = p_inputSamples; 
    if (line_guess > p_inputLines + 0.5) line_guess = p_inputLines;

    //Set the buffers positions to the sample/line guess and read from the lat/long cubes
    latPortal.SetPosition(sample_guess, line_guess, 1);
    p_latCube->Read(latPortal);

    lonPortal.SetPosition(sample_guess, line_guess, 1);
    p_lonCube->Read(lonPortal);

    //Get the lat/long guess from the interpolator
    lat_guess = interp.Interpolate(sample_guess, line_guess, latPortal.DoubleBuffer()) * PI/180.0;
    lon_guess = interp.Interpolate(sample_guess, line_guess, lonPortal.DoubleBuffer()) * PI/180.0;

    //Project the lat/long guess to x/y using the stereographic projection
    k_guess = 2/(1 + sin(p_latCenter) * sin(lat_guess) + cos(p_latCenter)*cos(lat_guess)*cos(lon_guess - p_lonCenter));
    x_guess = k_guess * cos(lat_guess) * sin(lon_guess - p_lonCenter);
    y_guess = k_guess * (cos(p_latCenter) * sin(lat_guess)) - (sin(p_latCenter) * cos(lat_guess) * cos(lon_guess - p_lonCenter));


    //Calculate the difference between the known x/y to the x/y from our least squares solutions
    x_diff = abs(x_guess - x_known) * p_radius;
    y_diff = abs(y_guess - y_known) * p_radius;
  }


  //Set the input sample/line to the sample/line we've determined to be the closest fit
  inSample = sample_guess;
  inLine = line_guess;
  return true;
}

int nocam2map::OutputSamples () const {
  return p_outputSamples;
}

int nocam2map::OutputLines () const {
  return p_outputLines;
}

// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

//Helper function to get camera resolution.
void ComputePixRes () {
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  Cube *latCube = p.SetInputCube("LATCUB");
  Cube *lonCube = p.SetInputCube("LONCUB");
  Brick latBrick(1,1,1,latCube->PixelType());
  Brick lonBrick(1,1,1,lonCube->PixelType());
  latBrick.SetBasePosition(1,1,1);
  latCube->Read(latBrick);

  lonBrick.SetBasePosition(1,1,1);
  lonCube->Read(lonBrick);
      
  double a = latBrick.at(0) * PI/180.0;
  double c = lonBrick.at(0) * PI/180.0;
  
  latBrick.SetBasePosition(latCube->Samples(),latCube->Lines(),1);
  latCube->Read(latBrick);

  lonBrick.SetBasePosition(lonCube->Samples(),lonCube->Lines(),1);     
  lonCube->Read(lonBrick);

  double b = latBrick.at(0) * PI/180.0;
  double d = lonBrick.at(0) * PI/180.0;

  double angle = acos(cos(a) * cos(b) * cos(c - d) + sin(a) * sin(b));
  angle *= 180/PI;

  double pixels = sqrt(pow(latCube->Samples() -1.0, 2.0) + pow(latCube->Lines() -1.0, 2.0));

  p.EndProcess();

  ui.Clear("RESOLUTION");
  ui.PutDouble("RESOLUTION", pixels/angle);

  ui.Clear("PIXRES");
  ui.PutAsString("PIXRES","PPD");
}

// Helper function to get mapping resolution.
void LoadMapRes () {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Set resolution
  if (userGrp.HasKeyword("Scale")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",userGrp["Scale"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES","PPD");
  }
  else if (userGrp.HasKeyword("PixelResolution")) {
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",userGrp["PixelResolution"]);
    ui.Clear("PIXRES");
    ui.PutAsString("PIXRES","MPP");
  }
  else {
    string msg = "No resolution value found in [" + ui.GetFilename("MAP") + "]";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }
}

//Helper function to compute input range.
void ComputeInputRange () {
  Process p;
  Cube *latCub = p.SetInputCube("LATCUB");
  Cube *lonCub = p.SetInputCube("LONCUB");

  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  Statistics *latStats = latCub->Statistics();
  Statistics *lonStats = lonCub->Statistics();

  double minLat = latStats->Minimum();
  double maxLat = latStats->Maximum();

  int lonDomain = userGrp.HasKeyword("LongitudeDomain") ? (int)userGrp.FindKeyword("LongitudeDomain") : 360;
  double minLon = lonDomain == 360 ? Projection::To360Domain(lonStats->Minimum()) : Projection::To180Domain(lonStats->Minimum());
  double maxLon = lonDomain == 360 ? Projection::To360Domain(lonStats->Maximum()) : Projection::To180Domain(lonStats->Maximum());

  if(userGrp.HasKeyword("LatitudeType")) {
    bool isOcentric = ((std::string)userGrp.FindKeyword("LatitudeType")) == "Planetocentric";

    double equRadius;
    double polRadius;

    //If the user entered the equatorial and polar radii
    if(ui.WasEntered("EQURADIUS") && ui.WasEntered("POLRADIUS")) {
      equRadius = ui.GetDouble("EQURADIUS");
      polRadius = ui.GetDouble("POLRADIUS");
    }
    //Else read them from the pck
    else {
      Filename pckFile("$base/kernels/pck/pck?????.tpc");
      pckFile.HighestVersion();

      string pckFilename = pckFile.Expanded();

      furnsh_c(pckFilename.c_str());

      string target;

      //If user entered target 
      if(ui.WasEntered("TARGET")) {
        target = ui.GetString("TARGET");
      }
      //Else read the target name from the input cube
      else {
        Pvl fromFile;
        fromFile.Read(ui.GetFilename("FROM"));
        target = (string)fromFile.FindKeyword("TargetName", Pvl::Traverse);
      }

      SpiceInt code;
      SpiceBoolean found;

      bodn2c_c (target.c_str(), &code, &found);

      if (!found) {
        string msg = "Could not convert Target [" + target +
                     "] to NAIF code";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }

      SpiceInt n;
      SpiceDouble radii[3];

      bodvar_c(code,"RADII",&n,radii);

      equRadius = radii[0] * 1000;
      polRadius = radii[2] * 1000;
    }

    if(isOcentric) {
      if(ui.GetString("LATTYPE") != "PLANETOCENTRIC") {
        minLat = Projection::ToPlanetocentric(minLat, (double)equRadius, (double)polRadius);
        maxLat = Projection::ToPlanetocentric(maxLat, (double)equRadius, (double)polRadius);
      }
    }
    else {
      if(ui.GetString("LATTYPE") == "PLANETOCENTRIC") {
        minLat = Projection::ToPlanetographic(minLat, (double)equRadius, (double)polRadius);
        maxLat = Projection::ToPlanetographic(maxLat, (double)equRadius, (double)polRadius);
      }
    }
  }

  if(userGrp.HasKeyword("LongitudeDirection")) {
    bool isPosEast = ((std::string)userGrp.FindKeyword("LongitudeDirection")) == "PositiveEast";

    if(isPosEast) {
      if(ui.GetString("LONDIR") != "POSITIVEEAST") {
        minLon = Projection::ToPositiveEast(minLon, lonDomain);
        maxLon = Projection::ToPositiveEast(maxLon, lonDomain);

        if(minLon > maxLon) {
          double temp = minLon;
          minLon = maxLon;
          maxLon = temp;
        }
      }
    }
    else {
      if(ui.GetString("LONDIR") == "POSITIVEEAST") {
        minLon = Projection::ToPositiveWest(minLon, lonDomain);
        maxLon = Projection::ToPositiveWest(maxLon, lonDomain);

        if(minLon > maxLon) {
          double temp = minLon;
          minLon = maxLon;
          maxLon = temp;
        }
      }
    }
  }

  // Set ground range parameters in UI
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT", minLat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT", maxLat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON", minLon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON", maxLon);

  p.EndProcess();

  // Set default ground range param to camera
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE","COMPUTE");
}

//Helper function to get ground range from map file.
void LoadMapRange() {
  UserInterface &ui = Application::GetUserInterface();

  // Get map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Set ground range keywords that are found in mapfile
  int count = 0;
  ui.Clear("MINLAT");
  ui.Clear("MAXLAT");
  ui.Clear("MINLON");
  ui.Clear("MAXLON");
  if (userGrp.HasKeyword("MinimumLatitude")) {
    ui.PutDouble("MINLAT",userGrp["MinimumLatitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MaximumLatitude")) {
    ui.PutDouble("MAXLAT",userGrp["MaximumLatitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MinimumLongitude")) {
    ui.PutDouble("MINLON",userGrp["MinimumLongitude"]);
    count++;
  }
  if (userGrp.HasKeyword("MaximumLongitude")) {
    ui.PutDouble("MAXLON",userGrp["MaximumLongitude"]);
    count++;
  }

  // Set default ground range param to map
  ui.Clear("DEFAULTRANGE");
  ui.PutAsString("DEFAULTRANGE","MAP");

  if (count < 4) {
    string msg = "One or more of the values for the ground range was not found";
    msg += " in [" + ui.GetFilename("MAP") + "]";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }
}
