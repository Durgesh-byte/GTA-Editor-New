/****************************************************************************
** Meta object code from reading C++ file 'maiaXmlRpcServerConnection.h'
**
** Created: Thu 4. Apr 10:43:34 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libmaia-master_modified/maiaXmlRpcServerConnection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maiaXmlRpcServerConnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MaiaXmlRpcServerConnection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      63,   28,   27,   27, 0x05,

 // slots: signature, parameters, type, tag, flags
     105,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MaiaXmlRpcServerConnection[] = {
    "MaiaXmlRpcServerConnection\0\0"
    "method,responseObject,responseSlot\0"
    "getMethod(QString,QObject**,const char**)\0"
    "readFromSocket()\0"
};

void MaiaXmlRpcServerConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MaiaXmlRpcServerConnection *_t = static_cast<MaiaXmlRpcServerConnection *>(_o);
        switch (_id) {
        case 0: _t->getMethod((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QObject**(*)>(_a[2])),(*reinterpret_cast< const char**(*)>(_a[3]))); break;
        case 1: _t->readFromSocket(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MaiaXmlRpcServerConnection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MaiaXmlRpcServerConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MaiaXmlRpcServerConnection,
      qt_meta_data_MaiaXmlRpcServerConnection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaiaXmlRpcServerConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaiaXmlRpcServerConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaiaXmlRpcServerConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaiaXmlRpcServerConnection))
        return static_cast<void*>(const_cast< MaiaXmlRpcServerConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int MaiaXmlRpcServerConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MaiaXmlRpcServerConnection::getMethod(QString _t1, QObject * * _t2, const char * * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
