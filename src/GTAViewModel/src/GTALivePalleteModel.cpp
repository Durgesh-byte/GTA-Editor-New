#include "AINGTALivePalleteModel.h"

#pragma warning(push, 0)
#include <QDebug>
#include <QTime>
#include <QColor>
#pragma warning(pop)


AINGTALivePalleteModel::AINGTALivePalleteModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    _pTimer = new QTimer(this);
    _pTimer->setInterval(500);
    connect(_pTimer,SIGNAL(timeout()) , this, SLOT(timerTimeout()));
    _ColumnList<<"Parameter"<<"Last Updated Value"<<"Last Updated Time"<<"PARAM_VALUES";
}

AINGTALivePalleteModel::~AINGTALivePalleteModel()
{
    if (nullptr != _pTimer)
    {
        _pTimer->stop();
        delete _pTimer;
        _pTimer = nullptr;
    }
}

QVariant AINGTALivePalleteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant oVal = QVariant();
    if (role==Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            int column = 0;
            QString colName = _ColumnList.value(section+column);
            if(! colName.isEmpty())
                oVal = QVariant(colName);

        }
        if (orientation == Qt::Vertical)
            oVal=QVariant(QString::number(section));
    }
    return oVal;
}

QModelIndex AINGTALivePalleteModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column, nullptr);
}

QModelIndex AINGTALivePalleteModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int AINGTALivePalleteModel::rowCount(const QModelIndex &) const
{
    return _getParamList.count();
}

int AINGTALivePalleteModel::columnCount(const QModelIndex &) const
{

    return _ColumnList.count();
}

QVariant AINGTALivePalleteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int row = index.row();
    int col = index.column();

    switch(role)
    {
    case Qt::DisplayRole:
        if(col == 0)
        {
            return QString((_getParamList.at(row).getName()));
        }
        else if(col == 1)
        {
            return QString("%1·%2").arg(_getParamList.at(row).getValue(),_getParamList.at(row).getSSMValue());
        }
        else if(col == 2)
        {
            return QString((_getParamList.at(row).getLastModifiedDate()));
        }
        else if(col ==3)
        {
            return QString("[Min:%1],[Max:%2]").arg(_getParamList.at(row).getMinValue(), _getParamList.at(row).getMaxValue());
        }
    case Qt::BackgroundRole:
        if(_getParamList.at(row).getValue().toInt() < _getParamList.at(row).getMinValue().toInt() || _getParamList.at(row).getValue().toInt() > _getParamList.at(row).getMaxValue().toInt())
        {
            return QColor(251,130,89);
        }
    }
    return QVariant();
}

bool AINGTALivePalleteModel::appendParamToList(const QList<AINGTAICDParameter> iICDParamList, const QMap<QString, QString> &iGetParamInfo)
{
    foreach(AINGTAICDParameter param, iICDParamList)
    {
        if(_getParamList.contains(param))
        {
            for(int i = 0 ; i< _getParamList.count(); i++)
            {
                if(!iGetParamInfo.isEmpty())
                {
                    if(iGetParamInfo.contains(_getParamList[i].getName()))
                    {
                        _getParamList[i].setMinValue(iGetParamInfo.value(param.getTempParamName()).split(",").first());
                        _getParamList[i].setMaxValue(iGetParamInfo.value(param.getTempParamName()).split(",").last());
                    }
                }
            }
        }
        else
        {
            if(!iGetParamInfo.isEmpty())
            {
                if(iGetParamInfo.contains(param.getTempParamName()))
                {
                    QStringList rangeValue = iGetParamInfo.value(param.getTempParamName()).split(",");
                    param.setMinValue(rangeValue.first());
                    param.setMaxValue(rangeValue.last());
                }
            }
            _getParamList.append(param);
        }
    }
    if(_getParamList.count() >= 1 && !_pTimer->isActive())
        _pTimer->start();
    return true;
}

/**
  * Author: Saurav
  * @brief: clears the parameter list.
  *         This function is called in the event of a live view close or unsubscribe all
  */
void AINGTALivePalleteModel::clearParamList()
{
    _getParamList.clear();
}

QList<AINGTAICDParameter> AINGTALivePalleteModel::getParamList() const
{
    return _getParamList;
}

/**
  * Author: Saurav
  * @brief: stops the timer that sends client commands.
  */
void AINGTALivePalleteModel::stopForcing()
{
    if (nullptr != _pTimer)
        _pTimer->stop();
}

void AINGTALivePalleteModel::timerTimeout()
{
    //send client requests
    if(_getParamList.count()>0)
    {
        QVariantList args;
        QVariantList ags; // intermediate Qvariantlist for BISG
        foreach(AINGTAICDParameter parameter, _getParamList)
        {
            ags<<QVariant(parameter.getName());
        }
        args<<QVariant(ags);
        emit sendBenchRequest(args,GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE,nullptr,nullptr);
    }
}

void AINGTALivePalleteModel::onGetMultiParamFinished(QVariant msg)
{
    if (msg.canConvert<QVariantMap>())
    {
        QVariantMap map = msg.toMap();
        for(int i=0 ; i<_getParamList.count() ; i++)
        {
            AINGTAICDParameter parameter = _getParamList[i];
            QVariantMap paramMap = map.value(parameter.getName()).toMap();
            QString updatedVal = paramMap.value("Value").toString();
            _getParamList[i].setValue(updatedVal);

            QString FS = paramMap.value("Status").toString();
            if (FS.isEmpty())
            {
                QString SSM = paramMap.value("Sdi").toString();
                if (!SSM.trimmed().isEmpty())
                {
//                    _getParamList[i].setValue(updatedVal+"·"+SSM);
                    _getParamList[i].setSSMValue(SSM);
                }
            }
            else
            {
                _getParamList[i].setSSMValue(FS);
            }
            QString epochTime = paramMap.value("Timestamp").toString();;
            if(epochTime.contains("."))
                epochTime.replace(".","");
            if(epochTime.length() == 12)
            {
                epochTime.append("0");
            }
            qint64 i64Time = epochTime.toLongLong();
            QDateTime execTime = QDateTime::fromMSecsSinceEpoch(i64Time);
            _getParamList[i].setLastModifiedDate(execTime.toString());
            emit dataChanged(createIndex(i,1),createIndex(i,2), {Qt::DisplayRole});


        }
    }

}

void AINGTALivePalleteModel::clearSelectedRows(const QStringList &iSelectedParams)
{
    foreach (QString paramName, iSelectedParams)
    {
        for(int i=0; i<_getParamList.count() ; i++)
        {
            if(_getParamList.at(i).getName().compare(paramName) == 0)
            {
                _getParamList.removeAt(i);
                break;
            }
        }
    }
    //DEV COMMENT: UnSubscription part is pending.
}

void AINGTALivePalleteModel::onUpdateParamRangeReceived(QString paramName, QString minValue, QString maxValue)
{
    for(int i =0; i < _getParamList.count(); i++)
    {
        if(_getParamList.at(i).getName() == paramName)
        {
          _getParamList[i].setMinValue(minValue);
          _getParamList[i].setMaxValue(maxValue);
          break;
        }
    }
}
