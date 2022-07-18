/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include <ctime>
/*#define USE_GUI_QAPP 1*/

#include <QApplication>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QFile>
#include <QTextStream>

// OpenCV stuff
#include "opencv2/core.hpp"

// boost library
#include <boost/foreach.hpp>

#define HAVE_ISNAN

#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "Distance.h"
#include "Environment.h"

#include "FastGeom.h"
#include "FeatureAlgorithmFactory.h"
#include "FileList.h"
#include "GenericTransform.h"
#include "ID.h"
#include "ImageSource.h"
#include "ImageTransform.h"
#include "ScalingTransform.h"

#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MatcherSolution.h"
#include "MatchImage.h"
#include "MatchMaker.h"

#include "Process.h"
#include "Progress.h"
#include "PvlFlatMap.h"
#include "PvlGroup.h"
#include "QDebugLogger.h"
#include "RobustMatcher.h"
#include "SerialNumber.h"
#include "ScharrTransform.h"
#include "SobelTransform.h"
#include "Statistics.h"
#include "SurfacePoint.h"
#include "TextFile.h"
#include "UserInterface.h"
#include "Pvl.h"

using namespace std;
namespace Isis{

  static void writeInfo(const QString &toname, Pvl &data, UserInterface &ui, Pvl *log) {
      if ( !toname.isEmpty() ) {
        FileName toinfo(toname);
        QString fname = toinfo.expanded();
        data.write(fname);
      }
      else {
        if ( !ui.IsInteractive()  ) {
          std::cout << data << "\n";
        }
        else {
          if (log){
            log->addObject(data);
          }
        }
      }
      return;
  }

