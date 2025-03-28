#include <QTextEdit>
#include <QString>
#include <QKeyEvent>
#include <QRegExp>
#include <QObject>
class GTARestrictedTextEdit: public QTextEdit
{
   // Q_OBJECT
public:
    GTARestrictedTextEdit(QWidget *parent = 0);
    virtual ~GTARestrictedTextEdit();
protected:
    void	keyPressEvent ( QKeyEvent * event );
 public slots:
    void replaceInvalidChar();

};
