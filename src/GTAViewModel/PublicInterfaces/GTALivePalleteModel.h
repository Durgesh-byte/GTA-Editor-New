#ifndef AINGTALIVEPALLETEMODEL_H
#define AINGTALIVEPALLETEMODEL_H

#include <AINGTAICDParameter.h>
#include "AINGTAViewModel.h"
#include "AINGTAUtil.h"

#pragma warning(push, 0)
#include <QAbstractTableModel>
#include <QPointer>
#include <QTimer>
#include <QtNetwork/QNetworkReply>
#pragma warning(pop)

class AINGTAViewModel_SHARED_EXPORT AINGTALivePalleteModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit AINGTALivePalleteModel(QObject *parent = 0);
    ~AINGTALivePalleteModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &) const override;

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool appendParamToList(const QList<AINGTAICDParameter> iICDParam, const QMap<QString, QString> &iGetParamInfo = QMap<QString, QString>());

    /**
      * Author: Saurav
      * @brief: clears the parameter list.
      *         This function is called in the event of a live view close or unsubscribe all
      */
    void clearParamList();

    void clearSelectedRows(const QStringList &iSelectedParams);

    /**
      * Author: Saurav
      * @brief: stops the timer that sends client commands.
      */
    void stopForcing();

    /**
      * Author: Saurav
      * @brief: returns the list of acquisition parameters.
      */
    QList<AINGTAICDParameter> getParamList() const;

private:
    QList<AINGTAICDParameter> _getParamList;
    QStringList _ColumnList;
    QPointer<QTimer> _pTimer;

private slots:
    void timerTimeout();

public slots:
    void onGetMultiParamFinished(QVariant msg);
    void onUpdateParamRangeReceived(QString paramName,QString minValue, QString maxValue);

signals:
    /**
      * Author: Saurav
      * @brief: sends an xmlrpc client call to client that connects to BISG server.
      * @iArgs: arguments to send to bisg server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @iResponseObj: object of the class to which the response needs to be connected
      * @iResponseSlotName: slot of the class to which the response needs to be connected
      */
    bool sendBenchRequest(QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName);
};

#endif // AINGTALIVEPALLETEMODEL_H
