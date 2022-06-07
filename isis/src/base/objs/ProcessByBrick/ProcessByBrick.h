#ifndef ProcessByBrick_h
#define ProcessByBrick_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <functional>
#include <QtConcurrentMap>
#include <QTime>

#include "Brick.h"
#include "Buffer.h"
#include "Cube.h"
#include "Process.h"
#include "Progress.h"

namespace Isis {
  /**
   * @brief Process cubes by brick
   *
   * This is the processing class used to move a brick through cube data. This
   * class allows only one input cube and one output cube or one input cube. If
   * the brick size does not evenly divide into the image the brick will be padded
   * with Null pixels as it falls off the right and/or bottom edge of the image.
   * The brick shape is only spatial-oriented with one band of data.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2006-03-24 Jacob Danton
   *
   * @internal
   *   @history 2005-02-08 Jacob Danton - Original Version
   *   @history 2006-08-08 Debbie A. Cook - Added overloaded method SetInputCube
   *   @history 2007-12-14 Steven Lambright - Same band is now forced on input
   *                           cubes when there are multiple input cubes
   *   @history 2008-01-09 Steven Lambright - Fixed a memory leak
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation
   *   @history 2011-04-22 Sharmila Prasad - Extended StartProcess functionality
   *                           to be able to be called from any Object class by
   *                           using Functors
   *   @history 2011-05-07 Sharmila Prasad - 1. Added API SetInputCube(Cube*) to
   *                           take opened cube 2. Edited StartProcess using
   *                           Functors take reference to Functors
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2011-11-23 Jai Rideout - Modified the two StartProcess() methods
   *                           that either accept one input and one output
   *                           cube, or multiple input and output cubes so that
   *                           the area that the brick traverses is the largest
   *                           of all of the cubes (including both input and
   *                           output cubes). This resolves the issue of
   *                           premature wrapping that would occur when the
   *                           sizes of the cubes differed. Now, the bricks are
   *                           filled with nulls if they read past the end of a
   *                           smaller cube before they have reached the end of
   *                           the larger one. These changes only take effect
   *                           when the wrap option is off, otherwise the
   *                           previous behavior is used.
   *   @history 2012-02-22 Steven Lambright - Refactored functor-based
   *                           StartProcess() calls. This class now supports
   *                           multi-threaded functor and function based
   *                           programs. StartProcess() is now deprecated
   *                           in favor of ProcessCube(), ProcessCubes(), and
   *                           ProcessCubeInPlace(). Added Finalize().
   *   @history 2013-03-27 Jeannie Backer - Modified SetBrickSize() code to call
   *                           existing methods rather than duplicating code.
   *                           Added SetOutputCube() method.References #1248.
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to several
   *                           functions to ensure successful inheritance between Process and its
   *                           child classes.  Also made destructor virtual.  Fixes #2215.
   *   @history 2016-04-26 Ian Humphrey - Modified BlockingReportProgress() so that it unlocks
   *                           the local QMutex before it goes out of scope (Qt5 issues a warning
   *                           if a locked QMutex is destroyed).
   *   @history 2017-05-08 Tyler Wilson - Added a call to the virtual method SetBricks inside
   *                          the functions PreProcessCubeInPlace/PreProcessCube/PreProcessCubes.
   *                          Fixes #4698.
   *   @history 2022-04-22 Jesse Mapel - Added std::function process method for multiple
   *                          input and output cubes.
   */
  class ProcessByBrick : public Process {
    public:
      //! Constructs a ProcessByBrick object
      ProcessByBrick();

      //! Destroys the ProcessByBrick object
      virtual ~ProcessByBrick();

      enum IOCubes{InPlace,
                   InputOutput,
                   InputOutputList};

      enum ProcessingDirection {
        LinesFirst,
        BandsFirst
      };

      using Process::SetInputCube;
      Cube *SetInputCube(const QString &parameter,
                         int requirements = 0);

      Cube *SetInputCube(const QString &fname,
                         const CubeAttributeInput &att,
                         int requirements = 0);

