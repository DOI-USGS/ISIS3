
%Import QtCore/QtCoremod.sip
%Import QtXml/QtXmlmod.sip



%Exception Isis::IException(SIP_Exception) /PyName=IException/
{
%TypeHeaderCode
#include "IException.h"
%End
%RaiseCode
    const char *detail = sipExceptionRef.what();

    SIP_BLOCK_THREADS
    PyErr_SetString(sipException_Isis_IException, detail);
    SIP_UNBLOCK_THREADS
%End
};


%MappedType QVector<QSharedPointer<Isis::BundleControlPoint> >
{
  %TypeHeaderCode
    #include "BundleControlPoint.h"
    #include <QList>
  %End

  %ConvertFromTypeCode
    size_t size = sipCpp->size();
    PyObject *l = PyList_New(size);
    for (size_t i = 0; i < size; ++i) {

      Isis::BundleControlPoint* cppCPoint = sipCpp->at(i).data();

      PyObject *pyCPoint = sipConvertFromType((void*)(cppCPoint), sipType_Isis_BundleControlPoint, NULL);
      PyList_SetItem(l, i, pyCPoint);
    }

    return l;
  %End

  %ConvertToTypeCode
    Py_ssize_t size = PyList_Size(sipPy);
    QVector<QSharedPointer<Isis::BundleControlPoint> > *cppPointList = new QVector<QSharedPointer<Isis::BundleControlPoint> > ();
    int sipErr = 0;
    for(int i = 0; i < size; i++) {
      PyObject *pyCPoint = PyList_GET_ITEM(sipPy, i);
      Isis::BundleControlPoint *cppCPoint = (Isis::BundleControlPoint*)sipConvertToType(pyCPoint, sipType_Isis_BundleControlPoint, NULL, SIP_NOT_NONE, NULL, &sipErr);
      cppPointList->append(QSharedPointer<Isis::BundleControlPoint>(cppCPoint));
    }

    *sipCppPtr = cppPointList;
    return sipGetState(sipTransferObj);
  %End

};

%MappedType QList<Isis::Statistics>
{
  %TypeHeaderCode
  #include "Statistics.h"
  #include <QList>
  %End

  %ConvertFromTypeCode
    size_t size = sipCpp->size();
    PyObject *l = PyList_New(size);
    for (size_t i = 0; i < size; ++i) {
      Isis::Statistics* cppStats = new Isis::Statistics(sipCpp->at(i));
      PyObject *pyStats = sipConvertFromType((void*)(cppStats), sipType_Isis_Statistics, NULL);
      PyList_SetItem(l, i, pyStats);
    }

    return l;
  %End

  %ConvertToTypeCode
    Py_ssize_t size = PyList_Size(sipPy);
    QList<Isis::Statistics> *cppStatList = new QList<Isis::Statistics>();
    int sipErr = 0;
    for(int i = 0; i < size; i++) {
      PyObject *pyStat = PyList_GET_ITEM(sipPy, i);
      Isis::Statistics *stats = (Isis::Statistics*)sipConvertToType(pyStat, sipType_Isis_Statistics, NULL, SIP_NOT_NONE, NULL, &sipErr);
      cppStatList->append(*stats);
    }

    *sipCppPtr = cppStatList;
    return sipGetState(sipTransferObj);
  %End
};



%MappedType QList<Isis::BundleObservationSolveSettings>
{
  %TypeHeaderCode
  #include "BundleObservationSolveSettings.h"
  #include <QList>
  %End

  %ConvertFromTypeCode
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp->size())) == NULL)
        return NULL;

    for (int i = 0; i < sipCpp->size(); ++i) {
      Isis::BundleObservationSolveSettings *settings = new Isis::BundleObservationSolveSettings(sipCpp->at(i));

      PyObject *pySettings = sipConvertFromType((void*)(settings), sipType_Isis_BundleObservationSolveSettings, NULL);
      PyList_SetItem(l, i, pySettings);
    }

    return l;
  %End


  %ConvertToTypeCode
    if (!sipIsErr) {
        if (!PyList_Check(sipPy))
            return 0;

        for (int i = 0; i < PyList_GET_SIZE(sipPy); ++i)
            if (!sipCanConvertToType(PyList_GET_ITEM(sipPy, i),
                                     sipType_Isis_BundleObservationSolveSettings, SIP_NOT_NONE))
                return 0;

        return 1;
    }

    Py_ssize_t size = PyList_Size(sipPy);

    QList<Isis::BundleObservationSolveSettings> *cppSettingsList = new QList<Isis::BundleObservationSolveSettings>();
    int state;

    for(Py_ssize_t i = 0; i < size; i++) {
      PyObject *pySettings = PyList_GET_ITEM(sipPy, i);
      Isis::BundleObservationSolveSettings *settings;

      settings = (Isis::BundleObservationSolveSettings*)sipConvertToType(pySettings, sipType_Isis_BundleObservationSolveSettings, 0, SIP_NOT_NONE, &state, sipIsErr);

      if (*sipIsErr) {
          sipReleaseType(pySettings, sipType_Isis_BundleObservationSolveSettings, state);
          delete cppSettingsList;
          return 0;
      }

      cppSettingsList->append(*settings);
      sipReleaseType(pySettings, sipType_Isis_BundleObservationSolveSettings, state);
    }

    *sipCppPtr = cppSettingsList;
    return sipGetState(sipTransferObj);
  %End
};

