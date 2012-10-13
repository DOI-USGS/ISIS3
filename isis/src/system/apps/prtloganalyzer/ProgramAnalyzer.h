#if !defined(ProgramAnalyzer_h)
#define ProgramAnalyzer_h
/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2008/09/06 06:47:48 $
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
#include <iostream>

#include "PvlKeyword.h"
#include "PvlContainer.h"
#include "PvlObject.h"
#include "Statistics.h"
#include "CollectorMap.h"
#include "IException.h"

namespace Isis {

  /**
   * @brief A ProgramAnalyzer accumulates print log runtime statistics 
   *  
   * This class reads an ISIS3 print log and accumulates runtime statistics for 
   * all programs found in the log file.  It will compute overall CPU and 
   * connect time to use in analysis of (typically I/O) performance.
   *  
   * Various format and reporting utilities are provided to externalize the 
   * results. 
   *
   * @ingroup Utility
   * @author 2006-11-09 Kris Becker
   *
   * @internal
   */
  class ProgramAnalyzer {

    public:
      //  Constructors and Destructor
      ProgramAnalyzer();
      ProgramAnalyzer(const std::string &logfile);

      /** Destructor ensures everything is cleaned up properly */
      virtual ~ProgramAnalyzer() { }

      /** Returns total number of programs analyzed */
      inline int count() const { return (_count); }
      /** Returns the total number of analyzed programs */
      inline int size() const { return (_pdata.size()); }
      /** Returns the number of valid programs */
      int valid() const { return (getCount(VALID)); }
      /** Returns the number of programs with errors  */
      int errors() const { return (getCount(ERRORS)); }
      /** Returns the number of programs with incomplete data */
      int nodata() const { return (getCount(NODATA)); }
      /** Returns the number of programs that had bad/invalid data */
      int baddata() const { return (getCount(BADDATA)); }
      /** Returns the number of programs that had zero CPU or run times */
      int zerotime() const { return (getCount(ZEROTIME)); }

      /** Returns total number of unique programs */
      int Programs() const { return (_programs.size()); }

      void setExclude(const std::string &name);
      void exclude(const std::string &exclude);
      void setInclude(const std::string &name);
      void include(const std::string &include);

      void add(const std::string &logfile);
      void add(PvlObject &program);

      PvlGroup review(const std::string &name = "Results") const;
      PvlGroup cumulative(const std::string &name = "ProgramTotals") const;
      PvlGroup summarize(const std::string &name) const;
      PvlGroup summarize(const int index) const;

      std::ostream &header(std::ostream &out) const;
      std::ostream &listify(std::ostream &out) const;


    private:

      /** Container for runtime status of a programs */
      struct RunTimeStats {
        std::string pname;
        Statistics  contime;
        Statistics  cputime;
        Statistics  iotime;
        RunTimeStats() : pname(""), contime(), cputime(), iotime() { }
        RunTimeStats(const std::string &name) : pname(name), contime(), 
                                                cputime(), iotime() { }
      };

      enum Status { VALID, ERRORS, NODATA, BADDATA, ZEROTIME };  //!< Program status
      /** Container for program data */
      struct ProgramData {
        Status      status;
        std::string name;
        std::string runtime;
        std::string from;
        std::string to;
        double      cpuTime;
        double      connectTime;
        ProgramData() : status(BADDATA), name(""), runtime(""), 
                        from(""), to(""), cpuTime(0.0), connectTime(0.0) { }
      };

      /* Containers for program lists */
      typedef CollectorMap<std::string, int> LogList;
      typedef CollectorMap<std::string, RunTimeStats> RunList;

      int     _count;                    //!< Count of all programs
      LogList _excludes;                 //!< Program exclusion list
      LogList _includes;                 //!< Program inclusion list
      RunList _programs;                 //!< List of unique programs
      RunTimeStats _totals;              //!< Runtime statistics for programs
      std::vector<ProgramData>  _pdata;  //!< Individual program data


      void init();
      int getCount (Status status) const;
      std::string getKey(PvlObject &obj, const std::string &key, 
                         const std::string &grp = "") const;

      /** Find a string in a PvlContainer */
      std::string findKey(PvlContainer &kset, const std::string &key) const {
        std::string value("");
        if ( kset.HasKeyword(key) ) {
          value = kset[key][0];
        }
        return (value);
      }

      Status convertTime(const std::string &time, double &ptime) const;
      bool accounting(PvlObject &obj, ProgramData &data) const;
      bool analyze(const ProgramData &data);

      IString format(const std::string &s) const;
      IString DblToStr(const double &value, const int precision = 2) const;
      PvlGroup toPvl(const RunTimeStats &stats, const std::string &name = "") const;
      int LimitTotals(const LogList &limit) const;
  };
}


#endif