      virtual void SetBricks(IOCubes cn);
      void VerifyCubes(IOCubes cn);

      void SetBrickSize(int ns, int nl, int nb);

      void SetInputBrickSize(int ns, int nl, int nb);
      void SetInputBrickSize(int ns, int nl, int nb, int cube);

      void SetOutputBrickSize(int ns, int nl, int nb);
      void SetOutputBrickSize(int ns, int nl, int nb, int cube);

      // Overload the SetOutputCube() method to allow the user to pass in the
      // file name and attributes without the lines and samples.
      // Any other calls to this method will use the prototypes found in the
      //process class due to the using statement below.
      using Isis::Process::SetOutputCube;
      virtual Cube *SetOutputCube(const QString &fname,
                                  const CubeAttributeOutput &att);

      void SetProcessingDirection(ProcessingDirection direction);
      ProcessingDirection GetProcessingDirection();

      void SetOutputRequirements(int outputRequirements);
      void SetWrap(bool wrap);
      bool Wraps();

      using Isis::Process::StartProcess;  // make parents virtual function visable
      virtual void StartProcess(void funct(Buffer &in));
      virtual void StartProcess(std::function<void(Buffer &in)> funct );
      virtual void StartProcess(void funct(Buffer &in, Buffer &out));
      virtual void StartProcess(std::function<void(Buffer &in, Buffer &out)> funct);
      virtual void StartProcess(void funct(std::vector<Buffer *> &in,
                                           std::vector<Buffer *> &out));
      virtual void StartProcess(std::function<void(std::vector<Buffer *> &in,
                                                   std::vector<Buffer *> &out)> funct);
      void EndProcess();// Depricated. Please use Finalize
      void Finalize();


      /**
       * Operate over a single cube (either input or output). The functor you
       *   pass in will be called for every position in the cube. If threaded
       *   is true, there is no guarantee to the sequence or timing of the
       *   functor's calls.
       *
       * If you are using a function, the prototype should look like:
       *   void SomeFunc(Buffer &);
       * If you are using a functor, the () operator should look like:
       *   void operator()(Buffer &) const;
       *
       * When threaded is true, your function (or functor's () operator)
       *   must be thread safe. Please document your function appropriately if
       *   it is thread safe.
       *
       * @param functor The processing function or functor which does your
       *     desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       *     Sequential calling of the functor is guaranteed if this is false.
       */
      template <typename Functor> void ProcessCubeInPlace(
          const Functor & functor, bool threaded = true) {
        Cube *cube = NULL;
        Brick *brick = NULL;

        bool haveInput = PrepProcessCubeInPlace(&cube, &brick);
        bool writeOutput = (!haveInput) || (cube->isReadWrite());

        ProcessCubeInPlaceFunctor<Functor> wrapperFunctor(
            cube, brick, haveInput, writeOutput, functor);

        RunProcess(wrapperFunctor, brick->Bricks(), threaded);

        delete brick;
      }


      /**
       * Operate over a single input cube creating a separate output cube. The
       *   functor you pass in will be called for every position in the cube.
       *   If threaded is true, there is no guarantee to the sequence or timing
       *   of the functor's calls.
       *
       * If you are using a function, the prototype should look like:
       *   void SomeFunc(Buffer &inputData, Buffer &outputData);
       * If you are using a functor, the () operator should look like:
       *   void operator()(Buffer &inputData, Buffer &outputData) const;
       *
       * When threaded is true, your function (or functor's () operator)
       *   must be thread safe. Please document your function appropriately if
       *   it is thread safe.
       *
       * @param functor The processing function or functor which does your
       *     desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       *     Sequential calling of the functor is guaranteed if this is false.
       */
      template <typename Functor> void ProcessCube(const Functor & functor,
                                                   bool threaded = true) {
        Brick *inputCubeData = NULL;
        Brick *outputCubeData = NULL;

        int numBricks = PrepProcessCube(&inputCubeData, &outputCubeData);

        ProcessCubeFunctor<Functor> wrapperFunctor(InputCubes[0], inputCubeData,
            OutputCubes[0], outputCubeData, functor);

        RunProcess(wrapperFunctor, numBricks, threaded);

        delete inputCubeData;
        delete outputCubeData;
      }


