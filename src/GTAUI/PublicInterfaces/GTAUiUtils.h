#pragma once
#include "GTAUI.h"

#pragma warning(push, 0)
#include <QMessageBox>
#pragma warning(pop)

namespace UiUtils {
class GTAUI_SHARED_EXPORT DoNotShowAgainMesageBox : public QMessageBox
{
    Q_OBJECT
public:
    static StandardButton information(const QString& key, QWidget* parent, const QString& title,
        const QString& text,
        QMessageBox::StandardButtons buttons = Ok,
        QMessageBox::StandardButton defaultButton = NoButton);
    static void clearAll();

    StandardButton execute();

private:
    explicit DoNotShowAgainMesageBox(const QString& key, QWidget* parent = nullptr);

    QString m_key;
};
}
