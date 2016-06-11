#include "SpiceRotation.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include <QDebug>

#include "BasisFunction.h"
#include "IException.h"
#include "IString.h"
#include "LeastSquares.h"
#include "LineEquation.h"
#include "NaifStatus.h"
#include "PolynomialUnivariate.h"
#include "Quaternion.h"
#include "Table.h"
#include "TableField.h"

// Declarations for bindings for Naif Spicelib routines that do not have
// a wrapper
extern int refchg_(integer *frame1, integer *frame2, doublereal *et,
                   doublereal *rotate);
extern int frmchg_(integer *frame1, integer *frame2, doublereal *et,
                   doublereal *rotate);
extern int invstm_(doublereal *mat, doublereal *invmat);
// Temporary declarations for bindings for Naif supportlib routines
// These three declarations should be removed once supportlib is in Isis3
extern int ck3sdn(double sdntol, bool avflag, int *nrec,
                  double *sclkdp, double *quats, double *avvs,
                  int nints, double *starts, double *dparr,
                  int *intarr);

namespace Isis {
  /**
   * Construct an empty SpiceRotation class using a valid Naif frame code to
   * set up for getting rotation from Spice kernels.  See required reading
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/naif_ids.req
   *
   * @param frameCode Valid naif frame code.
   */
  SpiceRotation::SpiceRotation(int frameCode) {
    p_constantFrames.push_back(frameCode);
    p_timeBias = 0.0;
    p_source = Spice;
    p_CJ.resize(9);
    p_matrixSet = false;
    p_et = -DBL_MAX;
    p_degree = 2;
    p_degreeApplied = false;
    p_noOverride = true;
    p_axis1 = 3;
    p_axis2 = 1;
    p_axis3 = 3;
    p_minimizeCache = No;
    p_hasAngularVelocity = false;
    p_av.resize(3);
    p_fullCacheStartTime = 0;
    p_fullCacheEndTime = 0;
    p_fullCacheSize = 0;
  }