      /**
       * Operate over an arbitrary number of input cubes given an arbitrary
       *   number of output cubes. The functor you pass in will be called for
       *   every position in the largest output cube. If threaded is true, there
       *   is no guarantee to the sequence or timing of the functor's calls.
       *
       * If you are using a function, the prototype should look like:
       *   void SomeFunc(std::vector<Buffer *> &inputData,
       *                 std::vector<Buffer *> &outputData);
       * If you are using a functor, the () operator should look like:
       *   void operator()(std::vector<Buffer *> &inputData,
       *                   std::vector<Buffer *> &outputData) const;
       *
       * When threaded is true, your function (or functor's () operator)
       *   must be thread safe. Please document your function appropriately if
       *   it is thread safe.
       *
       * @param functor The processing function or functor which does your
       *     desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       *     Sequential calling of the functor is guaranteed if this is false.
       */
      template <typename Functor> void ProcessCubes(const Functor & functor,
                                                    bool threaded = true) {
        std::vector<Brick *> inputCubeData;
        std::vector<Brick *> outputCubeData;

        std::vector<Buffer *> inputCubeDataParents;
        std::vector<Buffer *> outputCubeDataParents;

        int numBricks = PrepProcessCubes(
            inputCubeDataParents, outputCubeDataParents,
            inputCubeData, outputCubeData);

        ProcessCubesFunctor<Functor> wrapperFunctor(InputCubes, inputCubeData,
              OutputCubes, outputCubeData, Wraps(), functor);

        RunProcess(wrapperFunctor, numBricks, threaded);

        for(unsigned int i = 0; i < inputCubeData.size(); i++) {
          delete inputCubeData[i];
        }

        for(unsigned int i = 0; i < outputCubeData.size(); i++) {
          delete outputCubeData[i];
        }
      }


    private:
      /**
       * This method runs the given wrapper functor numSteps times with
       *   or without threading, reporting progress in both cases. This method
       *   is a blocking call.
       *
       * @param wrapperFunctor A functor that does the reading, processing, and
       *            writing required given a ProcessIterator position in the
       *            cube.
       * @param numSteps The end() value for the process iterator.
       * @param threaded Force threading off when set to false; threading may or
       *            may not be used if this is true.
       */
      template <typename Functor>
      void RunProcess(const Functor &wrapperFunctor,
                      int numSteps, bool threaded) {
        ProcessIterator begin(0);
        ProcessIterator end(numSteps);

        p_progress->SetMaximumSteps(numSteps);
        p_progress->CheckStatus();

        int threadCount = QThreadPool::globalInstance()->maxThreadCount();
        if (threaded && threadCount > 1) {
          QFuture<void> result = QtConcurrent::mapped(begin, end,
              wrapperFunctor);
          BlockingReportProgress(result);
        }
        else {
          while (begin != end) {
            wrapperFunctor(*begin);
            ++begin;
           p_progress->CheckStatus();
          }
        }
      }


