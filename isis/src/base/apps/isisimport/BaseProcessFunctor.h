#ifndef BASE_POST_PROCESS_FUNCTOR_H
#define BASE_POST_PROCESS_FUNCTOR_H

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
