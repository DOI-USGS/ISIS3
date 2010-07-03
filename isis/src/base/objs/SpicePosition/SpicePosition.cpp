#include <algorithm>
#include <cfloat>

#include "SpicePosition.h"
#include "BasisFunction.h"
#include "LeastSquares.h"
#include "LineEquation.h"
#include "NaifStatus.h"
#include "NumericalApproximation.h"
#include "PolynomialUnivariate.h"
#include "iException.h"

namespace Isis {
  /**
   * Construct an empty SpicePosition class using valid body codes.
   * See required reading
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/naif_ids.req
   *
   * @param targetCode Valid naif body name.
   * @param observerCode Valid naif body name.
   */
  SpicePosition::SpicePosition(int targetCode, int observerCode) {
    p_targetCode = targetCode;
    p_observerCode = observerCode;
    p_timeBias = 0.0;
    p_aberrationCorrection = "LT+S";
    p_source = Spice;
    p_coordinate.resize(3);
    p_velocity.resize(3);
    p_et = -DBL_MAX;
    p_noOverride = true;
    p_hasVelocity = false;
  }

  /** Apply a time bias when invoking SetEphemerisTime method.
   *
   * The bias is used only when reading from NAIF kernels.  It is added to the
   * ephermeris time passed into SetEphemerisTime and then the body
   * position is read from the NAIF kernels and returned.  When the cache
   * is loaded from a table the bias is ignored as it is assumed 
   * to have already been applied.  If this method is never called 
   * the default bias is 0.0 seconds. 
   *
   * @param timeBias time bias in seconds
   */
  void SpicePosition::SetTimeBias (double timeBias) {
    p_timeBias = timeBias;
  }