  /**
   * Construct an empty SpiceRotation object using valid Naif frame code and.
   * body code to set up for computing nadir rotation.  See required reading
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/ascii/individual_docs/naif_ids.req
   *
   * @param frameCode Valid naif frame code.
   * @param targetCode Valid naif body code.
   */
  SpiceRotation::SpiceRotation(int frameCode, int targetCode) {
    NaifStatus::CheckErrors();

    p_constantFrames.push_back(frameCode);
    p_targetCode = targetCode;
    p_timeBias = 0.0;
    p_source = Nadir;
    p_CJ.resize(9);
    p_matrixSet = false;
    p_et = -DBL_MAX;
    p_axisP = 3;
    p_degree = 2;
    p_degreeApplied = false;
    p_noOverride = true;
    p_axis1 = 3;
    p_axis2 = 1;
    p_axis3 = 3;
    p_minimizeCache = No;
    p_hasAngularVelocity = false;
    p_av.resize(3);
    p_fullCacheStartTime = 0;
    p_fullCacheEndTime = 0;
    p_fullCacheSize = 0;

    // Determine the axis for the velocity vector
    QString key = "INS" + toString(frameCode) + "_TRANSX";
    SpiceDouble transX[2];
    SpiceInt number;
    SpiceBoolean found;
    //Read starting at element 1 (skipping element 0)
    gdpool_c(key.toAscii().data(), 1, 2, &number, transX, &found);

    if (!found) {
      QString msg = "Cannot find [" + key + "] in text kernels";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    p_axisV = 2;
    if (transX[0] < transX[1]) p_axisV = 1;

    NaifStatus::CheckErrors();
  }


  /**
   * Construct a SpiceRotation object by copying from an existing one.
   */
  SpiceRotation::SpiceRotation(const SpiceRotation &rotToCopy) {
    p_cacheTime = rotToCopy.p_cacheTime;
    p_cache = rotToCopy.p_cache;
    p_cacheAv = rotToCopy.p_cacheAv;
    p_av = rotToCopy.p_av;
    p_degree = rotToCopy.p_degree;
    p_axis1 = rotToCopy.p_axis1;
    p_axis2 = rotToCopy.p_axis2;
    p_axis3 = rotToCopy.p_axis3;

    p_constantFrames = rotToCopy.p_constantFrames;
    p_timeFrames = rotToCopy.p_timeFrames;
    p_timeBias = rotToCopy.p_timeBias;

    p_et = rotToCopy.p_et;
    p_quaternion = rotToCopy.p_quaternion; 
    p_matrixSet = rotToCopy.p_matrixSet;
    p_source = rotToCopy.p_source;
    p_axisP = rotToCopy.p_axisP;
    p_axisV = rotToCopy.p_axisV;
    p_targetCode = rotToCopy.p_targetCode;
    p_baseTime = rotToCopy.p_baseTime;
    p_timeScale = rotToCopy.p_timeScale;
    p_degreeApplied = rotToCopy.p_degreeApplied;

//    for (std::vector<double>::size_type i = 0; i < rotToCopy.p_coefficients[0].size(); i++) 
    for (int i = 0; i < 3; i++) 
      p_coefficients[i] = rotToCopy.p_coefficients[i];

    p_noOverride = rotToCopy.p_noOverride;
    p_overrideBaseTime = rotToCopy.p_overrideBaseTime;
    p_overrideTimeScale = rotToCopy.p_overrideTimeScale;
    p_minimizeCache = rotToCopy.p_minimizeCache;
    p_fullCacheStartTime = rotToCopy.p_fullCacheStartTime;
    p_fullCacheEndTime = rotToCopy.p_fullCacheEndTime;
    p_fullCacheSize = rotToCopy.p_fullCacheSize;
    p_TC = rotToCopy.p_TC;

    p_CJ = rotToCopy.p_CJ;
    p_degree = rotToCopy.p_degree;
    p_hasAngularVelocity = rotToCopy.p_hasAngularVelocity;

  }


  /** 
   * Destructor for SpiceRotation object.
   */ 
  SpiceRotation::~SpiceRotation() { }

  /** 
   * Change the frame to the given frame code.  This method has no effect if
   * spice is cached. 
   * @param frameCode The integer-valued frame code 
   */ 
  void SpiceRotation::SetFrame(int frameCode) {
    p_constantFrames[0] = frameCode;
  }

  /** 
   * Accessor method that returns the frame code. This is the first value of the 
   * constant frames member variable. 
   *  
   * @return An integer value indicating the frame code. 
   */ 
  int SpiceRotation::Frame() {
    return p_constantFrames[0];
  }

  /** Apply a time bias when invoking SetEphemerisTime method.
   *
   * The bias is used only when reading from NAIF kernels.  It is added to the
   * ephermeris time passed into SetEphemerisTime and then the body
   * position is read from the NAIF kernels and returned.  When the cache
   * is loaded from
   * a table the bias is ignored as it is assumed to have already been
   * applied.  If this method is never called the default bias is 0.0
   * seconds.
   *
   * @param timeBias time bias in seconds
   */
  void SpiceRotation::SetTimeBias(double timeBias) {
    p_timeBias = timeBias;
  }

  /** Return the J2000 to reference frame quaternion at given time.
   *
   * This method returns the J2000 to reference frame rotational matrix at a
   * given et in seconds.  The quaternion is obtained from either valid NAIF ck
   * and/or fk, or alternatively from an internal cache loaded from an ISIS
   * Table object.  In the first case, the kernels must contain the rotation
   * for the frame specified in the constructor at the given time (as well as
   * all the intermediate frames going from the reference frame to J2000) and
   * they must be loaded using the SpiceKernel class.
   *
   * @param et   ephemeris time in seconds
   */
  void SpiceRotation::SetEphemerisTime(double et) {

    // Save the time
    if (p_et == et) return;
    p_et = et;

    // Read from the cache
    if (p_source == Memcache) {
      SetEphemerisTimeMemcache();
    }

    // Apply coefficients defining a function for each of the three camera angles and angular velocity if available
    else if (p_source == PolyFunction) {
      SetEphemerisTimePolyFunction();
    }
    // Apply coefficients defining a function for each of the three camera angles and angular velocity if available
    else if (p_source == PolyFunctionOverSpice) {
      SetEphemerisTimePolyFunctionOverSpice();
    }
    // Read from the kernel
    else if (p_source == Spice) {
      SetEphemerisTimeSpice();
      // Retrieve the J2000 (code=1) to reference rotation matrix
    }

    // Compute from Nadir
    else {
      SetEphemerisTimeNadir();
    }

    // Set the quaternion for this rotation
//   p_quaternion.Set ( p_CJ );
  }

  /** 
   * Accessor method to get current ephemeris time.
   * @return The current ephemeris time.
   */ 
  double SpiceRotation::EphemerisTime() const {
    return p_et;
  }

  /**
   * Checks if the cache is empty.
   * @return  Indicates whether this rotation cached.
   */
  bool SpiceRotation::IsCached() const {
    return (p_cache.size() > 0);
  }

  /**
   * Set the downsize status to minimize cache. 
   * @param status The DownsizeStatus enumeration value. 
   */ 
  void SpiceRotation::MinimizeCache(DownsizeStatus status) {
    p_minimizeCache = status;
  }

  /** Cache J2000 rotation quaternion over a time range.
   *
   * This method will load an internal cache with frames over a time
   * range.  This prevents the NAIF kernels from being read over-and-over
   * again and slowing an application down due to I/O performance.  Once the
   * cache has been loaded then the kernels can be unloaded from the NAIF
   * system.
   *
   * @param startTime   Starting ephemeris time in seconds for the cache
   * @param endTime     Ending ephemeris time in seconds for the cache
   * @param size        Number of frames to keep in the cache
   *
   */
  void SpiceRotation::LoadCache(double startTime, double endTime, int size) {

    // Check for valid arguments
    if (size <= 0) {
      QString msg = "Argument cacheSize must not be less or equal to zero";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (startTime > endTime) {
      QString msg = "Argument startTime must be less than or equal to endTime";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if ((startTime != endTime) && (size == 1)) {
      QString msg = "Cache size must be more than 1 if startTime endTime differ";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Make sure cache isn't already loaded
    if (p_source == Memcache) {
      QString msg = "A SpiceRotation cache has already been created";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Save full cache parameterssetis
    p_fullCacheStartTime = startTime;
    p_fullCacheEndTime = endTime;
    p_fullCacheSize = size;

    // Make sure the constant frame is loaded.  This method also does the frame trace.
    if (p_timeFrames.size() == 0) InitConstantRotation(startTime);

    LoadTimeCache();
    int cacheSize = p_cacheTime.size();

    // Loop and load the cache
    for (int i = 0; i < cacheSize; i++) {
      double et = p_cacheTime[i];
      SetEphemerisTime(et);
      p_cache.push_back(p_CJ);
      if (p_hasAngularVelocity) p_cacheAv.push_back(p_av);
    }
    p_source = Memcache;

// Downsize already loaded caches (both time and quats)
    if (p_minimizeCache == Yes  &&  cacheSize > 5) {
      LoadTimeCache();
    }
  }


  /** Cache J2000 to frame rotation for a time.
   *
   * This method will load an internal cache with a rotation for a single
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
  void SpiceRotation::LoadCache(double time) {
    LoadCache(time, time, 1);
  }

  /** Cache J2000 rotations using a table file.
   *
   * This method will load either an internal cache with rotations (quaternions)
   * or coefficients (for 3 polynomials defining the camera angles) from an
   * ISIS table file.  In the first case, the table must have 5 columns and
   * at least one row. The 5 columns contain the following information, J2000
   * to reference quaternion (q0, q1, q2, q3)  and the ephemeris time of that
   * position. If there are multiple rows, it is assumed the quaternions between
   * the rows can be interpolated.  In the second case, the table must have
   * three columns and at least two rows.  The three columns contain the
   * coefficients for each of the three camera angles.  Row one of the
   * table contains coefficient 0 (constant term) for angles 1, 2, and 3.
   * If the degree of the fit equation is greater than 1, row 2 contains
   * coefficient 1 (linear) for each of the three angles.  Row n contains
   * coefficient n-1 and the last row contains the time parameters, base time,
   * and time scale, and the degree of the polynomial.
   *
   * @param table   An ISIS table blob containing valid J2000 to reference
   *                quaternion/time values
   */
  void SpiceRotation::LoadCache(Table &table) {
    // Clear any existing cached data to make it reentrant (KJB 2011-07-20).
    p_timeFrames.clear();
    p_TC.clear();
    p_cache.clear();
    p_cacheTime.clear();
    p_cacheAv.clear();
    p_hasAngularVelocity = false;

    // Load the constant and time-based frame traces and the constant rotation
    if (table.Label().hasKeyword("TimeDependentFrames")) {
      PvlKeyword labelTimeFrames = table.Label()["TimeDependentFrames"];
      for (int i = 0; i < labelTimeFrames.size(); i++) {
        p_timeFrames.push_back(toInt(labelTimeFrames[i]));
      }
    }
    else {
      p_timeFrames.push_back(p_constantFrames[0]);
      p_timeFrames.push_back(J2000Code);
    }

    if (table.Label().hasKeyword("ConstantRotation")) {
      PvlKeyword labelConstantFrames = table.Label()["ConstantFrames"];
      p_constantFrames.clear();

      for (int i = 0; i < labelConstantFrames.size(); i++) {
        p_constantFrames.push_back(toInt(labelConstantFrames[i]));
      }
      PvlKeyword labelConstantRotation = table.Label()["ConstantRotation"];

      for (int i = 0; i < labelConstantRotation.size(); i++) {
        p_TC.push_back(toDouble(labelConstantRotation[i]));
      }
    }
    else {
      p_TC.resize(9);
      ident_c((SpiceDouble( *)[3]) &p_TC[0]);
    }

    // Load the full cache time information from the label if available
    if (table.Label().hasKeyword("CkTableStartTime")) {
      p_fullCacheStartTime = toDouble(table.Label().findKeyword("CkTableStartTime")[0]);
    }
    if (table.Label().hasKeyword("CkTableEndTime")) {
      p_fullCacheEndTime = toDouble(table.Label().findKeyword("CkTableEndTime")[0]);
    }
    if (table.Label().hasKeyword("CkTableOriginalSize")) {
      p_fullCacheSize = toInt(table.Label().findKeyword("CkTableOriginalSize")[0]);
    }

    int recFields = table[0].Fields();

    // Loop through and move the table to the cache.  Retrieve the first record to
    // establish the type of cache and then use the appropriate loop.

    // list table of quaternion and time
    if (recFields == 5) {
      for (int r = 0; r < table.Records(); r++) {
        TableRecord &rec = table[r];

        if (rec.Fields() != recFields) {
          // throw and error
        }

        std::vector<double> j2000Quat;
        j2000Quat.push_back((double)rec[0]);
        j2000Quat.push_back((double)rec[1]);
        j2000Quat.push_back((double)rec[2]);
        j2000Quat.push_back((double)rec[3]);

        Quaternion q(j2000Quat);
        std::vector<double> CJ = q.ToMatrix();
        p_cache.push_back(CJ);
        p_cacheTime.push_back((double)rec[4]);
      }
      p_source = Memcache;
    }

    // list table of quaternion, angular velocity vector, and time
    else if (recFields == 8) {
      for (int r = 0; r < table.Records(); r++) {
        TableRecord &rec = table[r];

        if (rec.Fields() != recFields) {
          // throw and error
        }

        std::vector<double> j2000Quat;
        j2000Quat.push_back((double)rec[0]);
        j2000Quat.push_back((double)rec[1]);
        j2000Quat.push_back((double)rec[2]);
        j2000Quat.push_back((double)rec[3]);


        Quaternion q(j2000Quat);
        std::vector<double> CJ = q.ToMatrix();
        p_cache.push_back(CJ);

        std::vector<double> av;
        av.push_back((double)rec[4]);
        av.push_back((double)rec[5]);
        av.push_back((double)rec[6]);
        p_cacheAv.push_back(av);

        p_cacheTime.push_back((double)rec[7]);
        p_hasAngularVelocity = true;
      }
      p_source = Memcache;
    }

    // coefficient table for angle1, angle2, and angle3
    else if (recFields == 3) {
      std::vector<double> coeffAng1, coeffAng2, coeffAng3;

      for (int r = 0; r < table.Records() - 1; r++) {
        TableRecord &rec = table[r];

        if (rec.Fields() != recFields) {
          // throw an error
        }
        coeffAng1.push_back((double)rec[0]);
        coeffAng2.push_back((double)rec[1]);
        coeffAng3.push_back((double)rec[2]);
      }

      // Take care of time parameters
      TableRecord &rec = table[table.Records()-1];
      double baseTime = (double)rec[0];
      double timeScale = (double)rec[1];
      double degree = (double)rec[2];
      SetPolynomialDegree((int) degree);
      SetOverrideBaseTime(baseTime, timeScale);
      SetPolynomial(coeffAng1, coeffAng2, coeffAng3);
      p_source = PolyFunction;
      if (degree > 0)  p_hasAngularVelocity = true;
      if (degree == 0  && p_cacheAv.size() > 0) p_hasAngularVelocity = true;
    }
    else  {
      QString msg = "Expecting either three, five, or eight fields in the SpiceRotation table";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }



  /** Cache J2000 rotation over existing cached time range using polynomials
   *
   * This method will reload an internal cache with matrices
   * formed from rotation angles fit to functions over a time
   * range.
   *
   */
  void SpiceRotation::ReloadCache() {
    // Save current et
    double et = p_et;
    p_et = -DBL_MAX;

    if (p_source == PolyFunction) {
    // Clear existing matrices from cache
      p_cacheTime.clear();
      p_cache.clear();

      // Clear the angular velocity cache if we can calculate it instead.  It can't be calculated for
      // functions of degree 0 (framing cameras), so keep the original av.  It is better than nothing.
      if (p_degree > 0  && p_cacheAv.size() > 1)  p_cacheAv.clear();

      // Load the time cache first
      p_minimizeCache = No;
      LoadTimeCache();

      if (p_fullCacheSize > 1) {
      // Load the matrix and av caches
        for (std::vector<double>::size_type pos = 0; pos < p_cacheTime.size(); pos++) {
          SetEphemerisTime(p_cacheTime.at(pos));
          p_cache.push_back(p_CJ);
          p_cacheAv.push_back(p_av);
        }
      }
      else {
      // Load the matrix for the single updated time instance
        SetEphemerisTime(p_cacheTime[0]);
        p_cache.push_back(p_CJ);
      }
    }
    else if (p_source == PolyFunctionOverSpice) {
      SpiceRotation tempRot(*this);

      std::vector<double>::size_type maxSize = p_fullCacheSize;

      // Clear the existing caches
      p_cache.clear();
      p_cacheTime.clear();
      p_cacheAv.clear();

      // Reload the time cache first
      p_minimizeCache = No;
      LoadTimeCache();

      for (std::vector<double>::size_type pos = 0; pos < maxSize; pos++) {
        tempRot.SetEphemerisTime(p_cacheTime.at(pos));
        p_cache.push_back(tempRot.TimeBasedMatrix());
        if (p_hasAngularVelocity) p_cacheAv.push_back(tempRot.AngularVelocity());
      }
    }
    else { //(p_source < PolyFunction) 
      QString msg = "The SpiceRotation has not yet been fit to a function";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Set source to cache and reset current et
    // Make sure source is Memcache now
    p_source = Memcache;
    p_et = -DBL_MAX;
    SetEphemerisTime(et);
  }



  /** Return a table with J2000 to reference rotations.
   *
   * Return a table containing the cached pointing with the given
   * name. The table will have eight columns, quaternio, angular
   * velocity, and time of J2000 to reference frame rotation.
   *
   * @param tableName    Name of the table to create and return
   */
  Table SpiceRotation::LineCache(const QString &tableName) {

    // Apply the function and fill the caches
    if (p_source >= PolyFunction)  ReloadCache();

    if (p_source != Memcache) {
      QString msg = "Only cached rotations can be  returned as a line cache of quaternions and time";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    // Load the table and return it to caller
    return Cache(tableName);
  }



  /** Return a table with J2000 to reference rotations.
   *
   * Return a table containing the cached pointing with the given
   * name. The table will have either five columns (for a list cache)
   * of J2000 to reference quaternions and times, eight columns (if
   * angular velocity is available), or three columns
   * (for a coefficient cache), of J2000 to reference frame rotation
   * angles defined by coefficients of a polynomial function (see
   * SetPolynommial).  In the coefficient cache the last row of
   * the table is the base time, time scale, and polynomial degree.
   * Note:  In the case of the coefficient cache, the angular
   * velocity is not written to the table since it can be calculated
   * from the polynomials.
   *
   * @param tableName    Name of the table to create and return
   */
  Table SpiceRotation::Cache(const QString &tableName) {
   // First handle conversion of PolyFunctionOverSpiceConstant
    // by converting it to the full Memcache and try to downsize it
    if (p_source == PolyFunctionOverSpice) {
      LineCache(tableName);

      //std::cout << "Full cache size is " << p_cache.size() << endl;

      p_minimizeCache = Yes;
      LoadTimeCache();

      //std::cout << "Minimized cache size is " << p_cache.size() << endl;
    }

    // Load the list of rotations and their corresponding times
    if (p_source == Memcache) {
      TableField q0("J2000Q0", TableField::Double);
      TableField q1("J2000Q1", TableField::Double);
      TableField q2("J2000Q2", TableField::Double);
      TableField q3("J2000Q3", TableField::Double);
      TableField t("ET", TableField::Double);

      TableRecord record;
      record += q0;
      record += q1;
      record += q2;
      record += q3;
      int timePos = 4;

      if (p_hasAngularVelocity) {
        TableField av1("AV1", TableField::Double);
        TableField av2("AV2", TableField::Double);
        TableField av3("AV3", TableField::Double);
        record += av1;
        record += av2;
        record += av3;
        timePos = 7;
      }

      record += t;
      Table table(tableName, record);

      for (int i = 0; i < (int)p_cache.size(); i++) {
        Quaternion q(p_cache[i]);
        std::vector<double> v = q.GetQuaternion();
        record[0] = v[0];
        record[1] = v[1];
        record[2] = v[2];
        record[3] = v[3];

        if (p_hasAngularVelocity) {
          record[4] = p_cacheAv[i][0];
          record[5] = p_cacheAv[i][1];
          record[6] = p_cacheAv[i][2];
        }

        record[timePos] = p_cacheTime[i];
        table += record;
      }

      CacheLabel(table);
      return table;
    }
    // Just load the position for the single epoch
    else if (p_source == PolyFunction  &&  p_degree == 0  &&  p_fullCacheSize == 1) {
      return LineCache(tableName);
    }
    // Load the coefficients for the curves fit to the 3 camera angles
    else if (p_source == PolyFunction) {
      TableField angle1("J2000Ang1", TableField::Double);
      TableField angle2("J2000Ang2", TableField::Double);
      TableField angle3("J2000Ang3", TableField::Double);

      TableRecord record;
      record += angle1;
      record += angle2;
      record += angle3;

      Table table(tableName, record);

      for (int cindex = 0; cindex < p_degree + 1; cindex++) {
        record[0] = p_coefficients[0][cindex];
        record[1] = p_coefficients[1][cindex];
        record[2] = p_coefficients[2][cindex];
        table += record;
      }

      // Load one more table entry with the time adjustments for the fit equation
      // t = (et - baseTime)/ timeScale
      record[0] = p_baseTime;
      record[1] = p_timeScale;
      record[2] = (double) p_degree;

      table += record;
      CacheLabel(table);
      return table;
    }
    else {
      // throw an error -- should not get here -- invalid Spice Source
      QString msg = "To create table source of data must be either Memcache or PolyFunction";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }



  /** Add labels to a SpiceRotation table.
   *
   * Return a table containing the labels defining the rotation.
   *
   * @param Table    Table to receive labels
   */
  void SpiceRotation::CacheLabel(Table &table) {
    NaifStatus::CheckErrors();
    // Load the constant and time-based frame traces and the constant rotation
    // into the table as labels
    if (p_timeFrames.size() > 1) {
      table.Label() += PvlKeyword("TimeDependentFrames");

      for (int i = 0; i < (int) p_timeFrames.size(); i++) {
        table.Label()["TimeDependentFrames"].addValue(toString(p_timeFrames[i]));
      }
    }

    if (p_constantFrames.size() > 1) {
      table.Label() += PvlKeyword("ConstantFrames");

      for (int i = 0; i < (int) p_constantFrames.size(); i++) {
        table.Label()["ConstantFrames"].addValue(toString(p_constantFrames[i]));
      }

      table.Label() += PvlKeyword("ConstantRotation");

      for (int i = 0; i < (int) p_TC.size(); i++) {
        table.Label()["ConstantRotation"].addValue(toString(p_TC[i]));
      }
    }

    // Write original time coverage
    if (p_fullCacheStartTime != 0) {
      table.Label() += PvlKeyword("CkTableStartTime");
      table.Label()["CkTableStartTime"].addValue(toString(p_fullCacheStartTime));
    }
    if (p_fullCacheEndTime != 0) {
      table.Label() += PvlKeyword("CkTableEndTime");
      table.Label()["CkTableEndTime"].addValue(toString(p_fullCacheEndTime));
    }
    if (p_fullCacheSize != 0) {
      table.Label() += PvlKeyword("CkTableOriginalSize");
      table.Label()["CkTableOriginalSize"].addValue(toString(p_fullCacheSize));
    }
    NaifStatus::CheckErrors();
  }


  /** 
   * Return the camera angles at the center time of the observation
   */
  std::vector<double> SpiceRotation::GetCenterAngles() {
    // Compute the center time
    double etCenter = (p_fullCacheEndTime + p_fullCacheStartTime) / 2.;
    SetEphemerisTime(etCenter);

    return Angles(p_axis3, p_axis2, p_axis1);
  }


  /** 
   * Return the camera angles (right ascension, declination, and twist) for the 
   * time-based matrix CJ 
   *
   */
  std::vector<double> SpiceRotation::Angles(int axis3, int axis2, int axis1) {
    NaifStatus::CheckErrors();

    SpiceDouble ang1, ang2, ang3;
    m2eul_c((SpiceDouble *) &p_CJ[0], axis3, axis2, axis1, &ang3, &ang2, &ang1);

    std::vector<double> angles;
    angles.push_back(ang1);
    angles.push_back(ang2);
    angles.push_back(ang3);

    NaifStatus::CheckErrors();
    return angles;
  }

  /** 
   * Accessor method to get the angular velocity 
   */ 
  std::vector<double> SpiceRotation::AngularVelocity() {
    return p_av;
  }

  /** 
   * Accessor method to get the frame chain for the constant part of the 
   * rotation (ends in target) 
   * @return The frame chain for the constant part of the rotation. 
   */ 
  std::vector<int>  SpiceRotation::ConstantFrameChain() {
    return p_constantFrames;
  }

  /** 
   * Accessor method to get the frame chain for the rotation (begins in J2000).
   * @return The frame chain for the rotation.
   */ 
  std::vector<int>  SpiceRotation::TimeFrameChain() {
    return p_timeFrames;
  }

  /** 
   * Checks whether the rotation has angular velocities.  
   * @return Indicates whether the rotation has angular velocities.
   */ 
  bool SpiceRotation::HasAngularVelocity() {
    return p_hasAngularVelocity;
  }

  /** Given a direction vector in the reference frame, return a J2000 direction.
   *
   * @param [in]  rVec A direction vector in the reference frame
   *
   * @return (vector<double>)   A direction vector in J2000 frame
   */
  std::vector<double> SpiceRotation::J2000Vector(const std::vector<double>& rVec) {
    NaifStatus::CheckErrors();

    std::vector<double> jVec;

    if (rVec.size() == 3) {
      double TJ[3][3];
      mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], TJ);
      jVec.resize(3);
      mtxv_c(TJ, (SpiceDouble *) &rVec[0], (SpiceDouble *) &jVec[0]);
    }
    else if (rVec.size() == 6) {
      // See Naif routine frmchg for the format of the state matrix.  The constant rotation, TC,
      // has a derivative with respect to time of I.
      if (!p_hasAngularVelocity) {
        // throw an error
      }
      std::vector<double> stateTJ(36);
      stateTJ = StateTJ();

      // Now invert (inverse of a state matrix is NOT simply the transpose)
      xpose6_c(&stateTJ[0], (SpiceDouble( *) [6]) &stateTJ[0]);
      double stateJT[6][6];
      invstm_((doublereal *) &stateTJ[0], (doublereal *) stateJT);
      xpose6_c(stateJT, stateJT);
      jVec.resize(6);

      mxvg_c(stateJT, (SpiceDouble *) &rVec[0], 6, 6, (SpiceDouble *) &jVec[0]);
    }

    NaifStatus::CheckErrors();
    return (jVec);
  }


  /** Given a direction vector in J2000, return a reference frame direction.
   *
   * @param [in] jVec A direction vector in J2000
   *
   * @return (vector<double>)   A direction vector in reference
   *         frame
   */
  std::vector<double>
  SpiceRotation::ReferenceVector(const std::vector<double>& jVec) {
    NaifStatus::CheckErrors();

    std::vector<double> rVec(3);

    if (jVec.size() == 3) {
      double TJ[3][3];
      mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], TJ);
      rVec.resize(3);
      mxv_c(TJ, (SpiceDouble *) &jVec[0], (SpiceDouble *) &rVec[0]);
    }
    else if (jVec.size() == 6) {
      // See Naif routine frmchg for the format of the state matrix.  The constant rotation, TC,
      // has a derivative with respect to time of I.
      if (!p_hasAngularVelocity) {
        // throw an error
      }
      std::vector<double>  stateTJ(36);
      stateTJ = StateTJ();
      rVec.resize(6);
      mxvg_c((SpiceDouble *) &stateTJ[0], (SpiceDouble *) &jVec[0], 6, 6, (SpiceDouble *) &rVec[0]);
    }

    NaifStatus::CheckErrors();
    return (rVec);
  }


  /** Set the coefficients of a polynomial fit to each
   * of the three camera angles for the time period covered by the
   * cache, angle = a + bt + ct**2, where t = (time - p_baseTime)/ p_timeScale.
   *
   * @internal
   *  @history 2012-05-01 Debbie A. Cook - Added type argument to allow other function types
   *                                       beyond PolyFunction
   */
  void SpiceRotation::SetPolynomial(const Source type) {
    NaifStatus::CheckErrors();
    std::vector<double> coeffAng1, coeffAng2, coeffAng3;

    // Rotation is already stored as a polynomial -- throw an error
    if (p_source == PolyFunction) {
      // Nothing to do
      return;
//      QString msg = "Rotation already fit to a polynomial -- spiceint first to refit";
//      throw IException(IException::User,msg,_FILEINFO_);
    }

    // Adjust degree of polynomial on available data
    if (p_cache.size() == 1) {
      p_degree = 0;
    }
    else if (p_cache.size() == 2) {
      p_degree = 1;
    }

    //Check for polynomial over original pointing constant and initialize coefficients
    if (type == PolyFunctionOverSpice) {
      coeffAng1.assign(p_degree + 1, 0.);
      coeffAng2.assign(p_degree + 1, 0.);
      coeffAng3.assign(p_degree + 1, 0.);
      SetPolynomial(coeffAng1, coeffAng2, coeffAng3, type);
      return;
    }

    Isis::PolynomialUnivariate function1(p_degree);       //!< Basis function fit to 1st rotation angle
    Isis::PolynomialUnivariate function2(p_degree);       //!< Basis function fit to 2nd rotation angle
    Isis::PolynomialUnivariate function3(p_degree);       //!< Basis function fit to 3rd rotation angle
    //
    LeastSquares *fitAng1 = new LeastSquares(function1);
    LeastSquares *fitAng2 = new LeastSquares(function2);
    LeastSquares *fitAng3 = new LeastSquares(function3);

    // Compute the base time
    ComputeBaseTime();
    std::vector<double> time;

    if (p_cache.size() == 1) {
      double t = p_cacheTime.at(0);
      SetEphemerisTime(t);
      std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);
      coeffAng1.push_back(angles[0]);
      coeffAng2.push_back(angles[1]);
      coeffAng3.push_back(angles[2]);
    }
    else if (p_cache.size() == 2) {
// Load the times and get the corresponding rotation angles
      p_degree = 1;
      double t1 = p_cacheTime.at(0);
      SetEphemerisTime(t1);
      t1 -= p_baseTime;
      t1 = t1 / p_timeScale;
      std::vector<double> angles1 = Angles(p_axis3, p_axis2, p_axis1);
      double t2 = p_cacheTime.at(1);
      SetEphemerisTime(t2);
      t2 -= p_baseTime;
      t2 = t2 / p_timeScale;
      std::vector<double> angles2 = Angles(p_axis3, p_axis2, p_axis1);
      angles2[0] = WrapAngle(angles1[0], angles2[0]);
      angles2[2] = WrapAngle(angles1[2], angles2[2]);
      double slope[3];
      double intercept[3];

// Compute the linear equation for each angle and save them
      for (int angleIndex = 0; angleIndex < 3; angleIndex++) {
        Isis::LineEquation angline(t1, angles1[angleIndex], t2, angles2[angleIndex]);
        slope[angleIndex] = angline.Slope();
        intercept[angleIndex] = angline.Intercept();
      }
      coeffAng1.push_back(intercept[0]);
      coeffAng1.push_back(slope[0]);
      coeffAng2.push_back(intercept[1]);
      coeffAng2.push_back(slope[1]);
      coeffAng3.push_back(intercept[2]);
      coeffAng3.push_back(slope[2]);
    }
    else {
      // Load the known values to compute the fit equation
      double start1 = 0.; // value of 1st angle1 in cache
      double start3 = 0.; // value of 1st angle1 in cache

      for (std::vector<double>::size_type pos = 0; pos < p_cacheTime.size(); pos++) {
        double t = p_cacheTime.at(pos);
        time.push_back((t - p_baseTime) / p_timeScale);
        SetEphemerisTime(t);
        std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);

// Fix 180/-180 crossovers on angles 1 and 3 before doing fit.
        if (pos == 0) {
          start1 = angles[0];
          start3 = angles[2];
        }
        else {
          angles[0] = WrapAngle(start1, angles[0]);
          angles[2] = WrapAngle(start3, angles[2]);
        }

        fitAng1->AddKnown(time, angles[0]);
        fitAng2->AddKnown(time, angles[1]);
        fitAng3->AddKnown(time, angles[2]);
        time.clear();

      }
      //Solve the equations for the coefficients
      fitAng1->Solve();
      fitAng2->Solve();
      fitAng3->Solve();

      // Delete the least squares objects now that we have all the coefficients
      delete fitAng1;
      delete fitAng2;
      delete fitAng3;

      // For now assume all three angles are fit to a polynomial.  Later they may
      // each be fit to a unique basis function.
      // Fill the coefficient vectors

      for (int i = 0;  i < function1.Coefficients(); i++) {
        coeffAng1.push_back(function1.Coefficient(i));
        coeffAng2.push_back(function2.Coefficient(i));
        coeffAng3.push_back(function3.Coefficient(i));
      }

    }

    // Now that the coefficients have been calculated set the polynomial with them
    SetPolynomial(coeffAng1, coeffAng2, coeffAng3);

    NaifStatus::CheckErrors();
    return;
  }



  /** Set the coefficients of a polynomial fit to each of the
   * three camera angles for the time period covered by the
   * cache, angle = c0 + c1*t + c2*t**2 + ... + cn*t**n,
   * where t = (time - p_baseTime) / p_timeScale, and n = p_degree.
   *
   * @param [in] coeffAng1 Coefficients of fit to Angle 1
   * @param [in] coeffAng2 Coefficients of fit to Angle 2
   * @param [in] coeffAng3 Coefficients of fit to Angle 3
   *
   * @internal
   *  @history 2012-05-01 Debbie A. Cook - Added type argument to allow other function types
   */
  void SpiceRotation::SetPolynomial(const std::vector<double>& coeffAng1,
                                    const std::vector<double>& coeffAng2,
                                    const std::vector<double>& coeffAng3,
                                    const Source type) {

    NaifStatus::CheckErrors();
    Isis::PolynomialUnivariate function1(p_degree);
    Isis::PolynomialUnivariate function2(p_degree);
    Isis::PolynomialUnivariate function3(p_degree);

    // Load the functions with the coefficients
    function1.SetCoefficients(coeffAng1);
    function2.SetCoefficients(coeffAng2);
    function3.SetCoefficients(coeffAng3);

    // Compute the base time
    ComputeBaseTime();

    // Save the current coefficients
    p_coefficients[0] = coeffAng1;
    p_coefficients[1] = coeffAng2;
    p_coefficients[2] = coeffAng3;

    // Set the flag indicating p_degree has been applied to the camera angles, the
    // coefficients of the polynomials have been saved, and the cache reloaded from
    // the polynomials
    p_degreeApplied = true;
    //    p_source = PolyFunction;
    p_source = type;

    // Update the current rotation
    double et = p_et;
    p_et = -DBL_MAX;
    SetEphemerisTime(et);

    NaifStatus::CheckErrors();
    return;
  }



  /**
   *  Return the coefficients of a polynomial fit to each of the
   *  three camera angles for the time period covered by the cache, angle =
   *  c0 + c1*t + c2*t**2 + ... + cn*t**n, where t = (time - p_basetime) / p_timeScale
   *  and n = p_degree.
   *
   * @param [out] coeffAng1 Coefficients of fit to Angle 1
   * @param [out] coeffAng2 Coefficients of fit to Angle 2
   * @param [out] coeffAng3 Coefficients of fit to Angle 3
   *
   */
  void SpiceRotation::GetPolynomial(std::vector<double>& coeffAng1,
                                    std::vector<double>& coeffAng2,
                                    std::vector<double>& coeffAng3) {
    coeffAng1 = p_coefficients[0];
    coeffAng2 = p_coefficients[1];
    coeffAng3 = p_coefficients[2];

    return;
  }



  //! Compute the base time using cached times
  void SpiceRotation::ComputeBaseTime() {
    if (p_noOverride) {
      p_baseTime = (p_cacheTime.at(0) + p_cacheTime.at(p_cacheTime.size() - 1)) / 2.;
      p_timeScale = p_baseTime - p_cacheTime.at(0);
      // Take care of case where 1st and last times are the same
      if (p_timeScale == 0)  p_timeScale = 1.0;
    }
    else {
      p_baseTime = p_overrideBaseTime;
      p_timeScale = p_overrideTimeScale;
    }

    return;
  }


  /**
   * Set an override base time to be used with observations on scanners to allow all
   * images in an observation to use the save base time and polynomials for the angles.
   *
   * @param [in] baseTime The baseTime to use and override the computed base time
   */
  void SpiceRotation::SetOverrideBaseTime(double baseTime, double timeScale) {
    p_overrideBaseTime = baseTime;
    p_overrideTimeScale = timeScale;
    p_noOverride = false;
    return;
  }



  /**
   *  Evaluate the derivative of the fit polynomial defined by the
   *  given coefficients with respect to the coefficient at the given index, at
   *  the current time.
   *
   * @param coeffIndex The index of the coefficient to differentiate
   * @return The derivative evaluated at the current time
   *
   */
  double SpiceRotation::DPolynomial(const int coeffIndex) {
    double derivative;
    double time = (p_et - p_baseTime) / p_timeScale;

    if (coeffIndex > 0  && coeffIndex <= p_degree) {
      derivative = pow(time, coeffIndex);
    }
    else if (coeffIndex == 0) {
      derivative = 1;
    }
    else {
      Isis::IString msg = "Coeff index, " + Isis::IString(coeffIndex) + " exceeds degree of polynomial";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    return derivative;
  }



  /** Compute the derivative with respect to one of the coefficients in the
   *  angle polynomial fit equation of a vector rotated from J2000 to a
   *  reference frame.  The polynomial equation is of the form
   *  angle = c0 + c1*t + c2*t**2 + ... cn*t**n, where t = (time - p_basetime) / p_timeScale
   *  and n = p_degree (the degree of the equation)
   *
   * @param [in]  lookJ       Look vector in J2000 frame
   * @param [in]  partialVar  Variable derivative is to be with respect to
   * @param [in]  coeffIndex  Coefficient index in the polynomial fit to the variable (angle)
   * @return Vector rotated by derivative of J2000 to
   *                              reference rotation
   *
   */
  std::vector<double>
  SpiceRotation::ToReferencePartial(std::vector<double>& lookJ,
                                    SpiceRotation::PartialType partialVar, int coeffIndex) {
    NaifStatus::CheckErrors();
    //**TODO** To save time possibly save partial matrices

    // Get the rotation angles and form the derivative matrix for the partialVar
    std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);
    int angleIndex = partialVar;
    int axes[3] = {p_axis1, p_axis2, p_axis3};
    double angle = angles.at(angleIndex);

    double dmatrix[3][3];
    drotat_(&angle, (integer *) axes + angleIndex, (doublereal *) dmatrix);
    // Transpose to obtain row-major order
    xpose_c(dmatrix, dmatrix);

    // Get the derivative of the polynomial with respect to partialVar

    double dpoly = DPolynomial(coeffIndex);

    // Multiply dpoly to complete dmatrix
    for (int row = 0;  row < 3;  row++) {
      for (int col = 0;  col < 3;  col++) {
        dmatrix[row][col] *= dpoly;
      }
    }
    // Apply the other 2 angles and chain them all together
    double dCJ[3][3];
    switch(angleIndex) {
      case 0:
        rotmat_c(dmatrix, angles[1], axes[1], dCJ);
        rotmat_c(dCJ, angles[2], axes[2], dCJ);
        break;
      case 1:
        rotate_c(angles[0], axes[0], dCJ);
        mxm_c(dmatrix, dCJ, dCJ);
        rotmat_c(dCJ, angles[2], axes[2], dCJ);
        break;
      case 2:
        rotate_c(angles[0], axes[0], dCJ);
        rotmat_c(dCJ, angles[1], axes[1], dCJ);
        mxm_c(dmatrix, dCJ, dCJ);
        break;
    }

    // Multiply the constant matrix to rotate to target frame
    double dTJ[3][3];
    mxm_c((SpiceDouble *) &p_TC[0], dCJ[0], dTJ);

    // Finally rotate the J2000 vector with the derivative matrix, dTJ
    std::vector<double> lookdT(3);

    mxv_c(dTJ, (const SpiceDouble *) &lookJ[0], (SpiceDouble *) &lookdT[0]);

    NaifStatus::CheckErrors();
    return lookdT;
  }


  /** Wrap the input angle to keep it within 2pi radians of the
   *  angle to compare.
   *
   * @param [in]  compareAngle Look vector in J2000 frame
   * @param [in]  angle Angle to be wrapped if needed
   * @return double Wrapped angle
   *
   */
  double SpiceRotation::WrapAngle(double compareAngle, double angle) {
    NaifStatus::CheckErrors();
    double diff1 = compareAngle - angle;

    if (diff1 < -1 * pi_c()) {
      angle -= twopi_c();
    }
    else if (diff1 > pi_c()) {
      angle += twopi_c();
    }

    NaifStatus::CheckErrors();
    return angle;
  }

  /** Set the degree of the polynomials to be fit to the
   * three camera angles for the time period covered by the
   * cache, angle = c0 + c1*t + c2*t**2 + ... + cn*t**n,
   * where t = (time - p_baseTime) / p_timeScale, and n = p_degree.
   *
   * @param [in] degree Degree of the polynomial to be fit
   *
   * @internal
   *  @history 2011-03-22 Debbie A. Cook - Fixed bug in second branch where existing
   *                       degree is greater than new degree
   */
  void SpiceRotation::SetPolynomialDegree(int degree) {
    // Adjust the degree for the data
    if (p_fullCacheSize == 1) {
      degree = 0;
    }
    else if (p_fullCacheSize == 2) {
      degree = 1;
    }
    // If polynomials have not been applied yet then simply set the degree and return
    if (!p_degreeApplied) {
      p_degree = degree;
    }

    // Otherwise the existing polynomials need to be either expanded ...
    else if (p_degree < degree) {   // (increase the number of terms)
      std::vector<double> coefAngle1(p_coefficients[0]),
          coefAngle2(p_coefficients[1]),
          coefAngle3(p_coefficients[2]);

      for (int icoef = p_degree + 1;  icoef <= degree; icoef++) {
        coefAngle1.push_back(0.);
        coefAngle2.push_back(0.);
        coefAngle3.push_back(0.);
      }
      p_degree = degree;
      SetPolynomial(coefAngle1, coefAngle2, coefAngle3, p_source);
    }
    // ... or reduced (decrease the number of terms)
    else if (p_degree > degree) {
      std::vector<double> coefAngle1(degree + 1),
          coefAngle2(degree + 1),
          coefAngle3(degree + 1);

      for (int icoef = 0;  icoef <= degree;  icoef++) {
        coefAngle1[icoef] = p_coefficients[0][icoef];
        coefAngle2[icoef] = p_coefficients[1][icoef];
        coefAngle3[icoef] = p_coefficients[2][icoef];
      }
      p_degree = degree;
      SetPolynomial(coefAngle1, coefAngle2, coefAngle3, p_source);
    }
  }

  /** 
   * Accessor method to get the rotation source. 
   * @return The source of the rotation.
   */
  SpiceRotation::Source SpiceRotation::GetSource() {
    return p_source;
  }

  /**
   * Resets the source of the rotation to the given value. 
   * @param source The rotation source to be set.  
   */
  void SpiceRotation::SetSource(Source source) {
    p_source = source;
    return;
  }

  /**
   * Accessor method to get the rotation base time. 
   * @return The base time for the rotation
   */
  double SpiceRotation::GetBaseTime() {
    return p_baseTime;
  }

  /**
   * Accessor method to get the rotation time scale. 
   * @return The time scale for the rotation
   */
  double SpiceRotation::GetTimeScale() {
    return p_timeScale;
  }

  /** Set the axes of rotation for decomposition of a rotation
   *  matrix into 3 angles.
   *
   * @param [in]  axis1 Axes of rotation of first angle applied (right rotation)
   * @param [in]  axis2 Axes of rotation of second angle applied (center rotation)
   * @param [in]  axis3 Axes of rotation of third angle applied (left rotation)
   * @return double Wrapped angle
   *
   */
  void SpiceRotation::SetAxes(int axis1, int axis2, int axis3) {
    if (axis1 < 1  ||  axis2 < 1  || axis3 < 1  || axis1 > 3  || axis2 > 3  || axis3 > 3) {
      QString msg = "A rotation axis is outside the valid range of 1 to 3";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    p_axis1 = axis1;
    p_axis2 = axis2;
    p_axis3 = axis3;
  }

  /** Load the time cache.  This method should works with the LoadCache(startTime, endTime, size) method
   *  to load the time cache.
   *
   */
  void SpiceRotation::LoadTimeCache() {
    NaifStatus::CheckErrors();
    int count = 0;

    double observStart  =  p_fullCacheStartTime + p_timeBias;
    double observEnd  = p_fullCacheEndTime + p_timeBias;
    double currentTime = observStart;  // Added 12-03-2009 to allow observations to cross segment boundaries
    bool timeLoaded = false;

    // Get number of ck loaded for this rotation.  This method assumes only one SpiceRotation
    // object is loaded.
    NaifStatus::CheckErrors();
    ktotal_c("ck", (SpiceInt *) &count);

    // Downsize the loaded cache
    if ((p_source == Memcache) && p_minimizeCache == Yes) {
      // Multiple ck case, type 5 ck case, or PolyFunctionOverSpice
      //  final step -- downsize loaded cache and reload

      if (p_fullCacheSize != (int) p_cache.size()) {

        Isis::IString msg = "Full cache size does NOT match cache size in LoadTimeCache -- should never happen";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      SpiceDouble timeSclkdp[p_fullCacheSize];
      SpiceDouble quats[p_fullCacheSize][4];
      double avvs[p_fullCacheSize][3];// Angular velocity vector

      // We will treat et as the sclock time and avoid converting back and forth 
     for (int r = 0; r < p_fullCacheSize; r++) {
        timeSclkdp[r] = p_cacheTime[r];
        SpiceDouble CJ[9] = { p_cache[r][0], p_cache[r][1], p_cache[r][2],
                              p_cache[r][3], p_cache[r][4], p_cache[r][5],
                              p_cache[r][6], p_cache[r][7], p_cache[r][8]
                            };
        m2q_c(CJ, quats[r]);
        if (p_hasAngularVelocity) {
          vequ_c((SpiceDouble *) &p_cacheAv[r][0], avvs[r]);
        }
      }

      double cubeStarts = timeSclkdp[0]; //,timsSclkdp[ckBlob.Records()-1] };
      double radTol = 0.000000017453; //.000001 degrees  Make this instrument dependent TODO
      SpiceInt avflag = 1;            // Don't use angular velocity for now
      SpiceInt nints = 1;             // Always make an observation a single interpolation interval
      double dparr[p_fullCacheSize];    // Double precision work array
      SpiceInt intarr[p_fullCacheSize]; // Integer work array
      SpiceInt sizOut = p_fullCacheSize; // Size of downsized cache

      ck3sdn(radTol, avflag, (int *) &sizOut, timeSclkdp, (doublereal *) quats, 
             (SpiceDouble *) avvs, nints, &cubeStarts, dparr, (int *) intarr);

      // Clear full cache and load with downsized version
      p_cacheTime.clear();
      p_cache.clear();
      p_cacheAv.clear();
      std::vector<double> av;
      av.resize(3);

      for (int r = 0; r < sizOut; r++) {
        SpiceDouble et;
        // sct2e_c(spcode, timeSclkdp[r], &et);
        et = timeSclkdp[r];
        p_cacheTime.push_back(et);
        std::vector<double> CJ(9);
        q2m_c(quats[r], (SpiceDouble( *)[3]) &CJ[0]);
        p_cache.push_back(CJ);
        vequ_c(avvs[r], (SpiceDouble *) &av[0]);
        p_cacheAv.push_back(av);
      }

      timeLoaded = true;
      p_minimizeCache = Done;
    }
    else if (count == 1  && p_minimizeCache == Yes) {
      // case of a single ck -- read instances and data straight from kernel for given time range
      SpiceInt handle;

      // Define some Naif constants
      int FILESIZ = 128;
      int TYPESIZ = 32;
      int SOURCESIZ = 128;
//      double DIRSIZ = 100;

      SpiceChar file[FILESIZ];
      SpiceChar filtyp[TYPESIZ];
      SpiceChar source[SOURCESIZ];

      SpiceBoolean found;
      bool observationSpansToNextSegment = false;

      double segStartEt;
      double segStopEt;

      kdata_c(0, "ck", FILESIZ, TYPESIZ, SOURCESIZ, file, filtyp, source, &handle, &found);
      dafbfs_c(handle);
      daffna_c(&found);
      int spCode = ((int)(p_constantFrames[0] / 1000)) * 1000;

      while (found) {
        observationSpansToNextSegment = false;
        double sum[10];   // daf segment summary
        double dc[2];     // segment starting and ending times in tics
        SpiceInt ic[6];   // segment summary values:
        // instrument code for platform,
        // reference frame code,
        // data type,
        // velocity flag,
        // offset to quat 1,
        // offset to end.
        dafgs_c(sum);
        dafus_c(sum, (SpiceInt) 2, (SpiceInt) 6, dc, ic);

        // Don't read type 5 ck here
        if (ic[2] == 5) break;
//      
        // Check times for type 3 ck segment if spacecraft matches
        if (ic[0] == spCode && ic[2] == 3) {
          sct2e_c((int) spCode / 1000, dc[0], &segStartEt);
          sct2e_c((int) spCode / 1000, dc[1], &segStopEt);
          NaifStatus::CheckErrors();
          double et;

          // Get times for this segment
          if (currentTime >= segStartEt  &&  currentTime <= segStopEt) {

            // Check for a gap in the time coverage by making sure the time span of the observation does not
            // cross a segment unless the next segment starts where the current one ends
            if (observationSpansToNextSegment && currentTime > segStartEt) {
              QString msg = "Observation crosses segment boundary--unable to interpolate pointing";
              throw IException(IException::Programmer, msg, _FILEINFO_);
            }
            if (observEnd > segStopEt) {
              observationSpansToNextSegment = true;
            }

            // Extract necessary header parameters
            int dovelocity = ic[3];
            int end = ic[5];
            double val[2];
            dafgda_c(handle, end - 1, end, val);
//            int nints = (int) val[0];
            int ninstances = (int) val[1];
            int numvel  =  dovelocity * 3;
            int quatnoff  =  ic[4] + (4 + numvel) * ninstances - 1;
//            int nrdir = (int) (( ninstances - 1 ) / DIRSIZ); /* sclkdp directory records */
            int sclkdp1off  =  quatnoff + 1;
            int sclkdpnoff  =  sclkdp1off + ninstances - 1;
//            int start1off = sclkdpnoff + nrdir + 1;
//            int startnoff = start1off + nints - 1;
            int sclkSpCode = spCode / 1000;

            // Now get the times
            std::vector<double> sclkdp(ninstances);
            dafgda_c(handle, sclkdp1off, sclkdpnoff, (SpiceDouble *) &sclkdp[0]);

            int instance = 0;
            sct2e_c(sclkSpCode, sclkdp[0], &et);

            while (instance < (ninstances - 1)  &&  et < currentTime) {
              instance++;
              sct2e_c(sclkSpCode, sclkdp[instance], &et);
            }

            if (instance > 0) instance--;
            sct2e_c(sclkSpCode, sclkdp[instance], &et);

            while (instance < (ninstances - 1)  &&  et < observEnd) {
              p_cacheTime.push_back(et - p_timeBias);
              instance++;
              sct2e_c(sclkSpCode, sclkdp[instance], &et);
            }
            p_cacheTime.push_back(et - p_timeBias);

            if (!observationSpansToNextSegment) {
              timeLoaded = true;
              p_minimizeCache = Done;
              break;
            }
            else {
              currentTime = segStopEt;
            }
          }
        }
        dafcs_c(handle);     // Continue search in daf last searched
        daffna_c(&found);    // Find next forward array in current daf
      }
    }
    else if (count == 0  &&  p_source != Nadir  &&  p_minimizeCache == Yes) {
      QString msg = "No camera kernels loaded...Unable to determine time cache to downsize";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Load times according to cache size (body rotations) -- handle first round of type 5 ck case and multiple ck case --
    // Load a time for every line scan line and downsize later
    if (!timeLoaded) {
      double cacheSlope = 0.0;
      if (p_fullCacheSize > 1)
        cacheSlope = (p_fullCacheEndTime - p_fullCacheStartTime) / (double)(p_fullCacheSize - 1);
      for (int i = 0; i < p_fullCacheSize; i++)
        p_cacheTime.push_back(p_fullCacheStartTime + (double) i * cacheSlope);
      if (p_source == Nadir) p_minimizeCache = No;
    }
    NaifStatus::CheckErrors();
  }

  /** Return full listing (cache) of original time coverage
   * requested.
   */
  std::vector<double> SpiceRotation::GetFullCacheTime() {

    // No time cache was initialized -- throw an error
    if (p_fullCacheSize < 1) {
      QString msg = "Time cache not available -- rerun spiceinit";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    std::vector<double> fullCacheTime;
    double cacheSlope = 0.0;
    if (p_fullCacheSize > 1)  cacheSlope = (p_fullCacheEndTime - p_fullCacheStartTime) / (double)(p_fullCacheSize - 1);

    for (int i = 0; i < p_fullCacheSize; i++)
      fullCacheTime.push_back(p_fullCacheStartTime + (double) i * cacheSlope);

    return fullCacheTime;
  }

  /** Compute frame trace chain from target frame to J2000
   */
  void SpiceRotation::FrameTrace(double et) {
    NaifStatus::CheckErrors();
// The code for this method was extracted from the Naif routine rotget written by N.J. Bachman & W.L. Taber (JPL)
    int           center;
    NaifFrameType type;
    int           typid;
    SpiceBoolean  found;
    int           frmidx;  // Frame chain index for current frame
    SpiceInt      nextFrame;   // Naif frame code of next frame
    NaifStatus::CheckErrors();
    std::vector<int> frameCodes;
    std::vector<int> frameTypes;
    frameCodes.push_back(p_constantFrames[0]);

    while (frameCodes[frameCodes.size()-1] != J2000Code) {
      frmidx  =  frameCodes.size() - 1;
      // First get the frame type  (Note:: we may also need to save center if we use dynamic frames)
      frinfo_c((SpiceInt) frameCodes[frmidx], (SpiceInt *) &center, (SpiceInt *) &type, (SpiceInt *) &typid, &found);

      if (!found) {

        if (p_source == Nadir) {
          frameTypes.push_back(0);
          break;
        }

        QString msg = "The frame" + toString((int) frameCodes[frmidx]) + " is not supported by Naif";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      double matrix[3][3];

      // To get the next link in the frame chain, use the frame type
      if (type == INERTL ||  type == PCK) {
        nextFrame = J2000Code;
      }
      else if (type == CK) {
        ckfrot_((SpiceInt *) &typid, &et, (double *) matrix, &nextFrame, (logical *) &found);

        if (!found) {

          if (p_source == Nadir) {
            frameTypes.push_back(0);
            break;
          }

          QString msg = "The ck rotation from frame " + toString(frameCodes[frmidx]) + " can not be found"
                            + " due to no pointing available at requested time or a problem with the frame";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
      }
      else if (type == TK) {
        tkfram_((SpiceInt *) &typid, (double *) matrix, &nextFrame, (logical *) &found);
        if (!found) {
          QString msg = "The tk rotation from frame " + toString(frameCodes[frmidx]) + " can not be found";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
      }
      else if (type == DYN) {
        //
        //        Unlike the other frame classes, the dynamic frame evaluation
        //        routine ZZDYNROT requires the input frame ID rather than the
        //        dynamic frame class ID. ZZDYNROT also requires the center ID
        //        we found via the FRINFO call.

        zzdynrot_((SpiceInt *) &typid, (SpiceInt *) &center, &et, (double *) matrix, &nextFrame);
      }

      else {
        QString msg = "The frame " + toString(frameCodes[frmidx]) +
                          " has a type " + toString(type) + " not supported by your version of Naif Spicelib." +
                          "You need to update.";
        throw IException(IException::Programmer, msg, _FILEINFO_);

      }
      frameCodes.push_back(nextFrame);
      frameTypes.push_back(type);
    }

    if ((int) frameCodes.size() == 1  &&  p_source != Nadir) {  // Must be Sky
      p_constantFrames.push_back(frameCodes[0]);
      p_timeFrames.push_back(frameCodes[0]);
      return;
    }

    int nConstants = 0;
    p_constantFrames.clear();
    while (frameTypes[nConstants] == TK  &&  nConstants < (int) frameTypes.size()) nConstants++;


    for (int i = 0; i <= nConstants; i++) {
      p_constantFrames.push_back(frameCodes[i]);
    }

    if (p_source != Nadir)  {
      for (int i = nConstants;  i < (int) frameCodes.size(); i++) {
        p_timeFrames.push_back(frameCodes[i]);
      }
    }
    else {
      // Nadir rotation is from spacecraft to J2000
      p_timeFrames.push_back(frameCodes[nConstants]);
      p_timeFrames.push_back(J2000Code);
    }
    NaifStatus::CheckErrors();
  }


  /** 
   *  Return the full rotation TJ as a matrix
   */
  std::vector<double> SpiceRotation::Matrix() {
    NaifStatus::CheckErrors();
    std::vector<double> TJ;
    TJ.resize(9);
    mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], (SpiceDouble( *) [3]) &TJ[0]);
    NaifStatus::CheckErrors();
    return TJ;
  }

  /** 
   *  Return the constant 3x3 rotation TC matrix as a quaternion.
   *  
   *  @return Constant rotation quaternion, TC
   */
  std::vector<double> SpiceRotation::ConstantRotation() {
    NaifStatus::CheckErrors();
    std::vector<double> q;
    q.resize(4);
    m2q_c((SpiceDouble( *)[3]) &p_TC[0], &q[0]);
    NaifStatus::CheckErrors();
    return q;
  }

  /** 
   *  Return the constant 3x3 rotation TC matrix as a vector of length 9.
   *  
   *  @return Constant rotation matrix, TC
   */
  std::vector<double> &SpiceRotation::ConstantMatrix() {
    return p_TC;
  }

  /** 
   *  Set the constant 3x3 rotation TC matrix from a vector of length 9.
   *  
   *  @param constantMatrix Constant rotation matrix, TC.
   */
  void SpiceRotation::SetConstantMatrix(std::vector<double> constantMatrix) {
    p_TC = constantMatrix;
    return;
  }

  /** 
   *  Return time-based 3x3 rotation CJ matrix as a quaternion.
   *  
   *  @return Time-based rotation quaternion, CJ
   */
  std::vector<double> SpiceRotation::TimeBasedRotation() {
    NaifStatus::CheckErrors();
    std::vector<double> q;
    q.resize(4);
    m2q_c((SpiceDouble( *)[3]) &p_CJ[0], &q[0]);
    NaifStatus::CheckErrors();
    return q;
  }

  /** 
   *  Return time-based 3x3 rotation CJ matrix as a vector of length 9.
   *  
   *  @return Time-based rotation matrix, CJ
   */
  std::vector<double> &SpiceRotation::TimeBasedMatrix() {
    return p_CJ;
  }

  /** 
   *  Set the time-based 3x3 rotation CJ matrix from a vector of length 9.
   *  
   *  @param timeBasedMatrix Time-based rotation matrix, TC.
   */
  void SpiceRotation::SetTimeBasedMatrix(std::vector<double> timeBasedMatrix) {
    p_CJ = timeBasedMatrix;
    return;
  }

  /** 
   *  Initialize the constant rotation
   */
  void SpiceRotation::InitConstantRotation(double et) {
    FrameTrace(et);
    // Get constant rotation which applies in all cases
    int targetFrame = p_constantFrames[0];
    int fromFrame = p_timeFrames[0];
    p_TC.resize(9);
    refchg_((SpiceInt *) &fromFrame, (SpiceInt *) &targetFrame, &et, (doublereal *) &p_TC[0]);
    // Transpose to obtain row-major order
    xpose_c((SpiceDouble( *)[3]) &p_TC[0], (SpiceDouble( *)[3]) &p_TC[0]);
  }



  /** Compute the angular velocity from the time-based functions fit to the pointing angles
   *  This method computes omega = angular velocity matrix, and extracts the angular velocity.
   *  See comments in the Naif Spicelib routine xf2rav_c.c.
   *
   *            _                     _
   *           |                       |
   *           |   0    -av[2]  av[1]  |
   *           |                       |
   *   omega = |  av[2]    0   -av[0]  |
   *           |                       |
   *           | -av[1]   av[0]   0    |
   *           |_                     _|

   *
   *
   *
   */
  void SpiceRotation::ComputeAv() {
    NaifStatus::CheckErrors();

    // Make sure the angles have been fit to polynomials
    if (p_source < PolyFunction) {
      QString msg = "The SpiceRotation pointing angles must be fit to polynomials in order to compute angular velocity";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    std::vector<double> dCJdt;
    dCJdt.resize(9);
    DCJdt(dCJdt);
    double omega[3][3];
    mtxm_c((SpiceDouble( *)[3]) &dCJdt[0], (SpiceDouble( *)[3]) &p_CJ[0], omega);
    p_av[0] = omega[2][1];
    p_av[1] = omega[0][2];
    p_av[2] = omega[1][0];
  }


  /** Compute the derivative of the rotation p_CJ with respect to time.
   *  The derivative is computed based on p_CJ = [angle3]   [angle2]   [angle1]
   *    p_CJ = [angle3]    [angle2]    [angle1]
   *                  axis3       axis2       axis1
   *
   * @param [out]  dCJ       Derivative of p_CJ
   *
   */
  void SpiceRotation::DCJdt(std::vector<double> &dCJ) {
    NaifStatus::CheckErrors();

    // Get the rotation angles and axes
    std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);
    int axes[3] = {p_axis1, p_axis2, p_axis3};

    double dmatrix[3][3];
    double dangle;
    double wmatrix[3][3]; // work matrix
    dCJ.assign(9, 0.);

    for (int angleIndex = 0; angleIndex < 3; angleIndex++) {
      drotat_(&(angles[angleIndex]), (integer *) axes + angleIndex, (doublereal *) dmatrix);
      // Transpose to obtain row-major order
      xpose_c(dmatrix, dmatrix);

      // To get the derivative of the polynomial fit to the angle with respect to time
      // first create the function object for this angle and load its coefficients
      Isis::PolynomialUnivariate function(p_degree);
      function.SetCoefficients(p_coefficients[angleIndex]);

      // Evaluate the derivative of function at p_et
      //      dangle = function.DerivativeVar((p_et - p_baseTime) / p_timeScale);
      dangle = function.DerivativeVar((p_et - p_baseTime) / p_timeScale) / p_timeScale;

      // Multiply dangle to complete dmatrix
      for (int row = 0;  row < 3;  row++) {
        for (int col = 0;  col < 3;  col++) {
          dmatrix[row][col] *= dangle;
        }
      }
      // Apply the other 2 angles and chain them all together
      switch(angleIndex) {
        case 0:
          rotmat_c(dmatrix, angles[1], axes[1], dmatrix);
          rotmat_c(dmatrix, angles[2], axes[2], dmatrix);
          break;
        case 1:
          rotate_c(angles[0], axes[0], wmatrix);
          mxm_c(dmatrix, wmatrix, dmatrix);
          rotmat_c(dmatrix, angles[2], axes[2], dmatrix);
          break;
        case 2:
          rotate_c(angles[0], axes[0], wmatrix);
          rotmat_c(wmatrix, angles[1], axes[1], wmatrix);
          mxm_c(dmatrix, wmatrix, dmatrix);
          break;
      }
      int i, j;
      for (int index = 0; index < 9; index++) {
        i = index / 3;
        j = index % 3;
        dCJ[index] += dmatrix[i][j];
      }
    }

    NaifStatus::CheckErrors();
  }


  /** Compute and return the rotation matrix that rotates state vectors from J2000 to the target frame.
   */
  std::vector<double> SpiceRotation::StateTJ() {
    std::vector<double> stateTJ(36);

    // Build the state matrix for the time-based rotation from the matrix and angulary velocity
    double stateCJ[6][6];
    rav2xf_c(&p_CJ[0], &p_av[0], stateCJ);
// (SpiceDouble (*) [3]) &p_CJ[0]
    int irow = 0;
    int jcol = 0;
    int vpos = 0;

    for (int row = 3; row < 6; row++) {
      irow  =  row - 3;
      vpos  =  irow * 3;

      for (int col = 0; col < 3; col++) {
        jcol  =  col + 3;
        // Fill the upper left corner
        stateTJ[irow*6 + col] = p_TC[vpos] * stateCJ[0][col] + p_TC[vpos+1] * stateCJ[1][col] + p_TC[vpos+2] * stateCJ[2][col];
        // Fill the lower left corner
        stateTJ[row*6 + col]  =  p_TC[vpos] * stateCJ[3][col] + p_TC[vpos+1] * stateCJ[4][col] + p_TC[vpos+2] * stateCJ[5][col];
        // Fill the upper right corner
        stateTJ[irow*6 + jcol] = 0;
        // Fill the lower right corner
        stateTJ[row*6 +jcol] = stateTJ[irow*6 + col];
      }
    }
    return stateTJ;
  }


  /** Extrapolate pointing for a given time assuming a constant angular velocity.
   *  The pointing and angular velocity at the current time will be used to
   *  extrapolate pointing at the input time.  If angular velocity does not
   *  exist, the value at the current time will be output.
   *
   * @param [in]   timeEt    The time of the pointing to be extrapolated
   * @param [out]            A quaternion defining the rotation at the input time
   *
   */
   std::vector<double> SpiceRotation::Extrapolate(double timeEt) {
    NaifStatus::CheckErrors();

    if (!p_hasAngularVelocity) return p_CJ;

    double diffTime = timeEt - p_et;
    std::vector<double> CJ(9, 0.0);
    double dmat[3][3];

    // Create a rotation matrix for the axis and magnitude of the angular velocity * the time difference
    axisar_c((SpiceDouble *) &p_av[0], diffTime*vnorm_c((SpiceDouble *) &p_av[0]), dmat);

    // Rotate from the current time to the desired time assuming constant angular velocity
    mxm_c(dmat, (SpiceDouble *) &p_CJ[0], (SpiceDouble( *)[3]) &CJ[0]);
    return CJ;
   }


   /** Set the full cache time parameters.
    *
    * @param [in]   startTime The earliest time of the full cache coverage
    * @param [in]   endTime   The latest time of the full cache coverage
    * @param [in]   cacheSize The number of epochs in the full (line) cache
    *
    */
   void SpiceRotation::SetFullCacheParameters(double startTime, double endTime, int cacheSize) {
    // Save full cache parameters
    p_fullCacheStartTime = startTime;
    p_fullCacheEndTime = endTime;
    p_fullCacheSize = cacheSize;
   }

   void SpiceRotation::SetEphemerisTimeMemcache() {
    // If the cache has only one rotation, set it
    NaifStatus::CheckErrors();

    if (p_cache.size() == 1) {
      /*        p_quaternion = p_cache[0];*/
      p_CJ = p_cache[0];
//         p_CJ = p_quaternion.ToMatrix();

      if (p_hasAngularVelocity) {
        p_av = p_cacheAv[0];
      }

    }

    // Otherwise determine the interval to interpolate
    else {
      std::vector<double>::iterator pos;
      pos = upper_bound(p_cacheTime.begin(), p_cacheTime.end(), p_et);

      int cacheIndex;

      if (pos != p_cacheTime.end()) {
        cacheIndex = distance(p_cacheTime.begin(), pos);
        cacheIndex--;
      }
      else {
        cacheIndex = p_cacheTime.size() - 2;
      }

      if (cacheIndex < 0) cacheIndex = 0;

      // Interpolate the rotation
      double mult = (p_et - p_cacheTime[cacheIndex]) /
                    (p_cacheTime[cacheIndex+1] - p_cacheTime[cacheIndex]);
      /*        Quaternion Q2 (p_cache[cacheIndex+1]);
               Quaternion Q1 (p_cache[cacheIndex]);*/
      std::vector<double> CJ2(p_cache[cacheIndex+1]);
      std::vector<double> CJ1(p_cache[cacheIndex]);
      SpiceDouble J2J1[3][3];
      mtxm_c((SpiceDouble( *)[3]) &CJ2[0], (SpiceDouble( *)[3]) &CJ1[0], J2J1);
      SpiceDouble axis[3];
      SpiceDouble angle;
      raxisa_c(J2J1, axis, &angle);
      SpiceDouble delta[3][3];
      axisar_c(axis, angle * (SpiceDouble)mult, delta);
      mxmt_c((SpiceDouble *) &CJ1[0], delta, (SpiceDouble( *) [3]) &p_CJ[0]);
 
      if (p_hasAngularVelocity) {
        double v1[3], v2[3]; // Vectors surrounding desired time
        vequ_c((SpiceDouble *) &p_cacheAv[cacheIndex][0], v1);
        vequ_c((SpiceDouble *) &p_cacheAv[cacheIndex+1][0], v2);
        vscl_c((1. - mult), v1, v1);
        vscl_c(mult, v2, v2);
        vadd_c(v1, v2, (SpiceDouble *) &p_av[0]);
      }
    }
    NaifStatus::CheckErrors();
   }


  void SpiceRotation::SetEphemerisTimeNadir() {
   // TODO what about spk time bias and mission setting of light time corrections
   //      That information has only been passed to the SpicePosition class and
   //      is not available to this class, but probably should be applied to the
   //      spkez call.

   // Make sure the constant frame is loaded.  This method also does the frame trace.
   NaifStatus::CheckErrors();
   if (p_timeFrames.size() == 0) InitConstantRotation(p_et);

   SpiceDouble stateJ[6];  // Position and velocity vector in J2000
   SpiceDouble lt;
   SpiceInt spkCode = p_constantFrames[0] / 1000;
   spkez_c((SpiceInt) spkCode, p_et, "J2000", "LT+S",
           (SpiceInt) p_targetCode, stateJ, &lt);
   // reverse the position to be relative to the spacecraft.  This may be
   // a mission dependent value and possibly the sense of the velocity as well.
   SpiceDouble sJ[3], svJ[3];
   vpack_c(-1 * stateJ[0], -1 * stateJ[1], -1 * stateJ[2], sJ);
   vpack_c(stateJ[3], stateJ[4], stateJ[5], svJ);
   twovec_c(sJ,
            p_axisP,
            svJ,
            p_axisV,
            (SpiceDouble( *)[3]) &p_CJ[0]);
   NaifStatus::CheckErrors();
  }


  void SpiceRotation::SetEphemerisTimeSpice() {
   NaifStatus::CheckErrors();
   SpiceInt j2000 = J2000Code;

   SpiceDouble time = p_et + p_timeBias;

   // Make sure the constant frame is loaded.  This method also does the frame trace.
   if (p_timeFrames.size() == 0) InitConstantRotation(p_et);
   int toFrame = p_timeFrames[0];

   // First try getting the entire state matrix (6x6), which includes CJ and the angular velocity
   double stateCJ[6][6];
   frmchg_((integer *) &j2000, (integer *) &toFrame, &time, (doublereal *) stateCJ);

   // If Naif fails attempting to get the state matrix, assume the angular velocity vector is
   // not available and just get the rotation matrix.  First turn off Naif error reporting and
   // return any error without printing them.
   SpiceBoolean ckfailure = failed_c();
   reset_c();                   // Reset Naif error system to allow caller to recover

   if (!ckfailure) {
     xpose6_c(stateCJ, stateCJ);
     xf2rav_c(stateCJ, (SpiceDouble( *)[3]) &p_CJ[0], (SpiceDouble *) &p_av[0]);
     p_hasAngularVelocity = true;
   }
   else {
     refchg_((integer *) &j2000, (integer *) &toFrame, &time, (SpiceDouble *) &p_CJ[0]);

     if (failed_c()) {
       char naifstr[64];
       getmsg_c("SHORT", sizeof(naifstr), naifstr);
       reset_c();  // Reset Naif error system to allow caller to recover

       if (eqstr_c(naifstr, "SPICE(UNKNOWNFRAME)")) {
         Isis::IString msg = Isis::IString((int) p_constantFrames[0]) + " is an unrecognized " +
                             "reference frame code.  Has the mission frames kernel been loaded?";
         throw IException(IException::Io, msg, _FILEINFO_);
       }
       else {
         Isis::IString msg = "No pointing available at requested time [" +
                             Isis::IString(p_et + p_timeBias) + "] for frame code [" +
                             Isis::IString((int) p_constantFrames[0]) + "]";
         throw IException(IException::Io, msg, _FILEINFO_);
       }
     }

     // Transpose to obtain row-major order
     xpose_c((SpiceDouble( *)[3]) &p_CJ[0], (SpiceDouble( *)[3]) &p_CJ[0]);
   }
  }


  std::vector<double> SpiceRotation::EvaluatePolyFunction() {
   Isis::PolynomialUnivariate function1(p_degree);
   Isis::PolynomialUnivariate function2(p_degree);
   Isis::PolynomialUnivariate function3(p_degree);

   // Load the functions with the coefficients
   function1.SetCoefficients(p_coefficients[0]);
   function2.SetCoefficients(p_coefficients[1]);
   function3.SetCoefficients(p_coefficients[2]);

   double rtime;
   rtime = (p_et - p_baseTime) / p_timeScale;
   std::vector<double> angles;
   angles.push_back(function1.Evaluate(rtime));
   angles.push_back(function2.Evaluate(rtime));
   angles.push_back(function3.Evaluate(rtime));

   // Get the first angle back into the range Naif expects [-180.,180.]
   if (angles[0] < -1 * pi_c()) {
     angles[0] += twopi_c();
   }
   else if (angles[0] > pi_c()) {
     angles[0] -= twopi_c();
   }
   return angles;
  }


  void SpiceRotation::SetEphemerisTimePolyFunction() {
   NaifStatus::CheckErrors();

   Isis::PolynomialUnivariate function1(p_degree);
   Isis::PolynomialUnivariate function2(p_degree);
   Isis::PolynomialUnivariate function3(p_degree);

   // Load the functions with the coefficients
   function1.SetCoefficients(p_coefficients[0]);
   function2.SetCoefficients(p_coefficients[1]);
   function3.SetCoefficients(p_coefficients[2]);

   double rtime;
   rtime = (p_et - p_baseTime) / p_timeScale;
   double angle1 = function1.Evaluate(rtime);
   double angle2 = function2.Evaluate(rtime);
   double angle3 = function3.Evaluate(rtime);

   // Get the first angle back into the range Naif expects [-180.,180.]
   if (angle1 < -1 * pi_c()) {
     angle1 += twopi_c();
   }
   else if (angle1 > pi_c()) {
     angle1 -= twopi_c();
   }

   eul2m_c((SpiceDouble) angle3, (SpiceDouble) angle2, (SpiceDouble) angle1,
           p_axis3,             p_axis2,              p_axis1,
           (SpiceDouble( *)[3]) &p_CJ[0]);

   if (p_hasAngularVelocity) {
     if ( p_degree == 0)
       p_av = p_cacheAv[0];
     else
       ComputeAv();
   }
   NaifStatus::CheckErrors();
  }


  void SpiceRotation::SetEphemerisTimePolyFunctionOverSpice() {
    NaifStatus::CheckErrors();
    SetEphemerisTimeMemcache();
    std::vector<double> cacheAngles(3);
    std::vector<double> cacheVelocity(3);
    cacheAngles = Angles(p_axis3, p_axis2, p_axis1);
    cacheVelocity = p_av;
    SetEphemerisTimePolyFunction();
    std::vector<double> polyAngles(3);
    // The decomposition fails because the angles are outside the valid range for Naif
    // polyAngles = Angles(p_axis3, p_axis2, p_axis1); 
    polyAngles = EvaluatePolyFunction();
    std::vector<double> polyVelocity(3);
    polyVelocity = p_av;
    std::vector<double> angles(3);

    for (int index = 0; index < 3; index++) {
      angles[index] = cacheAngles[index] + polyAngles[index];
      p_av[index] += cacheVelocity[index];
    }

   if (angles[0] < -1 * pi_c()) {
     angles[0] += twopi_c();
   }
   else if (angles[0] > pi_c()) {
     angles[0] -= twopi_c();
   }

   if (angles[2] < -1 * pi_c()) {
     angles[2] += twopi_c();
   }
   else if (angles[2] > pi_c()) {
     angles[2] -= twopi_c();
   }

   eul2m_c((SpiceDouble) angles[2], (SpiceDouble) angles[1], (SpiceDouble) angles[0],
           p_axis3,             p_axis2,              p_axis1,
           (SpiceDouble( *)[3]) &p_CJ[0]);
   NaifStatus::CheckErrors();
  }


}
