%module(package="isispvl") PvlObject
%{
    #include "PvlKeyword.h"
    #include "PvlGroup.h"
    #include "PvlObject.h"
%}

%include <QVector>
%include <QString>

%include "PvlKeyword.h"
%include "PvlGroup.h"

%rename(getGroup) Isis::PvlObject::findGroup(const QString &name, Isis::PvlObject::FindOptions opt);

%include "PvlObject.h"