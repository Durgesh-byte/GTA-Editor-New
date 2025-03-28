#ifndef GTAEQUATIONCRENELS_H
#define GTAEQUATIONCRENELS_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationCrenels : public GTAEquationBase
{

public:
    GTAEquationCrenels();
	GTAEquationCrenels(const GTAEquationCrenels& rhs);
    virtual ~GTAEquationCrenels();

    int getLengthCount() const;
    int getLevelCount() const;
    QString getLevel(int Idx) const;
    QString getLength(int Idx) const;
    void insertLevel(int index, const QString & iLevel );
    void insertLength(int index, const QString & iLength );

    virtual QString getStatement() const;
	GTAEquationBase* getClone() const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieves the variable name - variable value pair
      */
    void getEquationArguments(QHash<QString,QString>& iSerializationMap)const;
    /**
      * This function sets the value of equation from the variable name- variable value pair
      */
    void setEquationArguments(const QHash<QString,QString>& iSerializationMap);
    QString getSimulationName()const{return EQ_CRENELS;}

    QStringList getVariableList() const;
    void ReplaceTag(const QMap<QString,QString>& iTagValueMap);

    virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }

    QString getArgumentStatement() const;

private:
    QHash<int,QString> _LengthMapTable;
    QHash<int,QString> _LevelMapTable;

};

#endif // GTAEQUATIONCRENELS_H
