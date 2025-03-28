#pragma warning (push, 0)
#include "GTAHeaderWidget.h"
#include "ui_GTAHeaderWidget.h"
#include <QMessageBox>
#pragma warning (pop)

GTAHeaderWidget::GTAHeaderWidget(GTAEditorWindow *parent, QDialog *dParent) :
    QDialog(dParent),
    ui(new Ui::GTAHeaderWidget)
{
    _parent = parent;
    ui->setupUi(this);
    ui->HeaderTV->setModel(NULL);

    _pHdrFieldEditDlg = new GTAHeaderFieldEdit();
    _pHdrFieldEditDlg->hide();
    _pFileSystemHeader = NULL;

    connect(ui->addHdrItemPB,SIGNAL(clicked()),this,SLOT(onAddHeaderItem()));
    connect(ui->headerFieldEditPB,SIGNAL(clicked()),this,SLOT(onEditHeaderField()));
    connect(ui->headerFieldRemovePB,SIGNAL(clicked()),this,SLOT(onDeleteHeaderField()));
    connect(ui->movehdrFieldDownPB,SIGNAL(clicked()),this,SLOT(onMoveHdrRowDown()));
    connect(ui->movehdrFieldUpPB,SIGNAL(clicked()),this,SLOT(onMoveHdrRowUp()));
    connect(ui->HeaderCB,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onLoadHeaderDoc(const QString &)));
    connect(ui->saveHeaderPB,SIGNAL(clicked()),this,SLOT(onSaveHeaderDoc()));
    connect(ui->applyChangesHeaderPB, SIGNAL(clicked()), this, SLOT(onApplyChangesHeader()));
    connect(ui->NewHeaderPB,SIGNAL(clicked()),this,SLOT(onNewHeaderDoc()));
    connect(_pHdrFieldEditDlg,SIGNAL(signalAddField()),this,SLOT (startAddingHeaderItem()));
    connect(_pHdrFieldEditDlg,SIGNAL(signalEditField()),this,SLOT (startEditingHeaderItem()));
    connect(ui->getICDCkB,SIGNAL(toggled(bool)),this,SLOT(onShowICDDetails(bool)));

    ui->HeaderTV->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->HeaderTV, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(onCustomMenuRequested(const QPoint&)));

    GTAAppController *pAppController = GTAAppController::getGTAAppController();
    if(pAppController)
    {
        QString dataDir = pAppController->getGTADataDirectory();

        QStringList headerFilter;
        headerFilter<<"*.hdr";
        if(_pFileSystemHeader == NULL)
            _pFileSystemHeader = new QFileSystemModel(this);

        _pFileSystemHeader->setRootPath(dataDir);
        _pFileSystemHeader->setNameFilters(headerFilter);
        _pFileSystemHeader->setNameFilterDisables(false);
        _pFileSystemHeader->setFilter(QDir::Files);
        ui->HeaderCB->setModel(_pFileSystemHeader);
        ui->HeaderCB->setRootModelIndex(_pFileSystemHeader->index(dataDir));
        ui->HeaderCB->setCurrentIndex(-1);
        ui->WarningApplyChangesHeader->setStyleSheet("font-weight: bold; color: red");

    }

}

GTAHeaderWidget::~GTAHeaderWidget()
{
    if(_pHdrFieldEditDlg != NULL)
    {
        delete _pHdrFieldEditDlg;
        _pHdrFieldEditDlg = NULL;
    }
     if(_pFileSystemHeader != NULL )
     {
         delete _pFileSystemHeader;
         _pFileSystemHeader = NULL;
     }
    delete ui;
}

QTableView* GTAHeaderWidget::getHeaderTableView()
{
    return ui->HeaderTV;
}

void GTAHeaderWidget::setWindowTitle(const QString &iTitle)
{
    QDialog::setWindowTitle(iTitle);
}

void GTAHeaderWidget::setModel(QAbstractItemModel * iModel)
{
    if(iModel)
    {
        ui->HeaderTV->setModel(iModel);
    }
    else
    {
        ui->HeaderTV->setModel(NULL);
    }
}