      /**
       * Process a cube in place (one input/zero output or zero input/one
       *   output or one cube that acts both as input and output). Given a
       *   ProcessIterator position, this runs the functor passed into
       *   ProcessCubeInPlace with the appropriate data.
       *
       * This functor is a helper for the ProcessCubeInPlace() public method.
       *   This is designed to be passed into QtConcurrent::mapped to operate
       *   over a cube.
       *
       * @author 2012-02-22 Steven Lambright
       *
       * @internal
       */
      template <typename T>
      class ProcessCubeInPlaceFunctor :
          public std::unary_function<const int &, void *> {
        public:
          /**
           * Construct a ProcessCubeInPlaceFunctor. This doesn't take ownership
           *   of the passed in pointers but expects them to not be deleted.
           *
           * @param cube The input (or output) cube we're processing
           * @param templateBrick A brick initialized for use with the
           *     processingFunctor
           * @param readInput True if we should read the cube into the brick
           *     before calling the processingFunctor.
           * @param writeOutput True if we should write the resulting brick from
           *     the processingFunctor into the cube
           * @param processingFunctor The functor supplied to
           *     ProcessCubeInPlace() which actually does the work/
           *     calculations.
           */
          ProcessCubeInPlaceFunctor(Cube *cube,
                                    const Brick *templateBrick,
                                    bool readInput, bool writeOutput,
                                    const T &processingFunctor) :
              m_cube(cube),
              m_templateBrick(templateBrick),
              m_readInput(readInput),
              m_writeOutput(writeOutput),
              m_processingFunctor(processingFunctor) {
          }


          /**
           * Copy construction of these objects is fully supported.
           *
           * @param other The functor to copy
           */
          ProcessCubeInPlaceFunctor(const ProcessCubeInPlaceFunctor &other) :
              m_cube(other.m_cube),
              m_templateBrick(other.m_templateBrick),
              m_readInput(other.m_readInput),
              m_writeOutput(other.m_writeOutput),
              m_processingFunctor(other.m_processingFunctor) {
          }


          /**
           * Destructor
           */
          virtual ~ProcessCubeInPlaceFunctor() {
            m_cube = NULL;
            m_templateBrick = NULL;
          }


          /**
           * Do the work for one position in a cube.
           *
           * @param brickPosition The position we're calculating values for
           *                      currently.
           */
          void *operator()(const int &brickPosition) const {
            Brick cubeData(*m_templateBrick);
            cubeData.setpos(brickPosition);

            if (m_readInput)
              m_cube->read(cubeData);

            m_processingFunctor(cubeData);

            if (m_writeOutput)
              m_cube->write(cubeData);

            return NULL;
          }


          /**
           * Assignment of these objects is fully supported.
           *
           * @param rhs The ProcessCubeInPlaceFunctor on the right-hand side
           *            of the assignment.
           * @return *this
           */
          ProcessCubeInPlaceFunctor &operator=(
              const ProcessCubeInPlaceFunctor &rhs) {
            m_cube = rhs.m_cube;
            m_templateBrick = rhs.m_templateBrick;

            m_readInput = rhs.m_readInput;
            m_writeOutput = rhs.m_writeOutput;

            m_processingFunctor = rhs.m_processingFunctor;

            return *this;
          }

        private:
          //! The cube we're I/O'ing on
          Cube *m_cube;
          //! A brick with the right dimensions, pixel type, etc. for processing
          const Brick *m_templateBrick;
          //! Should we read from the cube before processing
          bool m_readInput;
          //! Should we write to the output cube after processing
          bool m_writeOutput;

          //! The functor which does the work/arbitrary calculations
          const T &m_processingFunctor;
       };


