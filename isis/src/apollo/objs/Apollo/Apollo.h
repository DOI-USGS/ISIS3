#ifndef Apollo_h
#define Apollo_h

/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2005/10/03 22:43:39 $
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

#include "iTime.h"
#include "iString.h"
#include "IException.h"

namespace Isis {
/**
 * @brief Reads user Apollos from a data file.
 *
 * This class is used to
 *
 * @author 2007-02-07 Jacob Danton
 *
 * @internal
 *   @history 2007-02-07 Jacob Danton - Original version
 */

  class Apollo {
    public:

      Apollo (iString spacecraft, iString instrument) {
        initialize(spacecraft.UpCase(), instrument.UpCase());
      };

      Apollo(iString filename) {
        iString spacecraft, instrument;
        if (filename.substr(0,4) == "AS15") spacecraft = "APOLLO 15";
        else if (filename.substr(0,4) == "AS16") spacecraft = "APOLLO 16";
        else if (filename.substr(0,4) == "AS17") spacecraft = "APOLLO 17";
        // throw an error
        else {
          iString msg = "The image filename does not match the required formatting.";
          throw IException(IException::User,msg,_FILEINFO_);
        }

        if (filename.substr(5,1) == "M") instrument = "METRIC";
        else if (filename.substr(5,1) == "P") instrument = "PANORAMIC";
        else if (filename.substr(5,1) == "H") instrument = "HASSELBLAD";
        // throw an error
        else {
          iString msg = "The image filename does not match the required formatting.";
          throw IException(IException::User,msg,_FILEINFO_);
        }

        initialize(spacecraft, instrument);
      };

      //! Destroys the Apollo object
      ~Apollo () {};

      bool IsMetric () {return p_instrumentId == "METRIC";}
      bool IsPanoramic () {return p_instrumentId == "PANORAMIC";}
      bool IsHasselblad () {return p_instrumentId == "HASSELBLAD";}
      bool IsApollo15 () {return p_spacecraftName == "APOLLO 15";}
      bool IsApollo16 () {return p_spacecraftName == "APOLLO 16";}
      bool IsApollo17 () {return p_spacecraftName == "APOLLO 17";}
      int Width () {return p_imageWidth;};
      int Height () {return p_imageHeight;};
      int Bands () { return p_imageBands;};
      int ReseauDimension () {return p_reseauDimension;};
      double PixelPitch () {return p_imagePixelPitch;};
      iString SpacecraftName () {return p_spacecraftName;};
      iString InstrumentId () {return p_instrumentId;};
      iString NaifFrameCode () {return p_naifFrameCode;};
      iString TargetName () {return "MOON";};
      iTime LaunchDate () {return p_launchDate;};

      private:

      void initialize(iString spacecraft, iString instrument) {
        if (instrument == "METRIC") {
          p_instrumentId = "METRIC";
          p_reseauDimension = 403;
          p_imageWidth = 22900;
          p_imageHeight = 22900;
          p_imageBands = 1;
          p_imagePixelPitch = 200.5;
        }
        else if (instrument == "PANORAMIC") {
          p_instrumentId = "PANORAMIC";
          p_reseauDimension = 0;
          p_imageWidth = 231480; // 228987
          p_imageHeight = 23007;
          p_imageBands = 1;
          p_imagePixelPitch =200.5;
        }
        else if (instrument == "HASSELBLAD") {
          p_instrumentId = "HASSELBLAD";
          p_reseauDimension = 403;
          p_imageWidth = 12800;
          p_imageHeight = 12800;
          p_imageBands = 3;
          p_imagePixelPitch = 200.5;
        }
        else {
          iString msg = "Unknown instrument: " + instrument;
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (spacecraft == "APOLLO 15" ){
          p_spacecraftName = "APOLLO 15";
          // Apollo 15 launched 1971-07-26 at 13:34 GMT
          p_launchDate = "1971-07-26T13:33:39.11";
          if (IsMetric()) p_naifFrameCode = "-915240";
          else if (IsPanoramic()) p_naifFrameCode = "-915230";
        }
        else if (spacecraft == "APOLLO 16") {
          p_spacecraftName = "APOLLO 16";
          // Apollo 16 launched 1972-04-16 at 17:54 GMT
          p_launchDate = "1972-04-16T17:53:36.238";
          if (IsMetric()) p_naifFrameCode = "-916240";
          else if (IsPanoramic()) p_naifFrameCode = "-916230";
        }
        else if (spacecraft == "APOLLO 17") {
          p_spacecraftName = "APOLLO 17";
          // Apollo 17 launched 1972-12-07 at 05:33 GMT
          p_launchDate = "1972-12-07T05:33:00.000";
          if (IsMetric()) p_naifFrameCode = "-917240";
          else if (IsPanoramic()) p_naifFrameCode = "-917230";
        }
        else {
          iString msg = "Unknown spacecraft: " + spacecraft;
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }

      int p_imageWidth, p_imageHeight, p_imageBands, p_reseauDimension;
      double p_imagePixelPitch;
      iString p_spacecraftName, p_instrumentId, p_naifFrameCode;
      iTime p_launchDate;
    };
};

#endif