void GTAHeaderWidget::setModelForCurrentDoc(QAbstractItemModel* pHeaderModel)
{
    if (NULL != pHeaderModel)
    {
        const int headerTVColumnWidth = (this->width() * 35) / 100; // dividing into 3 equal parts
        const int exportColumnWidth = (this->width() * 15) / 100;
        ui->HeaderCB->setToolTip(QString("Current header is pertaining to a loaded document"));
        ui->HeaderTV->setModel(pHeaderModel);
        ui->HeaderTV->setColumnWidth(0, headerTVColumnWidth);
        ui->HeaderTV->setColumnWidth(1, headerTVColumnWidth);
        ui->HeaderTV->setColumnWidth(2, exportColumnWidth);
        ui->HeaderTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

        ui->HeaderTV->horizontalHeader()->setStretchLastSection(true);
        ui->HeaderTV->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->HeaderTV->setSelectionBehavior(QAbstractItemView::SelectRows);
        silentDisableICDChkButton();
        ui->HeaderCB->setCurrentIndex(-1);

        connect(ui->HeaderTV->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(onHeaderItemSelectionChanged(const QItemSelection&, const QItemSelection&)));
    }
}

/**
 * @brief SLOT on right click performed on the GTAHeader Dialog.
 * This will open a context menu to Select/Unselect all header fields for LTRA Export
 * Then it will emit a SIGNAL that would be received by GTAHeaderWidget
 */
void GTAHeaderWidget::onCustomMenuRequested(const QPoint&) {

    QItemSelectionModel* model = ui->HeaderTV->selectionModel();
    if (model)
    {
        QAction SelectAllToExtract("Select All for LTRA export", ui->HeaderTV);
        connect(&SelectAllToExtract, SIGNAL(triggered()), this, SLOT(onSelectAllToExtract()));
        QAction UnSelectAllToExtract("UnSelect All for LTRA export", ui->HeaderTV);
        connect(&UnSelectAllToExtract, SIGNAL(triggered()), this, SLOT(onUnSelectAllToExtract()));

        QMenu menu(ui->HeaderTV);
        menu.addAction(&SelectAllToExtract);
        menu.addAction(&UnSelectAllToExtract);
        menu.exec(QCursor::pos());
    }
}

/**
 * @brief SLOT on Context menu item selection for Select All.
 * This will select all the header fields for export in the LTRA report. 
 * Then it will emit a SIGNAL that would be received by GTAHeaderWidget
 */
void GTAHeaderWidget::onSelectAllToExtract()
{
    GTAHeaderTableModel* pCurrentModel = (GTAHeaderTableModel*)ui->HeaderTV->model();
    pCurrentModel->SelectAllForExport();
}

/**
 * @brief SLOT on Context menu item selection for UnSelect All.
 * This will unselect all the non-mandatory header fields for export in the LTRA report.
 * Then it will emit a SIGNAL that would be received by GTAHeaderWidget
 */
void GTAHeaderWidget::onUnSelectAllToExtract()
{
    GTAHeaderTableModel* pCurrentModel = (GTAHeaderTableModel*)ui->HeaderTV->model();
    pCurrentModel->UnSelectAllForExport();
}

void GTAHeaderWidget::onAddHeaderItem()
{
    _pHdrFieldEditDlg->setMode(GTAHeaderFieldEdit::mode::ADDMODE);
    _pHdrFieldEditDlg->clear();
    _pHdrFieldEditDlg->show();
}

void GTAHeaderWidget::startAddingHeaderItem()
{
    QString sDesc =_pHdrFieldEditDlg->getFieldDescription();
    QString sName = _pHdrFieldEditDlg->getFieldName();
    QString sVal  = _pHdrFieldEditDlg->getFieldVal();
    QString sMandatory = _pHdrFieldEditDlg->getIsMandatory();

    QString sValid = sName;
    sValid.remove(" ");
    if (sValid.isEmpty())
    {
        QMessageBox::critical(this,"Empty field","Field Name requires to be filled mandatorily");
    }
    else
    {
        emit headerFieldAdded(sName,sVal,sMandatory,sDesc);
    }

}