  /** Set the aberration correction (light time).
   * See NAIF required reading for more information on this correction at
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/spk.req
   *
   * @param correction  This value must be one of the following: "NONE", "LT",
   * "LT+S", where LT is a correction for planetary aberration (light time) and
   * S a correction for stellar aberration.  If never called the default is
   * "LT+S".
   */
  void SpicePosition::SetAberrationCorrection(const std::string &correction) {
    if (correction == "NONE" || correction == "LT" || correction == "LT+S") {
      p_aberrationCorrection = correction;
    }
    else {
      std::string msg = "Invalid abberation correction [" + correction + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }

  /** Return J2000 coordinate at given time.
   *
   * This method returns the J2000 coordinates (x,y,z) of the body at a given
   * et in seconds.  The coordinates are obtained from either a valid NAIF spk
   * kernel, or alternatively from an internal cache loaded from an ISIS Table
   * object.  In the first case, the SPK kernel must contain positions for
   * the body code specified in the constructor at the given time and it must
   * be loaded using the SpiceKernel class.
   *
   * @param et   ephemeris time in seconds 
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Moved code to
   *                      individual methods for each Source type
   *                      to make software more readable.
   */
  const std::vector<double> &SpicePosition::SetEphemerisTime(double et) {
    NaifStatus::CheckErrors();

    // Save the time
    if (et == p_et) return p_coordinate;
    p_et = et;

    // Read from the cache
    if (p_source == Memcache) { 
      SetEphemerisTimeMemcache();
    }
    else if(p_source == HermiteCache) {
      SetEphemerisTimeHermiteCache();
    }
    else {  // Read from the kernel
      SetEphemerisTimeSpice();
    }
    // Return the coordinate
    return p_coordinate;
    NaifStatus::CheckErrors();
  }

  /** Cache J2000 position over a time range.
   *
   * This method will load an internal cache with coordinates over a time
   * range.  This prevents
   * the NAIF kernels from being read over-and-over again and slowing a
   * application down due to I/O performance.  Once the
   * cache has been loaded then the kernels can be unloaded from the NAIF
   * system.
   *
   * @param startTime   Starting ephemeris time in seconds for the cache
   * @param endTime     Ending ephemeris time in seconds for the cache
   * @param size        Number of coordinates/positions to keep in the cache
   *
   */
  void SpicePosition::LoadCache (double startTime, double endTime, int size) {
    // Make sure cache isn't alread loaded
    if (p_source == Memcache || p_source == HermiteCache) {
      std::string msg = "A SpicePosition cache has already been created";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    if (startTime > endTime) {
      std::string msg = "Argument startTime must be less than or equal to endTime";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    if ((startTime != endTime) && (size == 1)) {
      std::string msg = "Cache size must be more than 1 if startTime endTime differ";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }


    // Loop and load the cache
    double cacheSlope = 0.0;
    if (size > 1) cacheSlope = (endTime - startTime) / (double) (size - 1);

    for (int i=0; i<size; i++) {
      double et = startTime + (double) i * cacheSlope;
      SetEphemerisTime(et);
      p_cache.push_back(p_coordinate);
      if (p_hasVelocity) p_cacheVelocity.push_back(p_velocity);
      p_cacheTime.push_back(p_et);
    }
    p_source = Memcache;
  }

  /** Cache J2000 position for a time.
   *
   * This method will load an internal cache with coordinates for a single
   * time (e.g. useful for framing cameras). This prevents
   * the NAIF kernels from being read over-and-over again and slowing a
   * application down due to I/O performance.  Once the
   * cache has been loaded then the kernels can be unloaded from the NAIF
   * system.  This calls the LoadCache(stime,etime,size) method using the
   * time as both the starting and ending time with a size of 1.
   *
   * @param time   single ephemeris time in seconds to cache
   *
   */
  void SpicePosition::LoadCache (double time) {
    LoadCache(time,time,1);
  }

  /** Cache J2000 positions using a table file.
   *
   * This method will load an internal cache with coordinates from an
   * ISIS table file.  The table must have 4 columns, 
   * or 7 (if velocity) is included, and at least one row. The 4 
   * columns contain the following information, body position 
   * x,y,z in J2000 and the ephemeris time of that position. If 
   * there are multiple rows it is assumed you can interpolate 
   * position at times in between the rows. 
   *
   * @param table   An ISIS table blob containing valid J2000 coordinate/time
   *                values
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Reads CacheType
   *            keyword from table and sets p_source.  If no
   *            CacheType keyword, we know this is an older image,
   *            so set p_source to Memcache.
   *  
   */
  void SpicePosition::LoadCache(Table &table) {

    // Make sure cache isn't alread loaded
    if (p_source == Memcache || p_source == HermiteCache) {
      std::string msg = "A SpicePosition cache has already been created";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  
    // set source type by table's label keyword  
    if(!table.Label().HasKeyword("CacheType")) {
      p_source = Memcache;
    }
    else if (table.Label().FindKeyword("CacheType")[0] == "Linear") { 
      p_source = Memcache;
    }
    else if (table.Label().FindKeyword("CacheType")[0] == "HermiteSpline") { 
      p_source = HermiteCache;
    }
    else {
      throw iException::Message(iException::Io, 
                                "Invalid value for CacheType keyword in the table " 
                                + table.Name(), 
                                _FILEINFO_);
    }

    // Loop through and move the table to the cache
    for (int r=0; r<table.Records(); r++) {
      TableRecord &rec = table[r];
      if (rec.Fields() == 7) {
        p_hasVelocity = true;
      }
      else if (rec.Fields() == 4) {
        p_hasVelocity = false;
      }
      else  {
        std::string msg = "Expecting four or seven fields in the SpicePosition table";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
      }
  
      std::vector<double> j2000Coord;
      j2000Coord.push_back((double)rec[0]);
      j2000Coord.push_back((double)rec[1]);
      j2000Coord.push_back((double)rec[2]);
      int inext = 3;
  
      p_cache.push_back(j2000Coord);
      if (p_hasVelocity) {
        std::vector<double> j2000Velocity;
        j2000Velocity.push_back((double)rec[3]);
        j2000Velocity.push_back((double)rec[4]);
        j2000Velocity.push_back((double)rec[5]);
        inext = 6;
  
        p_cacheVelocity.push_back(j2000Velocity);
      }
      p_cacheTime.push_back((double)rec[inext]);
    }
  }

  /** Return a table with J2000 positions.
   *
   * Return a table containg the cached coordinates with the given name.  The
   * table will have four or seven columns, J2000 x,y,z (optionally vx,vy,vx)
   * and the ephemeris time.
   *
   * @param tableName    Name of the table to create and return 
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Added CacheType
   *            keyword to output table.  This is based on
   *            p_source and will be read by LoadCache(Table).
   */
  Table SpicePosition::Cache(const std::string &tableName) {

    // determine type of table to return 
    std::string tabletype = "";
    if (p_source == Memcache) {
      tabletype = "Linear";
    }
    else if (p_source == HermiteCache) {
      tabletype = "HermiteSpline";
    }
    else {
      throw iException::Message(iException::Io, 
                                "Cannot create Table, no Cache is loaded.", 
                                _FILEINFO_);
    }

    // reacord to be added to table
    TableRecord record;

    // add x,y,z position labels to record
    TableField x("J2000X",TableField::Double);
    TableField y("J2000Y",TableField::Double);
    TableField z("J2000Z",TableField::Double);
    record += x;
    record += y;
    record += z;

    if ( p_hasVelocity ) {
      // add x,y,z velocity labels to record
      TableField vx("J2000XV",TableField::Double);
      TableField vy("J2000YV",TableField::Double);
      TableField vz("J2000ZV",TableField::Double);
      record += vx;
      record += vy;
      record += vz;
    }
    // add time label to record
    TableField t("ET",TableField::Double);
    record += t;

    // create output table
    Table table(tableName,record);

    int inext=0;
    for (int i=0; i<(int)p_cache.size(); i++) {
      record[inext++] = p_cache[i][0];  // record[0]
      record[inext++] = p_cache[i][1];  // record[1]
      record[inext++] = p_cache[i][2];  // record[2]
      if (p_hasVelocity) {
        record[inext++] = p_cacheVelocity[i][0];  // record[3]
        record[inext++] = p_cacheVelocity[i][1];  // record[4]
        record[inext++] = p_cacheVelocity[i][2];  // record[5]
      }
      record[inext] = p_cacheTime[i];  // record[6]
      table += record;
    
      inext = 0;
    }

    // set CacheType keyword in table's label
    table.Label() += PvlKeyword("CacheType", tabletype);

    return table;
  }



  /** Cache J2000 position over existing cached time range using
   *  polynomials
   *
   * This method will reload an internal cache with positions
   * formed from coordinates fit to polynomials over a time
   * range.
   *
   * @param function1   The first polynomial function used to
   *                    find the position coordinates
   * @param function2   The second polynomial function used to
   *                    find the position coordinates
   * @param function3   The third polynomial function used to
   *                    find the position coordinates
   */
  void SpicePosition::ReloadCache (Isis::PolynomialUnivariate &function1,
                                 Isis::PolynomialUnivariate &function2,
                                 Isis::PolynomialUnivariate &function3){
   // Make sure cache is already loaded
    if ( p_source != Memcache && p_source != HermiteCache ) {
      std::string msg = "A SpicePosition cache has not been created yet";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

   // Clear existing coordinates from cache
    p_cache.clear();
    p_cacheVelocity.clear();


    // Set velocity vector to true since it is calculated
    p_hasVelocity = true;

//    std::cout <<"time cache size is " << p_cacheTime.size() << std::endl;

    // Calculate new postition coordinates from polynomials fit to coordinates & 
    // fill cache
//    std::cout << "Before" << std::endl;
//    double savetime = p_cacheTime.at(0);
//    SetEphemerisTime(savetime);
//    std::cout << "     at time " << savetime << std::endl;
//    for (int i=0; i<3; i++) {
//      std::cout << p_coordinate[i] << std::endl;
//    }

    // find the first and last time values
    double firstTime = p_cacheTime[0];
    double lastTime = p_cacheTime[p_cacheTime.size()-1];

    if (p_cacheTime.size() != 1 ) {

      // find time for the extremum of each polynomial
      // since this is only a 2nd degree polynomial, 
      // finding these extrema is simple 
      double b1 = function1.Coefficient(1);
      double c1 = function1.Coefficient(2);
      double extremumXtime = -b1/(2.*c1) + p_baseTime; // extremum is time value for root of 1st derivative
      // make sure we are in the domain
      if (extremumXtime < firstTime) {
        extremumXtime = firstTime;
      }
      if (extremumXtime > lastTime) {
        extremumXtime = lastTime;
      }

      double b2 = function2.Coefficient(1);
      double c2 = function2.Coefficient(2);
      double extremumYtime = -b2/(2.*c2) + p_baseTime;
      // make sure we are in the domain
      if (extremumYtime < firstTime) {
        extremumYtime = firstTime;
      }
      if (extremumYtime > lastTime) {
        extremumYtime = lastTime;
      }

      double b3 = function3.Coefficient(1);
      double c3 = function3.Coefficient(2);
      double extremumZtime = -b3/(2.*c3) + p_baseTime;
      // make sure we are in the domain
      if (extremumZtime < firstTime) {
        extremumZtime = firstTime;
      }
      if (extremumZtime > lastTime) {
        extremumZtime = lastTime;
      }

      // refill the time vector
      p_cacheTime.clear();
      p_cacheTime.push_back(firstTime);
      p_cacheTime.push_back(extremumXtime);
      p_cacheTime.push_back(extremumYtime);
      p_cacheTime.push_back(extremumZtime);
      p_cacheTime.push_back(lastTime);
      // we don't know order of extrema, so sort
      sort(p_cacheTime.begin(),p_cacheTime.end());
      // in case an extremum is an endpoint
      std::vector <double>::iterator it = unique(p_cacheTime.begin(),p_cacheTime.end());
      p_cacheTime.resize(it - p_cacheTime.begin());

      if (p_cacheTime.size() == 2) {
        p_cacheTime.clear();
        p_cacheTime.push_back(firstTime);
        p_cacheTime.push_back((firstTime+lastTime)/2);
        p_cacheTime.push_back(lastTime);
      }

      // add positions and velocities for these times
      for (int i = 0; i < (int) p_cacheTime.size(); i++) {
        // x,y,z positions 
        std::vector <double> time;
        time.push_back(p_cacheTime[i] - p_baseTime);
        p_coordinate[0] = function1.Evaluate(time); 
        p_coordinate[1] = function2.Evaluate(time); 
        p_coordinate[2] = function3.Evaluate(time); 
        p_cache.push_back(p_coordinate);

        // x,y,z velocities
        p_velocity[0] = b1 + 2*c1*(p_cacheTime[i]-p_baseTime); 
        p_velocity[1] = b2 + 2*c2*(p_cacheTime[i]-p_baseTime); 
        p_velocity[2] = b3 + 2*c3*(p_cacheTime[i]-p_baseTime); 
        p_cacheVelocity.push_back(p_velocity);
      }
    }
    else {
      std::vector <double> time;
      time.push_back(p_cacheTime[0] - p_baseTime);
      p_coordinate[0] = function1.Evaluate(time); 
      p_coordinate[1] = function2.Evaluate(time); 
      p_coordinate[2] = function3.Evaluate(time); 
      p_cache.push_back(p_coordinate);
      p_velocity[0] = 0.;
      p_velocity[1] = 0.;
      p_velocity[2] = 0.;
      p_cacheVelocity.push_back(p_velocity);
    }

    double et = p_et;
    p_et = -DBL_MAX;
    SetEphemerisTime(et);

/*    std::cout << "After" << std::endl;
    std::cout << "     at time " << et << std::endl;
    for (int i=0; i<3; i++) {
      std::cout << p_coordinate[i] << std::endl;
    }*/

    return;
  }



  /** Set the coefficients of a polynomial (parabola) fit to each
   * of the components (X, Y, Z) of the position vector for the time period covered
   * by the cache, component = a + bt + ct**2, where t = time - p_baseTime.
   *
   */
  void SpicePosition::SetPolynomial () {
    int degree=2;
    Isis::PolynomialUnivariate function1(degree);       //!< Basis function fit to X
    Isis::PolynomialUnivariate function2(degree);       //!< Basis function fit to Y
    Isis::PolynomialUnivariate function3(degree);       //!< Basis function fit to Z
                                    //
    LeastSquares *fitX = new LeastSquares ( function1 );
    LeastSquares *fitY = new LeastSquares ( function2 );
    LeastSquares *fitZ = new LeastSquares ( function3 );

    // Compute the base time
    ComputeBaseTime ();
    std::vector<double> time;
    std::vector<double> XC,YC,ZC;

    if (p_cache.size() == 1) {
      double t = p_cacheTime.at(0);
      SetEphemerisTime( t );
      XC.push_back(p_coordinate[0]);
      XC.push_back(0.0);
      XC.push_back(0.0);
      YC.push_back(p_coordinate[1]);
      YC.push_back(0.0);
      YC.push_back(0.0);
      ZC.push_back(p_coordinate[2]);
      ZC.push_back(0.0);
      ZC.push_back(0.0);
    }
    else if (p_cache.size() == 2) {
// Load the times and get the corresponding coordinates
      double t1 = p_cacheTime.at(0);
      SetEphemerisTime( t1 );
      std::vector<double> coord1 = p_coordinate;
      t1 -= p_baseTime;
      double t2 = p_cacheTime.at(1);
      SetEphemerisTime( t2 );
      std::vector<double> coord2 = p_coordinate;
      t2 -= p_baseTime;
      double slope[3];
      double intercept[3];

// Compute the linear equation for each coordinate and save them
      for (int cIndex=0; cIndex < 3; cIndex++) {
        Isis::LineEquation posline(t1, coord1[cIndex], t2, coord2[cIndex]);
        slope[cIndex] = posline.Slope();
        intercept[cIndex] = posline.Intercept();
      }
      XC.push_back(intercept[0]);
      XC.push_back(slope[0]);
      XC.push_back(0.0);
      YC.push_back(intercept[1]);
      YC.push_back(slope[1]);
      YC.push_back(0.0);
      ZC.push_back(intercept[2]);
      ZC.push_back(slope[2]);
      ZC.push_back(0.0);
    }
    else {
    // Load the known values to compute the fit equation

      for (std::vector<double>::size_type pos=0;pos < p_cacheTime.size();pos++) {
        double t = p_cacheTime.at(pos);
        time.push_back( t - p_baseTime );
        SetEphemerisTime( t );
        std::vector<double> coord = p_coordinate;

        fitX->AddKnown ( time, coord[0] );
        fitY->AddKnown ( time, coord[1] );
        fitZ->AddKnown ( time, coord[2] );
        time.clear();
      }
      //Solve the equations for the coefficients
      fitX->Solve();
      fitY->Solve();
      fitZ->Solve();

      // Delete the least squares objects now that we have all the coefficients
      delete fitX;
      delete fitY;
      delete fitZ;

      // For now assume all three coordinates are fit to a parabola.  Later they may
      // each be fit to a unique basis function.
      // Fill the coefficient vectors

      for ( int i = 0;  i < function1.Coefficients(); i++) {
        XC.push_back( function1.Coefficient( i ) );
        YC.push_back( function2.Coefficient( i ) );
        ZC.push_back( function3.Coefficient( i ) );
      }

    }

    // Now that the coefficients have been calculated set the polynomial with them
    SetPolynomial ( XC, YC, ZC );
    return;
  }



  /** Set the coefficients of a polynomial (parabola) fit to
   * each of the three coordinates of the position vector for the
   * time period covered by the cache, coord = a + bt + ct**2,
   * where t = time - p_baseTime.
   *
   * @param [in] XC Coefficients of fit to X coordinate
   * @param [in] YC Coefficients of fit to Y coordinate
   * @param [in] ZC Coefficients of fit to Z coordinate
   *
   */
  void SpicePosition::SetPolynomial ( const std::vector<double>& XC,
                                      const std::vector<double>& YC,
                                      const std::vector<double>& ZC ) {
    Isis::PolynomialUnivariate function1( 2 );
    Isis::PolynomialUnivariate function2( 2 );
    Isis::PolynomialUnivariate function3( 2 );

    // Load the functions with the coefficients
    function1.SetCoefficients ( XC );
    function2.SetCoefficients ( YC );
    function3.SetCoefficients ( ZC );

    // Compute the base time
    ComputeBaseTime ();


//    std::cout << "Basetime=" << p_baseTime << std::endl;

    // Reload the cache from the functions and the currently cached time
    ReloadCache ( function1, function2, function3 );

    // Save the current coefficients
    p_coefficients[0] = XC;

//    std::cout << "Saved coef0="<<p_coefficients[0][0]<<" "<<p_coefficients[0][1]
//              <<p_coefficients[0][2]<<std::endl;

    p_coefficients[1] = YC;

//    std::cout << "Saved coef1="<<p_coefficients[1][0]<<" "<<p_coefficients[1][1]
//              <<p_coefficients[1][2]<<std::endl;

    p_coefficients[2] = ZC;
    return;
  }



  /**
   *  Return the coefficients of a polynomial (parabola) fit to each of the
   *  three coordinates of the position for the time period covered by the cache,
   *  angle = a + bt + ct**2, where t = time - p_basetime.
   *
   * @param [out] XC Coefficients of fit to first coordinate of position
   * @param [out] YC Coefficients of fit to second coordinate of position
   * @param [out] ZC Coefficients of fit to third coordinate of position
   *
   */
  void SpicePosition::GetPolynomial ( std::vector<double>& XC,
                                      std::vector<double>& YC,
                                      std::vector<double>& ZC ) {
    XC = p_coefficients[0];
    YC = p_coefficients[1];
    ZC = p_coefficients[2];

    return;
  }



  //! Compute the base time using cached times
  void SpicePosition::ComputeBaseTime () {
    if (p_noOverride) {
      p_baseTime = (p_cacheTime.at(0) + p_cacheTime.at(p_cacheTime.size()-1))/ 2.;
    }
    else {
      p_baseTime = p_overrideBaseTime;
    }
    return;
  }


  /**
   * Set an override base time to be used with observations on scanners to allow all
   * images in an observation to use the save base time and polynomials for the positions.
   * 
   * @param [in] baseTime The baseTime to use and override the computed base time
   */
  void SpicePosition::SetOverrideBaseTime( double baseTime ) {
    p_overrideBaseTime = baseTime;
    p_noOverride = false;
    return;
  }



  /** Set the coefficients of a polynomial (parabola) fit to each of the
   *  three coordinates of the position vector for the time period covered by the cache,
   *  coordinate = A + B*t + C*t**2, where t = time - p_basetime.
   *
   * @param partialVar     Variable output derivative vector is to be with respect to
   * @return               Derivative of j2000 vector calculated with polynomial 
   *                              with respect to partialVar
   *
   */
  std::vector<double> SpicePosition::CoordinatePartial( SpicePosition::PartialType partialVar, int coeffIndex) {
    // Start with a zero vector since the derivative of the other coordinates with
    // respect to the partial var will be 0.
    std::vector<double> coordinate(3,0);

    // Get the index of the coordinate to update with the partial derivative
    int coordIndex = partialVar;

    if (coeffIndex > 2) {
      std::string msg = "SpicePosition only supports up to a 2nd order fit for the spacecraft position";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Reset the coordinate to its derivative
    coordinate[coordIndex] = DPolynomial ( (Coefficient)  coeffIndex );
    return coordinate;
  }



  /** Compute the derivative of the velocity with respect to the specified variable.  The
   *  velocity is the derivative of the coordinate with respect to time
   *  coordinate = A + B*t + C*t**2, where t = time - p_basetime.
   *  velocity = B + 2*C*t
   *  partial(velocity) with respect to A = 0.
   *  partial(velocity) with respect to B = 1.
   *  partial(velocity) with respect to C = 2*t.
   *
   * @param partialVar     Variable output derivative vector is to be with respect to
   * @return               Derivative of j2000 velocity vector calculated with respect 
   *                        to partialVar
   *
   */
  std::vector<double> SpicePosition::VelocityPartial( SpicePosition::PartialType partialVar, int coeffIndex ) {
    // Start with a zero vector since the derivative of the other coordinates with
    // respect to the partial var will be 0.
    std::vector<double> dvelocity(3,0);

    // Get the index of the coordinate to update with the partial derivative
    int coordIndex = partialVar;

    double time = p_et - p_baseTime;
    double derivative = 0.;

    // Reset the velocity coordinate to its derivative
    switch (coeffIndex) {
    case A:
      break;
    case B:
      derivative = 1.;
    case C:
      derivative *= 2.*time;
      break;
    }
    dvelocity[coordIndex] = derivative;
    return dvelocity;
  }


  /**
   *  Evaluate the derivative of the fit polynomial (parabola) defined by the
   *  given coefficients with respect to the coefficient at the given index, at
   *  the current time.
   *
   * @param coeffIndex Index of coefficient to differentiate with respect
   *                         to
   * @return The derivative evaluated at the current time
   *
   */
  double SpicePosition::DPolynomial ( const Coefficient coeffIndex ) {
    double derivative=1.;
    double time = p_et - p_baseTime;

//    std::cout << "coeff index = " << coeffIndex << std::endl;

    switch (coeffIndex) {
    case C:
      derivative = time;
    case B:
      derivative *= time;
    case A:
      derivative *= 1.;
      break;
    }
    return derivative;
  }

  /** Return the velocity vector if available.
   *
   * @return The velocity vector evaluated at the current time
   */
  const std::vector<double> &SpicePosition::Velocity() {
    if (p_hasVelocity) {
      return p_velocity;
    }
    else {
      std::string msg = "No velocity vector available";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }



  /**
   * This is a protected method that is called by 
   * SetEphemerisTime() when Source type is Memcache.  It 
   * calculates J2000 coordinates (x,y,z) of the body that 
   * correspond to a given et in seconds. These coordinates are
   * obtained from an internal cache loaded from an ISIS Table 
   * object.  
   * @see SetEphemerisTime()
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version
   *            (moved code from SetEphemerisTime() to its own
   *            method)
   */
  void SpicePosition::SetEphemerisTimeMemcache() {
    // If the cache has only one position return it
    if (p_cache.size() == 1) {
      p_coordinate[0] = p_cache[0][0];
      p_coordinate[1] = p_cache[0][1];
      p_coordinate[2] = p_cache[0][2];

      if (p_hasVelocity) {
        p_velocity[0] = p_cacheVelocity[0][0];
        p_velocity[1] = p_cacheVelocity[0][1];
        p_velocity[2] = p_cacheVelocity[0][2];
      }
    }

    else {
      // Otherwise determine the interval to interpolate
      std::vector<double>::iterator pos;
      pos = upper_bound(p_cacheTime.begin(),p_cacheTime.end(),p_et);

      int cacheIndex;
      if (pos != p_cacheTime.end()) {
        cacheIndex = distance(p_cacheTime.begin(),pos);
        cacheIndex--;
      }
      else {
        cacheIndex = p_cacheTime.size() - 2;
      }

      if (cacheIndex < 0) cacheIndex = 0;

      // Interpolate the coordinate
      double mult = (p_et - p_cacheTime[cacheIndex]) /
                    (p_cacheTime[cacheIndex+1] - p_cacheTime[cacheIndex]);
      std::vector<double> p2 = p_cache[cacheIndex+1];
      std::vector<double> p1 = p_cache[cacheIndex];

      p_coordinate[0] = (p2[0] - p1[0]) * mult + p1[0];
      p_coordinate[1] = (p2[1] - p1[1]) * mult + p1[1];
      p_coordinate[2] = (p2[2] - p1[2]) * mult + p1[2];

      if (p_hasVelocity) {
        p2 = p_cacheVelocity[cacheIndex+1];
        p1 = p_cacheVelocity[cacheIndex];
        p_velocity[0] = (p2[0] - p1[0]) * mult + p1[0];
        p_velocity[1] = (p2[1] - p1[1]) * mult + p1[1];
        p_velocity[2] = (p2[2] - p1[2]) * mult + p1[2];
      }
    }
  }



  /**
   * This is a protected method that is called by 
   * SetEphemerisTime() when Source type is HermiteCache.  It 
   * calculates J2000 coordinates (x,y,z) of the body that 
   * correspond to a given et in seconds. These coordinates are
   * obtained by using a Hermite spline to interpolate values from
   * an internal reduced cache loaded from an ISIS Table object. 
   * @see SetEphemerisTime()
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version
   */
  void SpicePosition::SetEphemerisTimeHermiteCache() {

    // what if framing camera???

    ComputeBaseTime();

    NumericalApproximation xhermite(NumericalApproximation::CubicHermite);
    NumericalApproximation yhermite(NumericalApproximation::CubicHermite);
    NumericalApproximation zhermite(NumericalApproximation::CubicHermite);

    for (unsigned int i = 0; i < p_cache.size(); i++) {
      xhermite.AddData(p_cacheTime[i]-p_baseTime, p_cache[i][0]);    // add time, x-position to x spline
      yhermite.AddData(p_cacheTime[i]-p_baseTime, p_cache[i][1]);    // add time, y-position to y spline
      zhermite.AddData(p_cacheTime[i]-p_baseTime, p_cache[i][2]);    // add time, z-position to z spline

      if (p_hasVelocity) { // Line scan camera
        xhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][0]); // add x-velocity to x spline
        yhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][1]); // add y-velocity to y spline
        zhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][2]); // add z-velocity to z spline
      }
      else { // Not line scan camera
        throw iException::Message(iException::Io, "No velcities available.", _FILEINFO_);
        // xhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
        // yhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
        // zhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
      }
    }
    // Next line added 07/13/2009 to prevent Camera unit test from bombing because time is outside domain. DAC
    // Also added etype to Evaluate calls
    NumericalApproximation::ExtrapType etype=NumericalApproximation::Extrapolate;
    p_coordinate[0] = xhermite.Evaluate(p_et-p_baseTime, etype);
    p_coordinate[1] = yhermite.Evaluate(p_et-p_baseTime, etype);
    p_coordinate[2] = zhermite.Evaluate(p_et-p_baseTime, etype);
    
    p_velocity[0] = xhermite.EvaluateCubicHermiteFirstDeriv(p_et-p_baseTime);
    p_velocity[1] = yhermite.EvaluateCubicHermiteFirstDeriv(p_et-p_baseTime);
    p_velocity[2] = zhermite.EvaluateCubicHermiteFirstDeriv(p_et-p_baseTime);
  }

  /**
   * This is a protected method that is called by 
   * SetEphemerisTime() when Source type is Spice.  It 
   * calculates J2000 coordinates (x,y,z) of the body that 
   * correspond to a given et in seconds.  The coordinates are 
   * obtained from a valid NAIF spk kernel.  The SPK kernel must 
   * contain positions for the body code specified in the 
   * constructor at the given time and it must be loaded using the 
   * SpiceKernel class. 
   * @see SetEphemerisTime()
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version
   *            (moved code from SetEphemerisTime() to its own
   *            method)
   */
  void SpicePosition::SetEphemerisTimeSpice() {
    // Read from the kernel
    SpiceDouble j[6], lt;
    // First try getting the entire state (including the velocity vector)
    spkez_c ((SpiceInt)p_targetCode,
              (SpiceDouble)(p_et + p_timeBias),
              "J2000",
              p_aberrationCorrection.c_str(),
              (SpiceInt)p_observerCode,
              j,
              &lt);
    // If Naif fails attempting to get the entire state, assume the velocity vector is
    // not available and just get the position.  First turn off Naif error reporting and
    // return any error without printing them.
    SpiceBoolean spfailure = failed_c();
    reset_c();                   // Reset Naif error system to allow caller to recover
    if ( !spfailure ) {
      p_velocity[0] = j[3];
      p_velocity[1] = j[4];
      p_velocity[2] = j[5];
      p_hasVelocity = true;
    }
    else {
      spkezp_c ((SpiceInt)p_targetCode,
              (SpiceDouble)(p_et + p_timeBias),
              "J2000",
              p_aberrationCorrection.c_str(),
              (SpiceInt)p_observerCode,
              j,
              &lt);
    }
    p_coordinate[0] = j[0];
    p_coordinate[1] = j[1];
    p_coordinate[2] = j[2];
  }


  /**
   * This method reduces the cache for position, time and velocity 
   * to the minimum number of values needed to interpolate the 
   * J2000 coordinates using a Hermite spline, given a tolerance 
   * of deviation from the NAIF values. 
   * 
   * @param tolerance Maximum error allowed between NAIF kernel
   *                  coordinate values and values interpolated by
   *                  the Hermite spline.
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version.
   */
  void SpicePosition::Memcache2HermiteCache(double tolerance){
    if (p_source == HermiteCache) {
      return;
    }
    else if(p_source != Memcache) {
        throw iException::Message(iException::Programmer, 
                                  "Source type is not Memcache, cannot convert.", 
                                  _FILEINFO_);
    }

    // make sure base time is set before it is needed
    ComputeBaseTime();

    // find current size of cache
    int n = p_cacheTime.size()-1;

    // create 3 starting values for the new table
    vector <int> inputIndices;
    inputIndices.push_back(0);
    inputIndices.push_back(n/2);
    inputIndices.push_back(n);

    // find all indices needed to make a hermite table within the appropriate tolerance
    vector <int> indexList = HermiteIndices(tolerance,inputIndices);

    // remove all lines from cache vectors that are not in the index list????
    for (int i = n; i >= 0; i--) {
      if (!binary_search(indexList.begin(),indexList.end(),i)) {
        p_cache.erase(p_cache.begin()+i);
        p_cacheTime.erase(p_cacheTime.begin()+i);
        p_cacheVelocity.erase(p_cacheVelocity.begin()+i);
      }
    }

    p_source = HermiteCache;
  }

  /**
   * This method is called by Memcache2HermiteCache() to determine
   * which indices from the orginal cache should be saved in the 
   * reduced cache. It is a recursive method that starts with an 
   * index list of 3 elements (first, center and last index 
   * values) and adds values to this list if the tolerance is not 
   * met. 
   * 
   * @param tolerance Maximum error allowed between NAIF kernel
   *                  coordinate values and values interpolated by
   *                  the Hermite spline.
   * @param indexList Vector containing the list of indices to be 
   *                  kept in the cache.  This list grows as the
   *                  method is recursively called
   * @return <b>vector/<double/></b> Vector containing final list 
   *         of indices that will be kept for the Hermite cache.
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version.
   *   @history 2009-08-14 Debbie A. Cook - Corrected indexing
   *            error in loop.
   */
  vector<int> SpicePosition::HermiteIndices(double tolerance, vector <int> indexList){

    unsigned int n = indexList.size();

    NumericalApproximation xhermite(NumericalApproximation::CubicHermite);
    NumericalApproximation yhermite(NumericalApproximation::CubicHermite);
    NumericalApproximation zhermite(NumericalApproximation::CubicHermite);
    for (unsigned int i = 0; i < indexList.size(); i++) {
      xhermite.AddData(p_cacheTime[indexList[i]]-p_baseTime, p_cache[indexList[i]][0]);    // add time, x-position to x spline
      yhermite.AddData(p_cacheTime[indexList[i]]-p_baseTime, p_cache[indexList[i]][1]);    // add time, y-position to y spline
      zhermite.AddData(p_cacheTime[indexList[i]]-p_baseTime, p_cache[indexList[i]][2]);    // add time, z-position to z spline

      if (p_hasVelocity) { // Line scan camera
        xhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][0]); // add x-velocity to x spline
        yhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][1]); // add y-velocity to y spline
        zhermite.AddCubicHermiteDeriv(p_cacheVelocity[i][2]); // add z-velocity to z spline
      }
      else { // Not line scan camera
        throw iException::Message(iException::Io, "No velcities available.", _FILEINFO_);
        // xhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
        // yhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
        // zhermite.AddCubicHermiteDeriv(0.0); // spacecraft didn't move => velocity = 0
      }
    }

    // loop through the saved indices from the end
    for(unsigned int i = indexList.size()-1; i > 0; i--) {
      double xerror = 0;
      double yerror = 0;
      double zerror = 0;

      // check every value of the original kernel values within interval
      for (int line = indexList[i-1]+1; line < indexList[i]; line++) {

        // find the errors at each value
        xerror = fabs(xhermite.Evaluate(p_cacheTime[line]-p_baseTime) - p_cache[line][0]);
        yerror = fabs(yhermite.Evaluate(p_cacheTime[line]-p_baseTime) - p_cache[line][1]);
        zerror = fabs(zhermite.Evaluate(p_cacheTime[line]-p_baseTime) - p_cache[line][2]);

        if (xerror > tolerance || yerror > tolerance || zerror > tolerance) {
          // if any error is greater than tolerance, no need to continue looking, break
          break;
        }
      }

      if (xerror < tolerance && yerror < tolerance && zerror < tolerance) {
        // if errors are less than tolerance after looping interval, no new point is necessary
        continue;
      }
      else {
        // if any error is greater than tolerance, add midpoint of interval to indexList vector
        indexList.push_back((indexList[i]+indexList[i-1])/2);
      }
    }

    if (indexList.size() > n) {  
      sort(indexList.begin(), indexList.end());               
      indexList = HermiteIndices(tolerance,indexList);    
    }
    return indexList;
  }


  /** Cache J2000 position over existing cached time range using
   *  table
   *
   * This method will reload an internal cache with positions
   * formed from coordinates in a table
   *
   * @param table  An ISIS table blob containing valid J2000 
   *               coordinate/time values.
   * @internal 
   *   @history 2009-08-03 Jeannie Walldren - Original version. 
   */  
  void SpicePosition::ReloadCache(Table &table) {
    p_source = Spice;
    p_cacheTime.clear();
    p_cache.clear();
    p_cacheVelocity.clear();
    LoadCache(table);
  }
}






