#if !defined(DarkModelPixel_h)
#define DarkModelPixel_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2008/09/04 18:48:13 $
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
#include <cmath>

#include "MdisCalUtils.h"
#include "Filename.h"
#include "CSVReader.h"
#include "tnt_array1d.h"
#include "tnt_array1d_utils.h"

namespace Isis {


class DarkModelPixel {
  public:
    DarkModelPixel() : _scale(1),_ccdTemp(0.0),_expTime(0.0),_coefs(8, 0.0),
                       _filename() { } 
    DarkModelPixel(int pixelBinning) : _scale(1),_ccdTemp(0.0),_expTime(0.0),
                                       _coefs(8, 0.0), _filename() { 
      setPixelBinning(pixelBinning);
    }

    DarkModelPixel(int pixelBinning, double ccdTemp, double expTime) : 
                   _scale(1), _ccdTemp(ccdTemp),_expTime(expTime), 
                   _coefs(8, 0.0) { 
      setPixelBinning(pixelBinning);
      setCCDTemperature(ccdTemp);
      setExposureTime(expTime);
    }

    ~DarkModelPixel() { }

    void setPixelBinning(int pxlBin) {
      if ( pxlBin > 0 ) { _scale = (int) std::pow(2.0, pxlBin); }
      else { _scale = 1; }
      return;
    }
    void setCCDTemperature(double ccdTemp) {
      _ccdTemp = ccdTemp;
      return;
    }

    /**
     * @brief Set exposure time
     * @param expTime Exposure time of image in seconds
     */
    void setExposureTime(const double &expTime) {
      _expTime = expTime * 1000.0;
    }


    std::string loadCoefficients(bool isNac, bool isFpuBinned) {
      std::string filename = "$messenger/calibration/DARK_MODEL/MDIS";
      // Filename consists of binned/notbinned, camera, and filter
      filename += (isNac)? "NAC" : "WAC";
      filename += (isFpuBinned)? "_BINNED_" : "_NOTBIN_";
      filename += "DARKMODEL_?.TAB";

      Filename finalName(filename);
      finalName.HighestVersion();
      filename = finalName.OriginalPath() + "/" + finalName.Name();
      _filename = filename;

      //  Open the CSV file
      CSVReader csv(finalName.Expanded());
      DVector coefs(8);
      double ccdTempSqrd = _ccdTemp * _ccdTemp;
      double ccdTempCubed = ccdTempSqrd * _ccdTemp;
      for ( int i = 0 ; i < 8 ; i++ ) {
        CSVReader::CSVAxis row = csv.getRow(i);
        DVector values(4);
        for(int v = 0 ; v < 4 ; v++) {
          values[v] = ToDouble(row[v]);
        }

      // temps are in order in the file
        coefs[i] = values[0] + 
                   values[1] * _ccdTemp +
                   values[2] * ccdTempSqrd + 
                   values[3] * ccdTempCubed;
      }

      //  All done, save off coefficients and return filename
      _coefs = coefs;
      return (filename);
    }

    std::string getFilename() const { return (_filename); }

    double getDarkPixel(int sample, int line) const {
       double sum(0.0);
       double npixels(0.0);
       int line0 = line * _scale;
       for ( int l = 0 ; l < _scale ; l++, line0++ ) {
         int samp0 = sample * _scale;
         for ( int s = 0 ; s < _scale ; s++, samp0++ ) {
             // ctemp + dtemp*exposure
            double acoef = _coefs[0] + _coefs[1] * _expTime;
            double bcoef = _coefs[2] + _coefs[3] * _expTime;
            double mcoef = _coefs[4] + _coefs[5] * _expTime;
            double ncoef = _coefs[6] + _coefs[7] * _expTime;

            double alphacoef = acoef + bcoef * line0;
            double betacoef  = mcoef + ncoef * line0;

            sum += (alphacoef + betacoef * samp0);
            npixels += 1.0;
         }
       }

       //  Return the average
       return (sum/npixels);
    }

  private:
    typedef TNT::Array1D<double> DVector;       //!<  1-D Buffer
    int _scale;
    double _ccdTemp;
    double _expTime;
    DVector _coefs;
    std::string _filename;
};

};
#endif