void GTAHeaderWidget::onHeaderFieldUpdatedInModel(bool val, const QString &iErrorMsg)
{
    if (!val)
        QMessageBox::critical(this,"Error",iErrorMsg);
    else
    {
        _pHdrFieldEditDlg->hide();
        _pHdrFieldEditDlg->clear();
    }
}

void GTAHeaderWidget::onEditHeaderField()
{
    // bool rC = false;
    if(_parent)
    {
        QItemSelectionModel* pSelectedModel =  ui->HeaderTV->selectionModel();
        if (NULL!=pSelectedModel)
        {
            QModelIndexList selectedIndexes =pSelectedModel->selectedIndexes();
            if (!selectedIndexes.isEmpty())
            {
                QModelIndex firstSelection = selectedIndexes.at(0);
                if (firstSelection.isValid())
                {
                    int row = firstSelection.row();

                    if (row!=0 && _parent->isHeaderRowEditable(row))
                    {
                        _pHdrFieldEditDlg->setMode(GTAHeaderFieldEdit::mode::EDITMODE);
                        QString sName, sVal, sMandatory, sDesc;
                        bool sShowInLTRA;
                        if(_parent->getHeaderFieldInfo(row,sName,sVal,sMandatory,sDesc, sShowInLTRA))
                        {
                            _pHdrFieldEditDlg->setHeaderInfo(sName,sVal,sMandatory,sDesc, sShowInLTRA);
                            _pHdrFieldEditDlg->show();
                        }
                        else
                            QMessageBox::critical(this,"Header Info Retrieve Error",_parent->getLastError());

                    }
                    else
                    {
                        QString errorMsg;
                        if (!row)
                            errorMsg="Header name edit is locked, Only field value can be edited";
                        else
                            errorMsg = _parent->getLastError();
                        QMessageBox::critical(this,"Uneditable Header Field",errorMsg);
                    }


                }
            }
            else
                QMessageBox::information(this,"Nothing to edit","please select a header item for editing");
        }
        else
            QMessageBox::information(this,"No header found","No header found for editing");
    }
}

void GTAHeaderWidget::startEditingHeaderItem()
{
    QString sDesc =_pHdrFieldEditDlg->getFieldDescription();
    QString sName = _pHdrFieldEditDlg->getFieldName();
    QString sVal  = _pHdrFieldEditDlg->getFieldVal();
    QString sMandatory = _pHdrFieldEditDlg->getIsMandatory();
    bool sShowInLTRA = _pHdrFieldEditDlg->getShowInLTRA();

    QString sValid = sName;
    sValid.remove(" ");
    if (sValid.isEmpty())
    {
        QMessageBox::critical(this,"Empty field","Field Name requires to be filled mandatorily");
    }
    else
    {
        if (_parent)
        {
            QItemSelectionModel* pSelectedModel =  ui->HeaderTV->selectionModel();
            if (NULL!=pSelectedModel)
            {
                QModelIndexList selectedIndexes = pSelectedModel->selectedIndexes();
                if (!selectedIndexes.isEmpty())
                {
                    QModelIndex firstSelection = selectedIndexes.at(0);
                    if (firstSelection.isValid())
                    {
                        int row = firstSelection.row();
                        bool status = _parent->editHeaderField(row,sName,sVal,sMandatory,sDesc, sShowInLTRA);
                        if (!status)
                            QMessageBox::critical(this,"Error",_parent->getLastError());
                        else
                        {
                            _pHdrFieldEditDlg->hide();
                            _pHdrFieldEditDlg->clear();
                        }
                    }
                }
            }

        }
    }
}