      /**
       * Create an output cube given one input cube. Given a
       *   ProcessIterator position, this runs the functor passed into
       *   ProcessCube with the appropriate data.
       *
       * This functor is a helper for the ProcessCube() public method.
       *   This is designed to be passed into QtConcurrent::mapped to operate
       *   over a cube.
       *
       * @author 2012-02-22 Steven Lambright
       *
       * @internal
       */
      template <typename T>
      class ProcessCubeFunctor :
          public std::unary_function<const int &, void *> {
        public:
          /**
           * Construct a ProcessCubeFunctor. This doesn't take ownership
           *   of the passed in pointers but expects them to not be deleted.
           *
           * @param inputCube The cube to read from for input data
           * @param inputTemplateBrick A brick initialized for use with the
           *     processingFunctor's input parameter
           * @param outputCube The cube to write to after running the
           *     processingFunctor
           * @param outputTemplateBrick A brick initialized for use with the
           *     processingFunctor's output parameter
           * @param processingFunctor The functor supplied to
           *     ProcessCube() which actually does the work/
           *     calculations.
           */
          ProcessCubeFunctor(Cube *inputCube,
                             const Brick *inputTemplateBrick,
                             Cube *outputCube,
                             const Brick *outputTemplateBrick,
                             const T &processingFunctor) :
              m_inputCube(inputCube),
              m_inputTemplateBrick(inputTemplateBrick),
              m_outputCube(outputCube),
              m_outputTemplateBrick(outputTemplateBrick),
              m_processingFunctor(processingFunctor) {
          }


          /**
           * Copy construction of these objects is fully supported.
           *
           * @param other The functor to copy
           */
          ProcessCubeFunctor(const ProcessCubeFunctor &other) :
              m_inputCube(other.m_inputCube),
              m_inputTemplateBrick(other.m_inputTemplateBrick),
              m_outputCube(other.m_outputCube),
              m_outputTemplateBrick(other.m_outputTemplateBrick),
              m_processingFunctor(other.m_processingFunctor) {
          }


          /**
           * Destructor
           */
          virtual ~ProcessCubeFunctor() {
            m_inputTemplateBrick = NULL;
            m_outputTemplateBrick = NULL;
          }


          /**
           * Do the work for one position in a cube.
           *
           * @param brickPosition The position we're calculating values for
           *                      currently.
           */
          void *operator()(const int &brickPosition) const {
            Brick inputCubeData(*m_inputTemplateBrick);
            Brick outputCubeData(*m_outputTemplateBrick);

            inputCubeData.setpos(brickPosition);
            outputCubeData.setpos(brickPosition);

            m_inputCube->read(inputCubeData);

            m_processingFunctor(inputCubeData, outputCubeData);

            m_outputCube->write(outputCubeData);

            return NULL;
          }


          /**
           * Assignment of these objects is fully supported.
           *
           * @param rhs The ProcessCubeFunctor on the right-hand side
           *            of the assignment.
           * @return *this
           */
          ProcessCubeFunctor &operator=(const ProcessCubeFunctor &rhs) {
            m_inputCube = rhs.m_inputCube;
            m_inputTemplateBrick = rhs.m_inputTemplateBrick;

            m_outputCube = rhs.m_outputCube;
            m_outputTemplateBrick = rhs.m_outputTemplateBrick;

            m_processingFunctor = rhs.m_processingFunctor;

            return *this;
          }

        private:
          //! The cube to read from for the input brick data
          Cube *m_inputCube;
          //! An example brick for the input parameter to m_processingFunctor
          const Brick *m_inputTemplateBrick;

          //! The cube to write to with the output of m_processingFunctor
          Cube *m_outputCube;
          //! An example brick for the output parameter to m_processingFunctor
          const Brick *m_outputTemplateBrick;

          //! The functor which does the work/arbitrary calculations
          const T &m_processingFunctor;
       };


