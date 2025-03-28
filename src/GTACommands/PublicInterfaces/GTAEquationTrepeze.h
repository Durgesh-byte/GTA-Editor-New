#ifndef GTAEQUATIONTREPEZE_H
#define GTAEQUATIONTREPEZE_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QHash>
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationTrepeze : public GTAEquationBase
{

public:
    GTAEquationTrepeze();
	GTAEquationTrepeze(const GTAEquationTrepeze& rhs);
	virtual ~GTAEquationTrepeze();

    int getRampCount() const;
    int getLengthCount() const;
    int getLevelCount() const;
    QString getRamp(int Idx) const;
    QString getLevel(int Idx) const;
    QString getLength(int Idx) const;
    void insertRamp(int index, const QString & iRamp );
    void insertLevel(int index, const QString & iLevel );
    void insertLength(int index, const QString & iLength );

    virtual QString getStatement() const;
    GTAEquationBase* getClone()const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieve the variable name - variable value pair
      */
    void getEquationArguments(QHash<QString,QString>& iSerializationMap)const;
    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    void setEquationArguments(const QHash<QString,QString>& iSerializationMap);
    QString getSimulationName()const{return EQ_TRAPEZE;}

    QStringList getVariableList() const  ;
    void ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

	virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }

    QString getArgumentStatement()const;

private:
    QHash<int,QString> _RampMapTable;
    QHash<int,QString> _LengthMapTable;
    QHash<int,QString> _LevelMapTable;
};

#endif // GTAEQUATIONTREPEZE_H