void GTAHeaderWidget::onDeleteHeaderField()
{
    //bool rC = false;

    if(_parent)
    {
        QItemSelectionModel* pSelectedModel =  ui->HeaderTV->selectionModel();
        if (NULL!=pSelectedModel)
        {
            QModelIndexList selectedIndexes = pSelectedModel->selectedIndexes();
            if (!selectedIndexes.isEmpty())
            {
                QModelIndex firstSelection = selectedIndexes.at(0);
                if (firstSelection.isValid())
                {
                    int row = firstSelection.row();
                    if (row!=0 && _parent->isHeaderRowDeletable(row))
                    {
                        _parent->removeHeaderItem(row);
                    }
                    else
                    {
                        QString errorMsg;
                        if (!row)
                            errorMsg="Header name edit is locked, Only field value can be edited";
                        else
                            errorMsg = _parent->getLastError();
                        QMessageBox::critical(this,"Uneditable Header Field",errorMsg);
                    }

                }
            }
            else
                QMessageBox::information(this,"Nothing to edit","please select a header item for editing");
        }
        else
            QMessageBox::information(this,"No header found","No header found for editing");
    }
}

void GTAHeaderWidget::onMoveHdrRowUp()
{
    bool rC = false;
    if(_parent)
    {
        QItemSelectionModel* pSelectedModel =  ui->HeaderTV->selectionModel();
        if (NULL!=pSelectedModel)
        {
            QModelIndexList selectedIndexes =pSelectedModel->selectedIndexes();
            if (!selectedIndexes.isEmpty())
            {
                QModelIndex firstSelection = selectedIndexes.at(0);
                if (firstSelection.isValid())
                {
                    int row = firstSelection.row();

                    if (row!=0 && _parent->isHeaderRowEditable(row))
                    {

                        rC = _parent->moveHeaderRowItemUp(row);
                        if (rC)
                            ui->HeaderTV->selectRow(--row);

                    }
                    else
                    {
                        QString errorMsg;
                        if (!row)
                            errorMsg="Header name edit is locked, Only field value can be edited";
                        else
                            errorMsg = _parent->getLastError();
                        QMessageBox::critical(this,"Uneditable Header Field",errorMsg);
                    }


                }
            }
            else
                QMessageBox::information(this,"Nothing to edit","please select a header item for editing");
        }
        else
            QMessageBox::information(this,"No header found","No header found for editing");

    }
}

void GTAHeaderWidget::onMoveHdrRowDown()
{
    bool rC = false;
    if(_parent)
    {
        QItemSelectionModel* pSelectedModel =  ui->HeaderTV->selectionModel();
        if (NULL != pSelectedModel)
        {
            QModelIndexList selectedIndexes = pSelectedModel->selectedIndexes();
            if (!selectedIndexes.isEmpty())
            {
                QModelIndex firstSelection = selectedIndexes.at(0);
                if (firstSelection.isValid())
                {
                    int row = firstSelection.row();

                    if (row!=0 && _parent->isHeaderRowEditable(row))
                    {

                        rC = _parent->moveHeaderRowItemDown(row);
                        if (rC)
                            ui->HeaderTV->selectRow(++row);

                    }
                    else
                    {
                        QString errorMsg;
                        if (!row)
                            errorMsg = "Header name edit is locked, Only field value can be edited";
                        else
                            errorMsg = _parent->getLastError();
                        QMessageBox::critical(this,"Uneditable Header Field",errorMsg);
                    }


                }
            }
            else
                QMessageBox::information(this,"Nothing to edit","please select a header item for editing");
        }
        else
            QMessageBox::information(this,"No header found","No header found for editing");

    }
}

