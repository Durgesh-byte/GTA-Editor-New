#ifndef GTALOGFRAME_H
#define GTALOGFRAME_H

#include <QFrame>
#include "GTAUI.h"
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"
#include "GTALogAttachmentWidget.h"
#include "GTAFilterModel.h"
#include <QToolBar>
#include "GTAProgress.h"
#include "GTACompatibilityReport.h"
#include <QMenu>
#include <QAction>
#include <QPointer>

namespace Ui {
class GTALogFrame;
}

class GTAUI_SHARED_EXPORT GTALogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit GTALogFrame(GTAAppController *pController, QWidget *parent = 0);
    ~GTALogFrame();

    void clear();
    void setSavePath(const QString &path) { _LtraReportDocxSavePath = path; }
    void setCliMode(bool cliMode);
    void setLog(const QString &iLogFilePath);
    void setElementRelativePath(const QString &iElemRelativePath);
    void setElementType(const QString& iElementType);
    void analyzeResults();
    /**
       * This function gets the document name of the last document analyzed.
       * @return: Document name of the analyzed element
      */
    QString getLastElementAnalyzed()const;
    QString getLastElementAnalyzedPath() const;
    void callToExpandAll();
    void callToCollapseAll();
    void disableLogGroupBox();


public slots:
    bool exportReport(bool cliMode = false, const QString iSaveFormat = "DOCX");
    void onRowDoubleClick(const QModelIndex & iIndex);
    void selectAllCommands();
    void deselectAllCommands();
    void onSetFocus();

    //     void onGetLogModelFinished();

private slots:
    void onAnalyzePBClicked();
    void onPurposeAndConclusion_OkClicked();
    void onPurposeAndConclusion_CancelClicked();
    void onPurposeAndConclusion_TextEdited();
    void onPurposeAndConclusion_CheckBoxClicked();
    void resizeColumnToContents();
    void onBrowsePBClicked();
    void onSearchPBClicked();
    void onSearchWidgetOKClicked();
    void onAttachmentPBClicked();
    void onFilterPBClicked();
    void clearSearch();
    void cout(const QString& txt);
    void enableAnalyseButton(QString);
    void onExportLTRAFinished(std::shared_ptr<bool> output = nullptr);
    void onCollapsed(QModelIndex index);
    void onExpanded(QModelIndex index);
    void setAllComandsExpanded(bool );
    void setAllComandsCollapsed(bool );
    void onGetLastExecution();
    void showProgressBar(const QString &iMsg);
    void hideProgressBar();
    void onGetLogModelFinished(std::shared_ptr<LogModelOutput> output = nullptr);

private:
    ErrorMessageList createResultErrorLogs(QStringList &iList)const;
    /**
     * This function sets the title of result as relative path after the result analysis generated
     * @return: void
    */
    void setLogFrameWindowTitle();

    void messageWrn(const QString& message);
    void displayPurposeAndConclusion(const QString& docFile);

signals:
     void UpdateErrorLog(const ErrorMessageList &iErrorList);
private:


    //QAction _pExpandAll;
    //QAction _pCollapseAll;
    Ui::GTALogFrame *ui;

    GTAAppController * _pAppController;

    QPointer<GTAFilterModel> _pFilterModel;
    GTAGenSearchWidget * _pGenSearchWdgt;
    GTALogAttachmentWidget * _pLogAttachWdgt;

    GTAProgress* _pProgressBar;
    QString _exportedLTRAPath;
    bool _ModelDataChanged;

    bool _cliMode;
    QString _hasFailed;
    QString _logErrorMsg;
    QStringList _ColumnList;
    QStringList _attachmentList;
    QAbstractItemModel* _pValidationModel;
    GTAElement* _pLastAnalyzedElement;
    QHash<QString, QString> _ioLogInstanceIDMap;
    QString _LtraReportDocxSavePath;

    GTAElement* _currentAnalyzedElement;
    QString _purpose;
    QString _conclusion;
    bool _ignorePurposeAndConlusionUpdate;
};

#endif // GTALOGFRAME_H
