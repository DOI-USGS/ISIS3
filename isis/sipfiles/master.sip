%Module(name=libisispy,
        version=0,
        keyword_arguments="Optional")

%ExportedHeaderCode
  #include <glob.h>
  #include <iostream>
  #include <QLibrary>
  #include <QString>
  #include <QFileInfo>

  using namespace std;

  // Wraps glob to return a STD vector to avoid dealing with C style globbing
  inline vector<QString> glob(const QString& pat){
    glob_t glob_result;
    glob(pat.toLatin1().data(),GLOB_TILDE,NULL,&glob_result);
    vector<QString> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(QString(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return ret;
  }

  // Loads library files which match the given regex.
  // All files matching regex are piped into QLibrary and Loaded.
  //
  // If verbose = true, it wil print every file followed by OK or FAILED
  // which indicates a successful load. If unsuccessful, it also prints
  // an error message for context.
  inline void load_libs(const QString& pat, bool verbose=false) {
    vector<QString> libs = glob(pat);
    QLibrary lib_loader;

    for (vector<QString>::const_iterator i = libs.begin(); i != libs.end(); ++i) {
      // Strip extension
      QFileInfo file(*i);
      QString lib_dir = file.path() + "/" + file.completeBaseName();

      lib_loader.setFileName(lib_dir);

      // Load hint required for symbols to be accessible to other libraries loaded later
      lib_loader.setLoadHints(QLibrary::ExportExternalSymbolsHint);

      bool load_ok = lib_loader.load();

      // if verbose is true, print out load info
      if (verbose) {
        if (load_ok) {
          cout << "Loaded " + lib_dir.toStdString() + " : OK" << endl;
        }
        else if (!load_ok) {
          cout << "Loaded " + lib_dir.toStdString() + " : FAILED" << endl
               << lib_loader.errorString().toStdString() << endl;
        }
      }
    }
  }
%End

%Include type_conversions.sip

%Include ControlPoint.sip
%Include BundleMeasure.sip
%Include BundleControlPoint.sip
%Include BundleImage.sip
%Include ControlNet.sip
%Include Statistics.sip
%Include BundleResults.sip
%Include BundleSolutionInfo.sip
%Include BundleObservationSolveSettings.sip
%Include BundleSettings.sip
%Include BundleAdjust.sip
%Include MaximumLikelihoodWFunctions.sip
