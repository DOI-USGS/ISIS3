#include "Isis.h"

#include <QList>

#include "ProcessByLine.h"
#include "Statistics.h"


using namespace Isis;

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class BandStretch {
  public:
    BandStretch(Cube *cube, int band, double variance) {
      m_variance = variance;

      Statistics *stats = cube->getStatistics(band);
      m_average = stats->Average();
      m_standardDeviation = stats->StandardDeviation();
      delete stats;
    }

    ~BandStretch() {}

    double calculateStretch(double dn) const {
      return !IsSpecial(dn) ?
          (dn - m_average) * (m_variance / m_standardDeviation) : dn;
    }

  private:
    double m_variance;
    double m_average;
    double m_standardDeviation;
};


/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class SigmaStretcher {
  public:
    SigmaStretcher() {}

    virtual ~SigmaStretcher() {
      for (int i = 0; i < m_bandStretches.size(); i++)
        delete m_bandStretches[i];
    }

    void addStretch(BandStretch *stretch) {
      m_bandStretches.append(stretch);
    }

    void operator()(Buffer &in, Buffer &out) const {
      for (int i = 0; i < in.size(); i++)
        out[i] = m_bandStretches[in.Band(i) - 1]->calculateStretch(in[i]);
    }

  private:
    QList<BandStretch *> m_bandStretches;
};


void IsisMain() {
  ProcessByLine p;
  Cube *cube = p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  SigmaStretcher stretcher;
  double variance = Application::GetUserInterface().GetDouble("VARIANCE");
  for (int i = 1; i <= cube->getBandCount(); i++)
    stretcher.addStretch(new BandStretch(cube, i, variance));

  p.ProcessCube(stretcher);
  p.EndProcess();
}