bool GTAHeaderWidget::onLoadHeaderDoc(const QString & isHdrName)
{

    bool rC = false;
    QString headerName=(QString) isHdrName;
    if (_parent && isHdrName.isEmpty()==false)
    {
        if (isHdrName.contains(".hdr"))
        {
            headerName.remove(".hdr");
        }
        rC = _parent->headerExists(headerName);
        if (rC)
        {

            QAbstractItemModel* pHeaderModel = NULL;
            
            _parent->createHeaderModelFromDoc(isHdrName,pHeaderModel);
            ui->HeaderCB->setToolTip("Current header is not pertaining to any editor document");
            if (NULL!=pHeaderModel)

            {
                ui->HeaderTV->setModel(pHeaderModel);
                ui->HeaderTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

                ui->HeaderTV->horizontalHeader()->setStretchLastSection(true);
                ui->HeaderTV->setSelectionMode(QAbstractItemView::SingleSelection);
                ui->HeaderTV->setSelectionBehavior(QAbstractItemView::SelectRows);
                ui->saveHeaderPB->setDisabled(false);
            }

        }

    }
    else
        ui->saveHeaderPB->setDisabled(false);
    return rC;
}

/**
 * @brief SLOT on changes applied to the header of a procedure. 
 * This will save the changes in a temporary hdr and also save the updated procedure
 * Then it will emit a SIGNAL that would be received by GTAEditorWindow
 */
void GTAHeaderWidget::onApplyChangesHeader() 
{    
    if (_parent) 
    {
        QString HeaderNameSelected; // CurrentHeaderName
        QString HeaderNameInElement;// PreviousHeaderName
        ui->HeaderTV->selectRow(0);
        if (ui->HeaderCB->currentIndex() >= 0)
        {
            HeaderNameSelected = ui->HeaderCB->currentText(); // CurrentHeaderName
        }
        HeaderNameInElement = _parent->getHeaderName();
        if (HeaderNameSelected != HeaderNameInElement)
            HeaderNameSelected = HeaderNameInElement;

        HeaderNameSelected = HeaderNameSelected + "_tempo";

        if (!_parent->saveHeader(HeaderNameSelected))
            return;
                
        _parent->onSave();
        if (_parent->headerExists(HeaderNameSelected))
            _parent->deleteHeader(HeaderNameSelected);
    }
    emit headerChanged();
    close();
}

bool GTAHeaderWidget::onSaveHeaderDoc()
{
    bool rC = false;
    if (_parent)
    {
        QString headerName;
        QString headerName1;
        ui->HeaderTV->selectRow(0);
        if (ui->HeaderCB->currentIndex()>=0)
        {
            headerName = ui->HeaderCB->currentText();
        }
        headerName1 =_parent->getHeaderName();

        if(headerName != headerName1)
            headerName = headerName1;

        if (headerName.isEmpty())
        {
            QMessageBox::critical(this,"Error","Header Name cannot be empty");
            return rC;
        }

        if(headerName != ELEM_TMPL)
        {
            if (_parent->headerExists(headerName))
            {
                QMessageBox msgBox (this);
                QString text("Header '");text.append(headerName);
                text.append("' already exists, \nOverwrite the existing header?");
                msgBox.setText(text);
                //msgBox.setIcon(QIcon(":/images/forms/img/dialog_warning.png"));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setWindowTitle("Save Header Warning");
                msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();
                if(ret==QMessageBox::Yes)
                {
                    QStringList emptyManFields;
                    bool status = _parent->headerComplete(emptyManFields);
                    if (!status)
                    {
                        QMessageBox::critical(this,"Error",QString("Following Mandatory fields are empty:\n-----------------------------------------\n\n%1").arg(emptyManFields.join("\n")));
                        return rC;
                    }
                    else
                        rC =_parent->saveHeader(headerName);
                }
                else
                    return rC;;
            }

            QStringList emptyManFields;
            bool status = _parent->headerComplete(emptyManFields);
            if (!status)
            {
                QMessageBox::critical(this,"Error",QString("Following Mandatory fields are empty:\n-----------------------------------------\n\n%1").arg(emptyManFields.join("\n")));
            }
            else
                rC= _parent->saveHeader(headerName);

            if (rC)
            {
                QString headerFileName = QString("%1.hdr").arg(headerName);
                int index = ui->HeaderCB->findText(headerFileName);
                if (index>=0)
                {
                    ui->HeaderCB->setCurrentIndex(index);
                }
            }
        }
        else
        {
            QMessageBox::warning(this, tr("WARNING"),
                        tr("The file cannot be saved.\nPlease to modify the Header filename."),
                        QMessageBox::Ok, QMessageBox::Ok);
        }
    }
    return rC;
}

