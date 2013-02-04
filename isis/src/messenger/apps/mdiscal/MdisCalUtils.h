#ifndef MdisCalUtils_h
#define MdisCalUtils_h
/**
 * @file
 * $Revision$
 * $Date$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <string>
#include <vector>
#include <cmath>

#include "CSVReader.h"
#include "FileName.h"
#include "IString.h"
#include "Spice.h"

namespace Isis {
  /**
   * @brief Helper function to convert values to doubles
   *
   * @param T Type of value to convert
   * @param value Value to convert
   *
   * @return double Converted value
   */
  template <typename T> double ToDouble(const T &value) {
    return toDouble(QString(value).trimmed());
  }

  template <typename T> int ToInteger(const T &value) {
    return toInt(QString(value).trimmed());
  }

  template <typename T> inline T MIN(const T &A, const T &B) {
    return (((A) < (B)) ? (A) : (B));
  }

  template <typename T> inline T MAX(const T &A, const T &B) {
    return (((A) > (B)) ? (A) : (B));
  }

  inline QString Quote(const QString &value) {
    if(value.isEmpty()) return (value);
    if(value[0] == '"') return (value);
    return (QString('"' + value + '"'));
  }

  /**
   * @brief Load required NAIF kernels required for timing needs
   *
   * This method maintains the loading of kernels for MESSENGER timing and
   * planetary body ephemerides to support time and relative positions of planet
   * bodies.
   */
  static void loadNaifTiming() {
    static bool _naifLoaded(false);
    if(!_naifLoaded) {
//  Load the NAIF kernels to determine timing data
      Isis::FileName leapseconds("$base/kernels/lsk/naif????.tls");
      leapseconds = leapseconds.highestVersion();

      Isis::FileName sclk("$messenger/kernels/sclk/messenger_????.tsc");
      sclk = sclk.highestVersion();

      Isis::FileName pck("$base/kernels/spk/de???.bsp");
      pck = pck.highestVersion();

//  Load the kernels
      QString leapsecondsName(leapseconds.expanded());
      QString sclkName(sclk.expanded());
      QString pckName(pck.expanded());
      furnsh_c(leapsecondsName.toAscii().data());
      furnsh_c(sclkName.toAscii().data());
      furnsh_c(pckName.toAscii().data());

//  Ensure it is loaded only once
      _naifLoaded = true;
    }
    return;
  }

  /**
   * @brief Computes the distance from the Sun to the observed body
   *
   * This method requires the appropriate NAIK kernels to be loaded that
   * provides instrument time support, leap seconds and planet body ephemeris.
   *
   * @return double Distance in AU between Sun and observed body
   */
  static bool sunDistanceAU(const QString &scStartTime,
                            const QString &target,
                            double &sunDist) {

    //  Ensure NAIF kernels are loaded
    loadNaifTiming();
    sunDist = 1.0;

    //  Determine if the target is a valid NAIF target
    SpiceInt tcode;
    SpiceBoolean found;
    (void) bodn2c_c(target.toAscii().data(), &tcode, &found);
    if(!found) return (false);

    //  Convert starttime to et
    double obsStartTime;
    scs2e_c(-236, scStartTime.toAscii().data(), &obsStartTime);

    //  Get the vector from target to sun and determine its length
    double sunv[3];
    double lt;
    (void) spkpos_c(target.toAscii().data(), obsStartTime, "J2000", "LT+S", "sun",
                    sunv, &lt);
    double sunkm = vnorm_c(sunv);

    //  Return in AU units
    sunDist = sunkm / 1.49597870691E8;
    return (true);
  }

  std::vector<double> loadWACCSV(const QString &fname, int filter,
                                 int nvalues, bool header = true, int skip = 0) {
    //  Open the CSV file
    FileName csvfile(fname);
    CSVReader csv(csvfile.expanded(), header, skip);
    for(int i = 0 ; i < csv.rows() ; i++) {
      CSVReader::CSVAxis row = csv.getRow(i);
      if(ToInteger(row[0]) == filter) {
        if((row.dim1() - 1) < nvalues) {
          QString mess = "Number values (" + QString(row.dim1() - 1) +
                             ") in file " + fname +
                             " less than number requested (" +
                             QString(nvalues) + ")!";
          throw IException(IException::User, mess, _FILEINFO_);
        }

        std::vector<double> rsp;
        for(int c = 0 ; c < nvalues ; c++) {
          rsp.push_back(ToDouble(row[1+c]));
        }
        return (rsp);
      }
    }

    // If it reaches here, the filter was not found
    std::ostringstream mess;
    mess << "CSV Vector MDIS filter " << filter <<  ", not found in file "
         << fname << "!";
    throw IException(IException::User, mess.str(), _FILEINFO_);
  }


  std::vector<double> loadNACCSV(const QString &fname, int nvalues,
                                 bool header = true, int skip = 0) {
    //  Open the CSV file
    FileName csvfile(fname);
    CSVReader csv(csvfile.expanded(), header, skip);
    CSVReader::CSVAxis row = csv.getRow(0);
    if(row.dim1() < nvalues) {
      QString mess = "Number values (" + QString(row.dim1()) +
                         ") in file " + fname + " less than number requested (" +
                         QString(nvalues) + ")!";
      throw IException(IException::User, mess, _FILEINFO_);

    }
    std::vector<double> rsp;
    for(int i = 0 ; i < nvalues ; i++) {
      rsp.push_back(ToDouble(row[i]));
    }
    return (rsp);
  }


  std::vector<double> loadResponsivity(bool isNAC, bool binned, int filter,
                                       QString &fname) {

    FileName resfile(fname);
    if(fname.isEmpty()) {
      QString camstr = (isNAC) ? "NAC" : "WAC";
      QString binstr = (binned)       ? "_BINNED" : "_NOTBIN";
      QString base   = "$messenger/calibration/RESPONSIVITY/";
      resfile = base + "MDIS" + camstr + binstr + "_RESP_?.TAB";
      resfile = resfile.highestVersion();
      fname = resfile.originalPath() + "/" + resfile.name();
    }

    // Unfortunately NAC has a slightly different format so must do it
    //  explicitly
    if(isNAC) {
      return (loadNACCSV(fname, 4, false, 0));
    }
    else {
      // Load the WAC parameters
      return (loadWACCSV(fname, filter, 4, false, 0));
    }
  }


  std::vector<double> loadSolarIrr(bool isNAC, bool binned, int filter,
                                   QString &fname)  {

    FileName solfile(fname);
    if(fname.isEmpty()) {
      QString camstr = (isNAC) ? "NAC" : "WAC";
      QString base   = "$messenger/calibration/SOLAR/";
      solfile = base + "MDIS" + camstr + "_SOLAR_?.TAB";
      solfile = solfile.highestVersion();
      fname = solfile.originalPath() + "/" + solfile.name();
    }

    if(isNAC) {
      return (loadNACCSV(fname, 3, false, 0));
    }
    else {
      return (loadWACCSV(fname, filter, 3, false, 0));
    }
  }

  double loadSmearComponent(bool isNAC, int filter, QString &fname) {

    FileName smearfile(fname);
    if(fname.isEmpty()) {
      QString camstr = (isNAC) ? "NAC" : "WAC";
      QString base   = "$messenger/calibration/smear/";
      smearfile = base + "MDIS" + camstr + "_FRAME_TRANSFER_??.TAB";
      smearfile = smearfile.highestVersion();
      fname = smearfile.originalPath() + "/" + smearfile.name();
    }

    std::vector<double> smear;
    if(isNAC) {
      smear = loadNACCSV(fname, 1, false, 0);
    }
    else {
      smear = loadWACCSV(fname, filter, 1, false, 0);
    }
    return (smear[0]);
  }

