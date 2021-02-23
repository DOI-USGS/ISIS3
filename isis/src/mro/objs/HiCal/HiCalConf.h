#ifndef HiCalConf_h
#define HiCalConf_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <iostream>
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "Pvl.h"
#include "DbAccess.h"
#include "IException.h"

namespace Isis {

  /**
   *  @brief HiCalConf manages HiRISE calibration matrices
   * that alter some or all of the
   * parameters contained in the Object section of the file.
   *  This object accepts a configuration file that contains a generic format of
   *  HiRISE Calibration matrices and loads the appropriate set based upon TDI,
   *  BIN, and channel number minimally.
   *
   *  Below is the basic unprofiled version of the configuration (database)
   *  file:
   *
   *  @code
   *  Object = Database
   *    A = "$mro/calibration/matrices/A_TDI{TDI}_BIN{BIN}_????.cub"
   *    B = "$mro/calibration/matrices/B_TDI{TDI}_BIN{BIN}_????.cub"
   *    G = "$mro/calibration/matrices/G_TDI{TDI}_BIN{BIN}_????.cub"
   *  EndObject
   *
   * @endcode
   *
   * The \b {TDI} and \b {BIN} will be replaced with the textual translation of
   * the TDI (128, 64, 32, 16, 8) and BIN (1, 2, 3, 4, 8, 16) numbers.  This
   * makes for easy defaulting to set up specific profiling of combinations.
   *
   * Additionally, you can add profiles for a TDI/BIN mode combination should
   * the need arise. Simply add one or more \a Profile groups that specify
   * different files for the observation conditions.
   * @endcode
   *
   * This object will retain B, G and IF references from the main Database
   * object but replace A with the one specified in the "TDI128/BIN2" Profile
   * called "MyNew_A_Matrix.cub" for all HiRISE images that were acquired with
   * TDI128 and BIN2 imaging parameters.
   * @code
   *  Object = Database
   *     Matrices = ("A", "B", "G")
   *
   *     A = "$mro/calibration/matrices/A_TDI{TDI}_BIN{BIN}_????.cub"
   *     B ="$mro/calibration/matrices/B_TDI{TDI}_BIN{BIN}_????.cub"
   *     G = "$mro/calibration/matrices/G_TDI{TDI}_BIN{BIN}_????.cub"
   *
   *    Group = Profile
   *      Name = "TDI128/BIN2"
   *      A = "MyNew_A_Matrix.cub"
   *    EndGroup
   *  EndObject
   * @ingroup Utility
   * @author 2007-06-27 Kris Becker
   * @history 2008-04-01 Kris Becker Added Phobos and Deimos to I/F modified
   *          target list as Mars in sundistanceAU() method so it returns
   *          acceptable distances
   * @history 2010-04-16 Kris Becker Added abstracted method, getChannelIndex(),
   *          to compute the new CcdChannelIndex keyword.
   * @history 2015-07-31 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *          were signaled. References #2248.
   * @history 2019-05-16 Jesse Mapel - Added Mars satellite kernel because the
   *          base planet orbit kernel only has Mars Bayrcenter now.
   */
  class HiCalConf : public DbAccess {
    public:
      typedef enum { Matrix, Scalar, Keyword } CalType;
      typedef std::vector<QString> ValueList;

    public:
      //  Constructors and Destructor
      HiCalConf();
      HiCalConf(Pvl &label);
      HiCalConf(Pvl &label, const QString &conf);

      /** Destructor ensures everything is cleaned up properly */
      virtual ~HiCalConf () { }

      void setLabel(Pvl &label);
      PvlKeyword &getKey(const QString &key, const QString &group = "");

      QString filepath(const QString &fname) const;
      void setConf(const QString &conf);
      void selectProfile(const QString &profile = "");

      QString getProfileName() const;
      QString getMatrixSource(const QString &name) const;
      QString getMatrixSource(const QString &name,
                                  const DbProfile &matconf) const;
      HiVector getMatrix(const QString &name, int expected_size = 0) const;
      HiVector getMatrix(const QString &name, const DbProfile &profile,
                         int expected_size = 0) const;
      HiVector getScalar(const QString &name, const DbProfile &profile,
                                int expected_size) const;
      int getMatrixBand() const;
      int getMatrixBand(const DbProfile &p) const;

      double sunDistanceAU();

      DbProfile getMatrixProfile(const QString &profile = "") const;
      ValueList getList(const DbProfile &profile, const QString &key) const;
      QString resolve(const QString &composite,
                          const DbProfile &matconf) const;

    private:
      static bool  _naifLoaded;  //!< Ensures one instance of NAIF kernels
      QString  _profName;    //!< Specified name of profile
      Pvl          _label;       //!< Hold label for future references

      QString  _filter;      //!< Filter set name (RED, IR, BG)


      void init();
      void init(Pvl &label);
      void loadNaifTiming();
      DbProfile getLabelProfile(const DbProfile &profile) const;
      int getChannelIndex(const int &ccd, const int &channel) const;
      DbProfile makeParameters(Pvl &label) const;
      DbProfile makeParameters(const DbProfile &profile) const;
      QString makePattern(const QString &str) const;
      QString parser(const QString &s, const ValueList &options,
                     const DbProfile &prof ) const;
    };

}     // namespace Isis
#endif