void GTAHeaderWidget::onNewHeaderDoc()
{
    if (_parent)
    {
        QMessageBox msgBox (this);
        msgBox.setText("This action would flush the current Header data and load header from template.\nWant to continue?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Header Warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)

        {
            disconnect(ui->HeaderCB,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onLoadHeaderDoc(const QString &)) );
            ui->HeaderCB->setCurrentIndex(-1);
            _parent->setInitialHeaderView(ui->HeaderTV);
            connect(ui->HeaderCB,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onLoadHeaderDoc(const QString &)) );
        }
    }
}

void GTAHeaderWidget::onShowICDDetails(bool status)
{
	QString temp = ui->HeaderCB->currentText();
    if (!ui->HeaderCB->currentText().isEmpty() && status==true)
    {
        QMessageBox::critical(this,"Error","Current header displayed is not that of document, please load document to view document header details");
        silentDisableICDChkButton();
        return;
    }
    QString sErrorMsg;
    bool rc = false;
    if (_parent)
    {
        rc = _parent->showICDDetails(status,sErrorMsg);
    }

    if (!rc)
    {
        QMessageBox::critical(this,"Error",sErrorMsg);
        silentDisableICDChkButton();
    }
}

/**
 * @brief SLOT on item selection changed.
 * This will capture the item selection change event and disable the remove button.
 * Then it will emit a SIGNAL that would be received by GTAHeaderDialog
 */
void GTAHeaderWidget::onHeaderItemSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    QItemSelectionModel* pSelectedModel = ui->HeaderTV->selectionModel();
    if (NULL != pSelectedModel)
    {
        QModelIndexList selectedIndexes = pSelectedModel->selectedIndexes();
        if (!selectedIndexes.isEmpty())
        {
            QModelIndex firstSelection = selectedIndexes.at(0);
            if (firstSelection.isValid())
            {
                int row = firstSelection.row();
                bool isMandatory = false;

                _parent->isHeaderFieldMandatory(row, isMandatory);
                bool isDefaultMandatoryHeaderField = _parent->isHeaderFieldDefaultMandatory(row);

                ui->headerFieldRemovePB->setDisabled(isMandatory || isDefaultMandatoryHeaderField);
            }
        }
    }
}

void GTAHeaderWidget::silentDisableICDChkButton()
{
    disconnect(ui->getICDCkB,SIGNAL(toggled(bool)),this,SLOT(onShowICDDetails(bool)));
    ui->getICDCkB->setChecked(false);
    connect(ui->getICDCkB,SIGNAL(toggled(bool)),this,SLOT(onShowICDDetails(bool)));

}

/**
 * @brief Highlights the specified row in the header table view.
 *
 * This function takes a row number and highlights the corresponding row in the header table view
 * with a green background to indicate selection or focus.
 *
 * @param row The row number of the header item to be highlighted.
 */
void GTAHeaderWidget::highlightHeaderTableRow(int row)
{
    QAbstractItemModel* model = ui->HeaderTV->model();
    if (!model || row < 0 || row >= model->rowCount()) {
        qWarning() << "Row out of range or model not set for the header table view.";
        return;
    }
    QBrush highlightBrush(Qt::green);

    // Apply the highlight color to all columns for the specified row
    for (int col = 0; col < model->columnCount(); ++col) {
        QModelIndex index = model->index(row, col);
        model->setData(index, highlightBrush, Qt::BackgroundRole);
    }

    // Optionally, scroll to the item and select it
    ui->HeaderTV->scrollTo(model->index(row, 0));
    ui->HeaderTV->selectionModel()->select(model->index(row, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

