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
  class OverlapNormalization;
  class OverlapStatistics;
  class Pvl;
  class PvlGroup;
  class Statistics;


  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-04-26 Jeannie Backer - Added includes to Filelist, Pvl,
   *                           PvlGroup, and Statistics classes to the
   *                           implementation file.
   */
  class Equalization {
    protected:
      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
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

      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class CalculateFunctor {
        public:
          CalculateFunctor(Statistics *stats, double percent) {
            m_stats = stats;
            m_linc = (int) (100.0 / percent + 0.5);
          }

          virtual ~CalculateFunctor() {}

          void operator()(Buffer &in) const;

        protected:
          virtual void addStats(Buffer &in) const;

        private:
          Statistics *m_stats;
          int m_linc;
      };

      /**
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class ApplyFunctor {
        public:
          ApplyFunctor(const ImageAdjustment *adjustment) {
            m_adjustment = adjustment;
          }

          void operator()(Buffer &in, Buffer &out) const;

        private:
          const ImageAdjustment *m_adjustment;
      };

      Equalization();

    public:
      Equalization(QString fromListName);
      virtual ~Equalization();

      void addHolds(QString holdListName);

      void calculateStatistics(double sampPercent, int mincnt,
          bool wtopt, int sType);
      void importStatistics(QString instatsFileName);
      void applyCorrection(QString toListName);

      PvlGroup getResults();
      void write(QString outstatsFileName);

      double evaluate(double dn, int imageIndex, int bandIndex) const;

    protected:
      void loadInputs(QString fromListName);
      void setInput(int index, QString value);
      const FileList & getInputs() const;

      virtual void fillOutList(FileList &outList, QString toListName);
      virtual void errorCheck(QString fromListName);

      void generateOutputs(FileList &outList);
      void loadOutputs(FileList &outList, QString toListName);
      void loadHolds(OverlapNormalization *oNorm);

      void setResults(std::vector<OverlapStatistics> &overlapStats);
      void setResults();

      void clearAdjustments();
      void addAdjustment(ImageAdjustment *adjustment);

      void addValid(int count);
      void addInvalid(int count);

    private:
      void init();
      std::vector<int> validateInputStatistics(QString instatsFileName);

      FileList m_imageList;
      std::vector<ImageAdjustment *> m_adjustments;
      std::vector<int> m_holdIndices;

      int m_validCnt;
      int m_invalidCnt;

      int m_mincnt;
      bool m_wtopt;

      int m_maxCube;
      int m_maxBand;

      Pvl *m_results;
  };
};

#endif