  void findfeatures(UserInterface &ui, Pvl *log) {

    //  Program constants
    const QString findfeatures_program = "findfeatures";
    const QString findfeatures_version = "1.0";
    const QString findfeatures_revision = "$Revision$";

    // Get time for findfeatures_runtime
    time_t startTime = time(NULL);
    struct tm *tmbuf = localtime(&startTime);
    char timestr[80];
    strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
    const QString findfeatures_runtime = (QString) timestr;

    QString toinfo;
    if ( ui.WasEntered("TOINFO")) {
      toinfo = ui.GetAsString("TOINFO");
    }

    // Set up program debugging and logging
    QDebugStream logger( QDebugLogger::null() );
    bool p_debug( ui.GetBoolean("DEBUG") );
    if ( p_debug ) {
      // User specified a file by name
      if ( ui.WasEntered("DEBUGLOG") ) {
        logger = QDebugLogger::create( ui.GetAsString("DEBUGLOG") );
      }
      else {
        // User wants debugging but entered no file so give'em std::cout.
        logger = QDebugLogger::toStdOut();
      }
    }

    // Write out program logger information
    logger->dbugout() << "\n\n---------------------------------------------------\n";
    logger->dbugout() << "Program:        " << findfeatures_program << "\n";
    logger->dbugout() << "Version         " << findfeatures_version << "\n";
    logger->dbugout() << "Revision:       " << findfeatures_revision << "\n";
    logger->dbugout() << "RunTime:        " << findfeatures_runtime << "\n";
    logger->dbugout() << "OpenCV_Version: " << CV_VERSION << "\n";
    logger->flush();

    // Set up for info requests
    FeatureAlgorithmFactory *factory = FeatureAlgorithmFactory::getInstance();
    if ( ui.GetBoolean("LISTALL") ) {
      Pvl info;
      QStringList algorithms = factory->getListAll();
      // cout << algorithms.join("\n") << "\n";
      info.addObject( factory->info(algorithms) );
      writeInfo(toinfo, info, ui, log);
      return;
    }


    // Get parameters from user
    PvlFlatMap parameters;
    // Check for parameters file provided by user
    if ( ui.WasEntered("PARAMETERS") ) {
      QString pfilename = ui.GetAsString("PARAMETERS");
      Pvl pfile(pfilename);
      parameters.merge( PvlFlatMap(pfile) );
      parameters.add("ParameterFile", pfilename);
    }

    // Get individual parameters if provided
    QStringList parmlist;
    parmlist << "Ratio" << "EpiTolerance" << "EpiConfidence" << "HmgTolerance"
             << "MaxPoints" << "FastGeom" << "FastGeomPoints" << "GeomType"
             << "GeomSource" << "Filter";
    BOOST_FOREACH (QString p, parmlist ) {
      parameters.add(p, ui.GetAsString(p));
    }

    // Got all parameters.  Add them now and they don't need to be considered
    // from here on.  Parameters specified in input algorithm specs take
    // precedence (in MatchMaker)
    factory->setGlobalParameters(parameters);

    // Retrieve the ALGORITHM specification (if requested)
    QString aspec;
    if ( ui.WasEntered("ALGORITHM") ) {
      aspec = ui.GetString("ALGORITHM");
    }

    // Now check for file containing algorithms
    if ( ui.WasEntered("ALGOSPECFILE") ) {
      QString specfile = ui.GetAsString("ALGOSPECFILE");
      TextFile sFile(specfile);
      sFile.OpenChk(true);
      QString algorithm;
      QStringList algos;
      while ( sFile.GetLine(algorithm, true) ) {
        algos.append(algorithm);
      }

      if ( !aspec.isEmpty() ) { aspec.append("|");  }
      aspec.append( algos.join("|") );
    }


    // Create a list of algorithm specifications from user specs and log it
    // if requested
    RobustMatcherList algorithms = factory->create(aspec);
    if ( ui.GetBoolean("LISTSPEC") ) {
      Pvl info;
      info.addObject(factory->info(algorithms));
      writeInfo(toinfo, info, ui, log);

      // If no input files are provided exit here
      if ( ! ( ui.WasEntered("FROM") && ui.WasEntered("FROMLIST") ) ) {
        return;
      }
    }

    // First see what we can do about threads if your user is resource conscience
    int nCPUs = cv::getNumberOfCPUs();
    int nthreads = cv::getNumThreads();
    logger->dbugout() << "\nSystem Environment...\n";
    logger->dbugout() << "Number available CPUs:     " << nCPUs << "\n";
    logger->dbugout() << "Number default threads:    " << nthreads << "\n";

    // See if user wants to restrict the number of threads used
    int uthreads = nthreads;
    if ( ui.WasEntered("MAXTHREADS") ) {
      uthreads = ui.GetInteger("MAXTHREADS");
      if (uthreads < nthreads) cv::setNumThreads(uthreads);
      logger->dbugout() << "User restricted threads:   " << uthreads << "\n";
    }
    int total_threads = cv::getNumThreads();
    logger->dbugout() << "Total threads:             " << total_threads << "\n";
    logger->flush();


    //-------------Matching Business--------------------------
    // Make the matcher class
    MatchMaker matcher(ui.GetString("NETWORKID"));
    matcher.setDebugLogger(logger, p_debug );

    // Acquire query image
    matcher.setQueryImage(MatchImage(ImageSource(ui.GetAsString("MATCH"))));

    // Get the trainer images
    if ( ui.WasEntered("FROM") ) {
      matcher.addTrainImage(MatchImage(ImageSource(ui.GetAsString("FROM"))));
    }

    // If there is a list provided, get that too
    if ( ui.WasEntered("FROMLIST") ) {
      FileList trainers(ui.GetFileName("FROMLIST"));
      BOOST_FOREACH ( FileName tfile, trainers ) {
        matcher.addTrainImage(MatchImage(ImageSource(tfile.original())));
      }
    }

    // Got to have both file names provided at this point
    if ( matcher.size() <= 0 ) {
      throw IException(IException::User,
                       "Must provide both a FROM/FROMLIST and MATCH cube or image filename",
                       _FILEINFO_);
    }

    // Define which geometry source we should use.  None is the default
    QString geomsource = ui.GetString("GEOMSOURCE").toLower();
    if ( "match" == geomsource ) { matcher.setGeometrySourceFlag(MatchMaker::Query); }
    if ( "from"  == geomsource ) { matcher.setGeometrySourceFlag(MatchMaker::Train); }

    // Check for FASTGEOM option
    if ( ui.GetBoolean("FASTGEOM") ) {
      FastGeom geom( factory->globalParameters() );
      matcher.foreachPair( geom );
    }

    // Check for Sobel/Scharr filtering options for both Train and Images
    QString filter = factory->globalParameters().get("FILTER", "").toLower();
    // Apply the Sobel filter to all image
    if ( "sobel" == filter ) {
      matcher.query().addTransform(new SobelTransform("SobelTransform"));
      for (int i = 0 ; i < matcher.size() ; i++) {
        matcher.train(i).addTransform(new SobelTransform("SobelTransform"));
      }
    }

    // Add the Scharr filter to all images
    if ( "scharr" == filter ) {
      matcher.query().addTransform(new ScharrTransform("ScharrTransform"));
      for (int i = 0 ; i < matcher.size() ; i++) {
        matcher.train(i).addTransform(new ScharrTransform("ScharrTransform"));
      }
    }


    //  Apply all matcher/transform permutations
    logger->dbugout() << "\nTotal Algorithms to Run:     " << algorithms.size() << "\n";
    MatcherSolutionList matches = matcher.match(algorithms);
    const MatcherSolution *best = MatcherSolution::best(matches);
    logger->dbugout().flush();

    // If all failed, we're done
    if ( !best ) {
      logger->dbugout() << "Bummer! No matches were found!\n";
      QString mess = "NO MATCHES WERE FOUND!!!";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Not valid results
    if ( best->size() <= 0 ) {
      logger->dbugout() << "Shucks! Insufficient matches were found ("
                        << best->size() << ")\n";
      QString mess = "Shucks! Insufficient matches were found (" +
                     QString::number(best->size()) + ")";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Got some matches so lets process them
    Statistics quality = best->qualityStatistics();
    PvlGroup bestinfo("MatchSolution");
    bestinfo += PvlKeyword("Matcher", best->matcher()->name());
    bestinfo += PvlKeyword("MatchedPairs", toString(best->size()));
    bestinfo += PvlKeyword("Efficiency",  toString(quality.Average()));
    if ( quality.ValidPixels() > 1 ) {
      bestinfo += PvlKeyword("StdDevEfficiency",
                             toString(quality.StandardDeviation()));
    }

    if(log){
      log->addLogGroup(bestinfo);
    }


    // If a cnet file was entered, write the ControlNet file of the specified
    // type.  Note that it was created as an image-to-image network. Must make
    // adjustments if a ground network is requested.
    if ( ui.WasEntered("ONET") ) {
      ControlNet cnet;
      cnet.SetNetworkId(ui.GetString("NETWORKID"));
      cnet.SetUserName(Isis::Environment::userName());
      cnet.SetDescription(best->matcher()->name());
      cnet.SetCreatedDate(findfeatures_runtime);
      QString target = ( ui.WasEntered("TARGET") ) ? ui.GetString("TARGET") :
                                                     best->target();
      cnet.SetTarget( target );
      ID pointId( ui.GetString("POINTID"), ui.GetInteger("POINTINDEX") );
      matcher.setDebugOff();
      PvlGroup cnetinfo = matcher.network(cnet, *best, pointId);

      if ( cnet.GetNumPoints() <= 0 ) {
        QString mess = "No control points found!!";
        logger->dbugout() << mess << "\n";
        throw IException(IException::User, mess, _FILEINFO_);
      }

      // Umm..have to check this. Probably only makes sense with two images
      if ( ui.GetString("NETTYPE").toLower() == "ground" ) {
        cnetinfo += PvlKeyword("SpecialNetType", "Ground");
        for ( int i = 0 ; i < cnet.GetNumPoints() ; i++ ) {
          ControlPoint *point = cnet.GetPoint(i);
          point->SetType(ControlPoint::Fixed);
          point->Delete(matcher.query().id());
          point->SetRefMeasure(0);
        }
      }

      // Write out control network
      cnet.Write( ui.GetFileName("ONET") );
      if(log){
        log->addLogGroup(cnetinfo);
      }
    }

    // If user wants a list of matched images, write the list to the TOLIST filename
    if ( ui.WasEntered("TOLIST") ) {
      QLogger fout( QDebugLogger::create( ui.GetAsString("TOLIST"),
                                               (QIODevice::WriteOnly |
                                                QIODevice::Truncate) ) );
      fout.logger() << matcher.query().name() << "\n";
      MatcherSolution::MatchPairConstIterator mpair = best->begin();
      while ( mpair != best->end() ) {
        if ( mpair->size() > 0 ) {
           fout.logger() << mpair->train().source().name() << "\n";
        }
        ++mpair;
      }
    }


    // If user wants a list of failed matched images, write the list to the
    // TONOTMATCHED file in append mode (assuming other runs will accumulate
    // failed matches)
    if ( ui.WasEntered("TONOTMATCHED") ) {
      QLogger fout( QDebugLogger::create( ui.GetAsString("TONOTMATCHED"),
                                               (QIODevice::WriteOnly |
                                                QIODevice::Append) ) );
      MatcherSolution::MatchPairConstIterator mpair = best->begin();
      while ( mpair != best->end() ) {
        if ( mpair->size() == 0 ) {
           fout.logger() << mpair->train().source().name() << "\n";
        }
        ++mpair;
      }
    }

    return;
  }
}
