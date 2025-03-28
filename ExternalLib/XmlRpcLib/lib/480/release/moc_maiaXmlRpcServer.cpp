/****************************************************************************
** Meta object code from reading C++ file 'maiaXmlRpcServer.h'
**
** Created: Fri 5. Apr 22:17:18 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libmaia-master_modified/maiaXmlRpcServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maiaXmlRpcServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MaiaXmlRpcServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      53,   18,   17,   17, 0x0a,
      95,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MaiaXmlRpcServer[] = {
    "MaiaXmlRpcServer\0\0method,responseObject,responseSlot\0"
    "getMethod(QString,QObject**,const char**)\0"
    "newConnection()\0"
};

void MaiaXmlRpcServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MaiaXmlRpcServer *_t = static_cast<MaiaXmlRpcServer *>(_o);
        switch (_id) {
        case 0: _t->getMethod((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QObject**(*)>(_a[2])),(*reinterpret_cast< const char**(*)>(_a[3]))); break;
        case 1: _t->newConnection(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MaiaXmlRpcServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MaiaXmlRpcServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MaiaXmlRpcServer,
      qt_meta_data_MaiaXmlRpcServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaiaXmlRpcServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaiaXmlRpcServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaiaXmlRpcServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaiaXmlRpcServer))
        return static_cast<void*>(const_cast< MaiaXmlRpcServer*>(this));
    return QObject::qt_metacast(_clname);
}

int MaiaXmlRpcServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