// convert Bundlesettings wrapped in a QSharedPointer in Isis
%MappedType QSharedPointer<Isis::BundleImage>
{
  %TypeHeaderCode
  #include "BundleImage.h"
  #include <QSharedPointer>
  %End

  %ConvertFromTypeCode
    Isis::BundleImage *cpp = sipCpp->data();
    PyObject *py = sipConvertFromType(cpp, sipType_Isis_BundleImage, NULL);
    return py;
  %End

  %ConvertToTypeCode
    int sipErr = 0;
    QSharedPointer<Isis::BundleImage>* cpp_qsp = new QSharedPointer<Isis::BundleImage>();
    Isis::BundleImage *cpp = (Isis::BundleImage*)sipConvertToType(sipPy, sipType_Isis_BundleImage, NULL, SIP_NOT_NONE, NULL, &sipErr);
    cpp_qsp->reset(cpp);
    *sipCppPtr = cpp_qsp;

    return sipGetState(sipTransferObj);
  %End
};


// convert Bundlesettings wrapped in a QSharedPointer in Isis
%MappedType QSharedPointer<Isis::BundleSettings>
{
  %TypeHeaderCode
  #include "BundleSettings.h"
  #include <QSharedPointer>
  using namespace std;
  %End

  %ConvertFromTypeCode
    Isis::BundleSettings *settings = sipCpp->data();
    PyObject *pySettings = sipConvertFromType(settings, sipType_Isis_BundleSettings, NULL);
    return pySettings;
  %End

  %ConvertToTypeCode
    int sipErr = 0;
    QSharedPointer<Isis::BundleSettings>* wrappedSettings = new QSharedPointer<Isis::BundleSettings>();

    Isis::BundleSettings *settings = (BundleSettings*) sipConvertToType(sipPy, sipType_Isis_BundleSettings, NULL, SIP_NOT_NONE, NULL, &sipErr);
    wrappedSettings->reset(settings);
    *sipCppPtr = wrappedSettings;

    return sipGetState(sipTransferObj);
  %End
};


// convert Bundlesettings wrapped in a QSharedPointer in Isis
%MappedType QSharedPointer<Isis::ControlNet>
{
  %TypeHeaderCode
  #include "ControlNet.h"
  #include <QSharedPointer>
  using namespace std;
  %End

  %ConvertFromTypeCode
    PyObject *pySettings = sipConvertFromType(sipCpp, sipType_Isis_ControlNet, NULL);
    return pySettings;
  %End

  %ConvertToTypeCode
    int sipErr = 0;
    QSharedPointer<Isis::ControlNet> *qspControlNet = NULL;
    if (!sipCanConvertToType(sipPy, sipType_Isis_ControlNet, SIP_NOT_NONE)) {
        sipErr = 1;
    }
    else {
        Isis::ControlNet *cppControlNet = (Isis::ControlNet*)sipConvertToType(sipPy, sipType_Isis_ControlNet, NULL, SIP_NOT_NONE, NULL, &sipErr);
        qspControlNet->reset(cppControlNet);
    }

    *sipCppPtr = qspControlNet;
    return sipGetState(sipTransferObj);
  %End
};



// Convert a python str object to a std::string.
%MappedType std::string
{
  %TypeHeaderCode
  #include <string>
  %End

  %ConvertFromTypeCode
      // convert an std::string to a Python (unicode) string
      PyObject* newstring;
      newstring = PyUnicode_DecodeUTF8(sipCpp->c_str(), sipCpp->length(), NULL);
      if(newstring == NULL) {
          PyErr_Clear();
          newstring = PyBytes_FromString(sipCpp->c_str());
      }
      return newstring;
  %End

  %ConvertToTypeCode
      // Allow a Python string (or a unicode string) whenever a string is
      // expected.
      // If argument is a Unicode string, just decode it to UTF-8
      // If argument is a Python string, assume it's UTF-8
      if (sipIsErr == NULL)
          return (PyBytes_Check(sipPy) || PyUnicode_Check(sipPy));

      if (sipPy == Py_None)
      {
          *sipCppPtr = new std::string;
          return 1;
      }

      if (PyUnicode_Check(sipPy))
      {
          PyObject* s = PyUnicode_AsEncodedString(sipPy, "UTF-8", "");
          *sipCppPtr = new std::string(PyBytes_AS_STRING(s));
          Py_DECREF(s);
          return 1;
      }

      if (PyBytes_Check(sipPy))
      {
          *sipCppPtr = new std::string(PyBytes_AS_STRING(sipPy));
          return 1;
      }
      return 0;
  %End
};