      /**
       * Create an arbitrary number of output cubes given an arbitrary number
       *   of input cubes (these counts can be zero). Given a
       *   ProcessIterator position, this runs the functor passed into
       *   ProcessCubes with the appropriate data.
       *
       * This functor is a helper for the ProcessCubes() public method.
       *   This is designed to be passed into QtConcurrent::mapped to operate
       *   over the pre-set cubes.
       *
       * @author 2012-02-22 Steven Lambright
       *
       * @internal
       */
      template <typename T>
      class ProcessCubesFunctor :
          public std::unary_function<const int &, void *> {
        public:
          /**
           * Construct a ProcessCubesFunctor. This doesn't take ownership
           *   of the passed in pointers but expects them to not be deleted.
           *
           * @param inputCubes The cubes to read from for input data
           * @param inputTemplateBricks The brick initialized for use with the
           *     processingFunctor's input parameter. These must be in the same
           *     order as the inputCubes.
           * @param outputCubes The cubes to write to after running the
           *     processingFunctor
           * @param outputTemplateBricks The bricks initialized for use with the
           *     processingFunctor's output parameter. These must be in the same
           *     order as the outputCubes.
           * @param wraps The current setting for the ProcessByBrick::Wrap()
           *     option.
           * @param processingFunctor The functor supplied to
           *     ProcessCubes() which actually does the work/
           *     calculations.
           */
          ProcessCubesFunctor(std::vector<Cube *> &inputCubes,
                              std::vector<Brick *> &inputTemplateBricks,
                              std::vector<Cube *> &outputCubes,
                              std::vector<Brick *> &outputTemplateBricks,
                              bool wraps,
                              const T &processingFunctor) :
              m_inputCubes(inputCubes),
              m_inputTemplateBricks(inputTemplateBricks),
              m_outputCubes(outputCubes),
              m_outputTemplateBricks(outputTemplateBricks),
              m_wraps(wraps),
              m_processingFunctor(processingFunctor) {
          }


          /**
           * Copy construction of these objects is fully supported.
           *
           * @param other The functor to copy
           */
          ProcessCubesFunctor(const ProcessCubesFunctor &other)  :
              m_inputCubes(other.m_inputCubes),
              m_inputTemplateBricks(other.m_inputTemplateBricks),
              m_outputCubes(other.m_outputCubes),
              m_outputTemplateBricks(other.m_outputTemplateBricks),
              m_wraps(other.m_wraps),
              m_processingFunctor(other.m_processingFunctor) {
          }


          /**
           * Destructor
           */
          virtual ~ProcessCubesFunctor() {
          }


          /**
           * Do the work for one position in a cube.
           *
           * @param brickPosition The position we're calculating values for
           *                      currently.
           */
          void *operator()(const int &brickPosition) const {
            QPair< std::vector<Buffer *>, std::vector<Buffer *> > functorBricks;

            for (int i = 0; i < (int)m_inputTemplateBricks.size(); i++) {
              Brick *inputBrick = new Brick(*m_inputTemplateBricks[i]);
              functorBricks.first.push_back(inputBrick);

              if (m_wraps) {
                inputBrick->setpos(brickPosition % inputBrick->Bricks());
              }
              else {
                inputBrick->setpos(brickPosition);
              }

              if (i != 0 &&
                  functorBricks.first.size() &&
                  inputBrick->Band() != functorBricks.first[0]->Band() &&
                  m_inputCubes[i]->bandCount() != 1) {
                inputBrick->SetBaseBand(functorBricks.first[0]->Band());
              }

              m_inputCubes[i]->read(*inputBrick);
            }

            for (int i = 0; i < (int)m_outputTemplateBricks.size(); i++) {
              Brick *outputBrick = new Brick(*m_outputTemplateBricks[i]);
              functorBricks.second.push_back(outputBrick);
              outputBrick->setpos(brickPosition);
            }

            // Pass them to the application function
            m_processingFunctor(functorBricks.first, functorBricks.second);

            // And copy them into the output cubes
            for (int i = 0; i < (int)functorBricks.second.size(); i++) {
              m_outputCubes[i]->write(*functorBricks.second[i]);
              delete functorBricks.second[i];
            }

            for (int i = 0; i < (int)functorBricks.first.size(); i++) {
              delete functorBricks.first[i];
            }

            return NULL;
          }


          /**
           * Assignment of these objects is fully supported.
           *
           * @param rhs The ProcessCubeFunctor on the right-hand side
           *            of the assignment.
           * @return *this
           */
          ProcessCubesFunctor &operator=(const ProcessCubesFunctor &rhs) {
            m_inputCubes = rhs.m_inputCubes;
            m_inputTemplateBricks = rhs.m_inputTemplateBricks;

            m_outputCubes = rhs.m_outputCubes;
            m_outputTemplateBricks = rhs.m_outputTemplateBricks;

            m_wraps = rhs.m_wraps;

            m_processingFunctor = rhs.m_processingFunctor;

            return *this;
          }

        private:
          //! The input cubes for reading data from
          std::vector<Cube *> m_inputCubes;
          /**
           * Template bricks for reading input data. Must be parallel to
           *   inputCubes.
           */
          std::vector<Brick *> &m_inputTemplateBricks;

          //! The output cubes for writing data to
          std::vector<Cube *> m_outputCubes;
          /**
           * Template bricks for writing output data. Must be parallel to
           *   oututCubes.
           */
          std::vector<Brick *> &m_outputTemplateBricks;

          //! Wrap smaller cubes back to the beginning?
          bool m_wraps;

          //! The functor which does the work/arbitrary calculations
          const T &m_processingFunctor;
       };


