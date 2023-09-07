%module(package="astroset") UserInterface


%{
  #include "Gui.h"
  #include "Pvl.h"
  #include "UserInterface.h"
%}

%include <QVector>
%include <QString>

// Rename the constructors to Python-friendly names
%rename(UserInterfaceFromArgsv) UserInterface(const QString &xmlfile, int &argc, char *argv[]);
%rename(UserInterfaceFromList) UserInterface(const QString &xmlfile, QVector<QString> &args);

%rename(GetFileName) IsisAml::GetFileName(QString const &) const;
%rename(GetFileNameWithExt) IsisAml::GetFileName(QString const &, QString) const;
%rename(GetFileNames) IsisAml::GetFileName(QString const &, std::vector<QString> &) const;

%rename(GetAsString) IsisAml::GetAsString(QString const &) const;
%rename(GetAsStrings) IsisAml::GetAsString(QString const &, std::vector<QString> &) const;

%rename(GetString) IsisAml::GetString(QString const &) const;
%rename(GetStrings) IsisAml::GetString(QString const &, std::vector<QString> &) const;

%rename(GetCubeNameWithExt) IsisAml::GetCubeName(QString const &,QString) const;
%rename(GetCubeName) IsisAml::GetCubeName(QString const &) const;

%rename(GetInteger) IsisAml::GetInteger(const QString &) const;
%rename(GetIntegers) IsisAml::GetInteger(const QString &, std::vector<int> &) const;

%rename(GetDouble) IsisAml::GetDouble(const QString &) const;
%rename(GetDoubles) IsisAml::GetDouble(const QString &, std::vector<int> &) const;

%rename(GetBoolean) IsisAml::GetBoolean(const QString &) const;
%rename(GetBooleans) IsisAml::GetBoolean(const QString &, std::vector<bool> &) const;

%include "Pvl.h"
%include "IsisAml.h"
%include "UserInterface.h"