/**
 * @brief Load and retrieve contamination correction factor 
 *  
 * This function determines the correction factor for a contamination event 
 * that occured on the spacecraft after Mercury orbit insertion.  The affected 
 * dates are May 24, 2011 to January 3, 2012. 
 *  
 * The table of correction factors is expected to be stored in 
 * $messenger/calibration/events/event_table_ratioed_v?.txt.  However, the 
 * caller may provide a table that conforms to the expected format.  The 
 * expected format for the contamination event file is a comma separated value
 * (CSV) table that contains 13 columns of data per row.  The first column is
 * the UTC time during the event. The next 12 columns contain multiplicative 
 * correction factors for each WAC filter (NAC correction factors are not 
 * provided). These factors are expected to be around 1.0 (the default) as it 
 * is expected to directly scale DN values. 
 *  
 * Below is the expected mapping of column indexes to filter numbers as 
 * specfied in the BandBin/Number keyword from MDIS ISIS cube labels. Index is 
 * the column index from each row for a given filter, Number is the value of 
 * the BandBin/Number keyword from the label designating the filter number 
 * (corresponding to the filter parameter passed to this routine) and Letter is
 * the filter letter designation used in the last alpha numeric character in 
 * MDIS filenames: 
 *  
 *  Index   Number   Letter   Wavelength
 *    1       6        F         430 nm
 *    2       3        C         480 nm
 *    3       4        D         560 nm
 *    4       5        E         630 nm
 *    5       7        G         750 nm
 *    6      12        L         830 nm
 *    7      10        J         900 nm
 *    8       9        I        1000 nm
 *    9       1        A        Filter 1 (700 nm)
 *   10       2        B        Filter 2 (clear)
 *   11       8        H        Filter 8 (950 nm)
 *   12      11        K        Filter 11 (1010 nm)
 *  
 * The UTC dates in the first column are assumed to be strictly increasing in 
 * time.  The initial table (*_v2) contains dates that span the complete 
 * expected timeframe of the mission (launch at 2004-08-04T10:00:00.000000, 
 * termination at 2015-01-03T09:00:00.000000). 
 *  
 * The spacecraft clock time is provided as input (scStartTime) to this 
 * function.  This value is converted to ET (SCET) and used to determine the 
 * corresponding event time in the first column of the table.  The first table 
 * column time is represented in UTC time.  This time is converted to ET and 
 * then compared with the start time in ET. 
 *  
 * The algorithm searches linearly through the table essentially storing the 
 * time slot prior to the SCET and the next occuring one.  Ultimately, the 
 * factor returned by the algorithm is the one whose event time is closest to 
 * the SCET. 
 *  
 * The contamination correction model and algorithm was developed by
 * Mary Ruth Keller of JHA/APL.
 *
 * @author Kris Becker - 10/23/2012
 * 
 * @param scStartTime - Start time of the image in SCLK format
 * @param filter      - WAC filter number to return event correction factor for
 * @param ename       - Returns the name of the event table file if not 
 *                      provided by caller.  If non-empty string is passed by
 *                      caller, it is assumed to be a fully qualified filename
 *                      of the event table.
 * @param eDate       - Returns the UTC date entry of the selected correction 
 *                      event factor
 * 
 * @return double     - Event correction factor at the selected time to apply 
 *                      to WAC filter data.
 */
 double loadContaminationEvent(const QString &scStartTime, const int filter, 
                               QString &ename, QString &eDate) {

   //  This table maps the filter number extracted from BandBin/Number keyword
   //  to the columns (index) in the contamination table
   const int filterMap[12] = { 6, 3, 4, 5, 7, 12, 10, 9, 1, 2, 8, 11 };

   //  Find the WAC filter column index
   int ncols = sizeof(filterMap)/sizeof(filterMap[0]);
   int column(-1);
   for (int c = 0 ; c < ncols ; c++) {
     if (filterMap[c] == filter) {
       column = c + 1;  // indexes start after 1st (time) column
       break;
     }
   }

   // Ensure we have a valid filter number
   if (column <= 0) {
     std::ostringstream mess;
      mess << "Invalid MDIS WAC filter number (" << filter 
           <<  " - range:1-12) for determining index into contamination event table.";
      throw IException(IException::User, mess.str(), _FILEINFO_);     
   }

   //  File name not provided by caller.  Determine the event table name
    if ( ename.isEmpty() ) {
      FileName eventfile("$messenger/calibration/events/event_table_ratioed_v?.txt");
      eventfile = eventfile.highestVersion();
      ename = eventfile.originalPath() + "/" + eventfile.name();
    }

    //  Open/read the CSV contamination event file
    FileName csvfile(ename);
    const bool header(false);  // No header in file
    const int skip(0);         // No lines to skip to data
    const int nvalues(13);     // Expected columns in table
    CSVReader csv(csvfile.expanded(), header, skip);
    if (csv.columns() < nvalues) {  // All rows should have same # columns 
      QString mess = "Number values (" + QString(csv.columns()) +
                         ") in file " + ename + " less than number requested (" +
                         QString(nvalues) + ")!";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Ensure NAIF kernels are loaded for NAIF time computations
    (void) loadNaifTiming();

    //  Convert s/c clock start time to et
    double obsStartTime;
    (void) scs2e_c(-236, scStartTime.toAscii().data(), &obsStartTime);

    // Set initial conditions and loop through all rows in the event table
    double evalue(1.0);
    eDate = "N/A";  // Will attain a valid time on guaranteed first pass
    double preEventTime(0.0);
    for (int i = 0 ; i < csv.rows() ; i++) {
      CSVReader::CSVAxis eRow = csv.getRow(i);
      QString utcTime = eRow[0];
      double eTime;
      (void) utc2et_c(utcTime.toAscii().data(), &eTime);

      // If current time is greater than start time this is the post event case
      if (eTime > obsStartTime) {
        //  Get closest pre or post event correction factor
        if ( fabs(obsStartTime-preEventTime) > fabs(eTime-obsStartTime) ) {
          //  Post-event time closer to SCLK than Pre-event time
          eDate = utcTime;
          evalue = toDouble(eRow[column]);
        }

        break;  //  Terminate loop and return
      }

      // Record pre-event time slot - Sets return variables as well
      eDate = utcTime;
      preEventTime = eTime;
      evalue = toDouble(eRow[column]);
    }
 
    // Return the factor
    return (evalue);
  }


};
#endif
