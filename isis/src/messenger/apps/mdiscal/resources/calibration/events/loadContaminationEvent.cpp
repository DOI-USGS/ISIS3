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
 double loadContaminationEvent(const std::string &scStartTime, const int filter, 
                               std::string &ename, std::string &eDate) {

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
    if ( ename.empty() ) {
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
      std::string mess = "Number values (" + IString(csv.columns()) +
                         ") in file " + ename + " less than number requested (" +
                         IString(nvalues) + ")!";
      throw IException(IException::User, mess.c_str(), _FILEINFO_);
    }

    // Ensure NAIF kernels are loaded for NAIF time computations
    (void) loadNaifTiming();

    //  Convert s/c clock start time to et
    double obsStartTime;
    (void) scs2e_c(-236, scStartTime.c_str(), &obsStartTime);

    // Set initial conditions and loop through all rows in the event table
    double evalue(1.0);
    eDate = "N/A";  // Will attain a valid time on guaranteed first pass
    double preEventTime(0.0);
    for (int i = 0 ; i < csv.rows() ; i++) {
      CSVReader::CSVAxis eRow = csv.getRow(i);
      std::string utcTime = eRow[0];
      double eTime;
      (void) utc2et_c(utcTime.c_str(), &eTime);

      // If current time is greater than start time this is the post event case
      if (eTime > obsStartTime) {
#define CONSIDER_POST_EVENT_TIME 1
#if defined(CONSIDER_POST_EVENT_TIME)
        //  Get closest pre or post event correction factor
        if ( fabs(obsStartTime-preEventTime) > fabs(eTime-obsStartTime) ) {
          //  Post-event time closer to SCLK than Pre-event time
          eDate = utcTime;
          evalue = eRow[column].ToDouble();
        }
#endif
        break;  //  Terminate loop and return
      }

      // Record pre-event time slot - Sets return variables as well
      eDate = utcTime;
      preEventTime = eTime;
      evalue = eRow[column].ToDouble();
    }
 
    // Return the factor
    return (evalue);
  }