      void BlockingReportProgress(QFuture<void> &future);
      std::vector<int> CalculateMaxDimensions(std::vector<Cube *> cubes) const;
      bool PrepProcessCubeInPlace(Cube **cube, Brick **bricks);
      int PrepProcessCube(Brick **ibrick, Brick **obrick);
      int PrepProcessCubes(std::vector<Buffer *> & ibufs,
                           std::vector<Buffer *> & obufs,
                           std::vector<Brick *> & imgrs,
                           std::vector<Brick *> & omgrs);


      /**
       * This class is designed to iterate over all brick positions in a cube.
       *   This isn't using a BigInt because Brick::Bricks() isn't, so a
       *   performance penalty would be incurred for no reason.
       *
       * @author 2012-02-22 Steven Lambright
       *
       * @internal
       */
      class ProcessIterator : public std::iterator<
          std::forward_iterator_tag, int> {
        public:
          ProcessIterator(int position);
          ProcessIterator(const ProcessIterator &other);
          virtual ~ProcessIterator();

          ProcessIterator &operator++();

          /**
           * Compare equality of two iterator positions.
           *
           * @param rhs The right hand side of the '==' operator
           * @return True if this and rhs are equal
           */
          bool operator==(const ProcessIterator &rhs) {
            return (m_currentPosition == rhs.m_currentPosition);
          }

          /**
           * Compare inequality of two iterator positions.
           *
           * @param rhs The right hand side of the '!=' operator
           * @return True if this and rhs are not equal
           */
          bool operator!=(const ProcessIterator &rhs) {
            return !(*this == rhs);
          }


          /**
           * Exception-safe swap method.
           *
           * @param other The instance to swap with
           */
          void swap(ProcessIterator &other) {
            std::swap(m_currentPosition, other.m_currentPosition);
          }


          /**
           * Assignment of these iterators is fully supported.
           *
           * @param rhs The right hand side of the '=' operator.
           * @return *this
           */
          ProcessIterator &operator=(const ProcessIterator &rhs) {
            ProcessIterator copy(rhs);
            swap(copy);
            return *this;
          }


          /**
           * Convert this iterator into a position. An integer is sufficient.
           * @return The brick position to use
           */
          int operator*() const {
            return m_currentPosition;
          }

        private:
          //! The current iterator's position/value
          int m_currentPosition;
      };

    private:
      bool p_reverse; /**< Use the reverse option for constructing the Buffer
                        objects when the Processing Direction is changed from
                        LinesFirst to BandsFirst*/
      bool p_wrapOption;    //!< Indicates whether the brick manager will wrap
      bool p_inputBrickSizeSet;  /**< Indicates whether the brick size has been
                                      set*/
      bool p_outputBrickSizeSet; /**< Indicates whether the brick size has been
                                      set*/

      int p_outputRequirements;


      std::vector<int> p_inputBrickSamples;  /**< Number of samples in the input
                                                  bricks*/
      std::vector<int> p_inputBrickLines;    /**< Number of lines in the input
                                                  bricks*/
      std::vector<int> p_inputBrickBands;    /**< Number of bands in the input
                                                  bricks*/
      std::vector<int> p_outputBrickSamples; /**< Number of samples in the
                                                  output bricks*/
      std::vector<int> p_outputBrickLines;   /**< Number of lines in the output
                                                  bricks*/
      std::vector<int> p_outputBrickBands;   /**< Number of bands in the output
                                                  bricks*/




  };

};

#endif
