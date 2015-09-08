/****************************************************************************
** Meta object code from reading C++ file 'TargetBodyList.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TargetBodyList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TargetBodyList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Isis__TargetBodyList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Isis__TargetBodyList[] = {
    "Isis::TargetBodyList\0\0newCount\0"
    "countChanged(int)\0"
};

void Isis::TargetBodyList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TargetBodyList *_t = static_cast<TargetBodyList *>(_o);
        switch (_id) {
        case 0: _t->countChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Isis::TargetBodyList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Isis::TargetBodyList::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Isis__TargetBodyList,
      qt_meta_data_Isis__TargetBodyList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Isis::TargetBodyList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Isis::TargetBodyList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Isis::TargetBodyList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Isis__TargetBodyList))
        return static_cast<void*>(const_cast< TargetBodyList*>(this));
    if (!strcmp(_clname, "QList<TargetBodyQsp>"))
        return static_cast< QList<TargetBodyQsp>*>(const_cast< TargetBodyList*>(this));
    return QObject::qt_metacast(_clname);
}

int Isis::TargetBodyList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Isis::TargetBodyList::countChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
