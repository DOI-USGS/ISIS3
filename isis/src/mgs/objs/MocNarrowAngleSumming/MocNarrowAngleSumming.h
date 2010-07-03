#ifndef MocNarrowAngleSumming_h
#define MocNarrowAngleSumming_h

namespace Isis {
  namespace Mgs {
    class MocNarrowAngleSumming {
      public:
        // constructors
        MocNarrowAngleSumming (int csum, int ss) {
          p_csum = csum;
          p_ss = ((double) csum / 2.0) + 0.5 + (double) (ss - 1);
        }
    
        // destructor
        ~MocNarrowAngleSumming () {};
    
        inline double Detector (double sample) const {
          return (sample - 1.0) * (double) p_csum + p_ss;
        }
    
        inline double Sample (double detector) const {
          return (detector - p_ss) / ((double) p_csum) + 1.0;
        }
    
      private:
        int p_csum;
        double p_ss;
    
    };
  };
};

#endif
