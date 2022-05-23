#ifndef ISISIMPORT_BASE_PROCESS_FUNCTOR_H
#define ISISIMPORT_BASE_PROCESS_FUNCTOR_H

namespace Isis {
  class ProcessFunctor {
    public:
      /**
       * Constructs a processFunctor
       *
       */
      ProcessFunctor() {}

      virtual ~ProcessFunctor() {}

      void operator()(Buffer &buf) const {};
  };
}

#endif
