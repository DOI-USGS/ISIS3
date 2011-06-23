#ifndef Equalization_h
#define Equalization_h

/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/11/25 22:09:21 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

// TODO Qt instead
#include <string>
#include <vector>

// TODO Don't include
#include "FileList.h"


namespace Isis {
  class Buffer;
  class Cube;
  class FileList;
  class OverlapStatistics;
  class PvlGroup;
  class Statistics;


  class Equalization {
    public:
      Equalization(std::string fromListName);
      virtual ~Equalization();

      void addHolds(std::string holdListName);

      std::vector<OverlapStatistics> calculateStatistics(
          double sampPercent, int mincnt, bool wtopt, int sType);
      void importStatistics(std::string instatsFileName);
      void applyCorrection(std::string toListName);

      PvlGroup getResults();
      void write(std::string outstatsFileName,
          std::vector<OverlapStatistics> *overlapStats=NULL);

    private:
      void loadOutputs(FileList &outList, std::string toListName);
      std::vector<int> validateInputStatistics(std::string instatsFileName);
      Statistics getBandStatistics(Cube &icube, const int band,
          double sampPercent, std::string maxCubeStr);
      void apply(Buffer &in, Buffer &out);

      class ImageAdjustment {
        public:
          ImageAdjustment() {}
          ~ImageAdjustment() {}

          void addGain(double gain) {
            gains.push_back(gain);
          }

          void addOffset(double offset) {
            offsets.push_back(offset);
          }

          void addAverage(double average) {
            avgs.push_back(average);
          }

          double getGain(int index) const {
            return gains[index];
          }

          double getOffset(int index) const {
            return offsets[index];
          }

          double getAverage(int index) const {
            return avgs[index];
          }

          double evaluate(double dn, int index) const {
            double gain = gains[index];
            double offset = offsets[index];
            double avg = avgs[index];
            return (dn - avg) * gain + offset + avg;
          }

        private:
          std::vector<double> gains;
          std::vector<double> offsets;
          std::vector<double> avgs;
      };

      class EqualizationFunctor {
        public:
          EqualizationFunctor(const ImageAdjustment *adjustment) {
            m_adjustment = adjustment;
          }

          void operator()(Buffer &in, Buffer &out);

        private:
          const ImageAdjustment *m_adjustment;
      };

      FileList m_imageList;
      std::vector<ImageAdjustment *> adjustments;
      std::vector<int> hold;

      int m_validCnt;
      int m_invalidCnt;

      int m_mincnt;
      bool m_wtopt;

      int m_currentImage;
      int m_maxCube;
      int m_maxBand;
  };
};

#endif
