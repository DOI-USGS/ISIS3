#if !defined(GainChannelNormalize_h)
#define GainChannelNormalize_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/05/14 21:07:22 $
 * $Id: GainChannelNormalize.h,v 1.2 2008/05/14 21:07:22 slambright Exp $
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
#include <cmath>
#include <string>
#include <vector>

#include "iString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "Module.h"
#include "Filename.h"

#include "IException.h"

namespace Isis {

  /**
   * @brief Computes a gain correction for each sample GainChannelNormalize
   *
   * This class computes the HiRISE gain component correction for each sample.
   *
   * @ingroup Utility
   *
   * @author 2010-04-19 Kris Becker
   * @internal
   */
  class GainChannelNormalize : public Module {

    public:
      //  Constructors and Destructor
      GainChannelNormalize() : Module("GainChannelNormalize"),
                               _normalizer(0.0) { }
      GainChannelNormalize(const HiCalConf &conf) :
                           Module("GainChannelNormalize"), _normalizer(0.0) {
        init(conf);
      }

      /** Destructor */
      virtual ~GainChannelNormalize() { }

    private:
      double   _normalizer;

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");

        double bin = ToDouble(prof("Summing"));
        double tdi = ToDouble(prof("Tdi"));
        double _normalizer = 128.0 / tdi / (bin*bin);
        _history.add("ModeNormalizer["+ToString(_normalizer)+"]");

        HiVector z = loadCsv("Gains", conf, prof, 0);
        int nsamps = ToInteger(prof("Samples"));
        _data = HiVector(nsamps);
        // Support a single value being read or equal to the number of samples
        if ( z.dim() == 1 ) {
          _data = z[0];
        }
        else if ( z.dim() == nsamps) {
          _data = z;
        }
        else {
          std::ostringstream mess;
          mess << "Expected 1 or " << nsamps << " values from CSV file "
               << getcsvFile() << " but got " << z.dim() << " instead!";
          throw IException(IException::User, mess.str(), _FILEINFO_);
        }

        //  Apply the factor to the data
        for ( int i = 0 ; i < _data.dim() ; i++ ) { _data[i] *= _normalizer; }
        return;
      }

  };

}     // namespace Isis
#endif

