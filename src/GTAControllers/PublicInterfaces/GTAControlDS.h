#ifndef GTACONTROLDS_H
#define GTACONTROLDS_H

#include "GTAControllers.h"
#include "GTAControlDS.h"


//enum EditorType{Header=0,Object,Procedure,Function,Sequence};

struct GTAControllers_SHARED_EXPORT structLoadFileInfo
{
    QStringList _icdFiles;
    QStringList _FileToUpdateList;
    QStringList _FileToRemoveList;
    bool        _isNew;
    bool        _execStatus;
    structLoadFileInfo():_isNew(false),_execStatus(false){}
};
struct GTAControllers_SHARED_EXPORT GTAStSearchInfo
{
    bool _bSearchUp;
    bool _bMatchCase;
    bool _bUseRegularExpression;
    bool _bSearchParameters;
    bool _bSearchCommandByName;
    bool _bSearchComment;
    GTAStSearchInfo():_bSearchUp(false),
                         _bMatchCase(false),
                         _bUseRegularExpression(false),
                         _bSearchParameters(false),
                         _bSearchCommandByName(false),
                         _bSearchComment(false){}
};
#endif
