#define GTAAPPLOGMSGREDIRECTOR_H
#define GTAAPPLOGMSGREDIRECTOR_H

#include "GTAControllers.h"

#pragma warning(push, 0)
#include "GTAMdiController.h"
#include <QtDebug>
#pragma warning(pop)

/**
        @brief	Allows to redirect qDebug output. Adapted for Qt5 and Qt4
*/
class GTAControllers_SHARED_EXPORT GTAAppLogMsgRedirector
{
public:

    /**
      * @brief Redirect info/warning type messages to application log window. Function is adapted for Qt5
      * @itype: Qt Message type enum
      * @icontext: Source of the incoming message (function, file line, version)
      * @imsg: Any message which is coming from any of the QT classes QDebug, QWarning, QCritical.
      */
    static void info_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg);
    /**
      * @brief Redirect debug type messages to application log window. Function is adapted for Qt5
      * @type: Qt Message type enum
      * @icontext: Source of the incoming message (function, file line, version)
      * @msg: Any message which is coming from any of the QT classes QDebug, QWarning, QCritical.
      */
    static void debug_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg);
    /**
      * @brief Redirect all type of messages to application log window. Function is adapted for Qt5
      * @itype: Qt Message type enum
      * @icontext: Source of the incoming message (function, file line, version)
      * @imsg: Any message which is coming from any of the QT classes QDebug, QWarning, QCritical.
      */
    static void all_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg);
};