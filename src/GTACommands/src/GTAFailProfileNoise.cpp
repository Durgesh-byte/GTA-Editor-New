#include "GTAFailProfileNoise.h"
#include "GTAUtil.h"

GTAFailProfileNoise::GTAFailProfileNoise():GTAFailureProfileBase(GTAFailureProfileBase::NOISE)
{

}
GTAFailProfileNoise::GTAFailProfileNoise(const GTAFailProfileNoise& rhs):GTAFailureProfileBase(GTAFailureProfileBase::NOISE)
{
    _mean=rhs.getMean();
    _Seed=rhs.getSeed();
    _Sigma=rhs.getSigma();
   
}
GTAFailProfileNoise::~GTAFailProfileNoise()
{
       
}
void GTAFailProfileNoise::setMean(const QString &iVal)
{
 _mean=iVal;
}

void GTAFailProfileNoise::setSeed(const QString &iVal)
{
 _Seed=iVal;
}
void GTAFailProfileNoise::setSigma(const QString &iVal)
{
 _Sigma=iVal;
}


QString GTAFailProfileNoise::getMean() const
{
    return _mean;
}

QString GTAFailProfileNoise::getSeed() const
{
    return _Seed;
}
QString GTAFailProfileNoise::getSigma() const
{
    return _Sigma;
} 

QString GTAFailProfileNoise::getStatement() const
{
    
    QString sVarVal1= 		QString("%1=%2").arg(XATTR_EQ_MEAN,_mean);
    QString sVarVal2= 		QString("%1=%2").arg(XATTR_EQ_SEED,_Seed);
    QString sVarVal3= 		QString("%1=%2").arg(XATTR_EQ_SIGMA,_Sigma);


    QString oEquation = QString("%1(%2,%3,%4)").arg(EQ_NOISE,sVarVal1,sVarVal2,sVarVal3);
    return oEquation;
}
 GTAFailureProfileBase* GTAFailProfileNoise:: getClone()const
{
	return new GTAFailProfileNoise(*this);
}
void  GTAFailProfileNoise::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
   /* osFuncType=QString(EQ_SQUARE).toUpper();
   
    QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_MEAN,_mean);
    QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_SEED,_Seed);
    QString sVarVal3= QString("'%1':'%2'").arg(XATTR_EQ_SIGMA,_Sigma);

    osFunctionStatement = QString("%1{%2,%3,%4}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3);*/
    osFunctionStatement=QString("'type':'NOISE','mode':2,'mean':%1,'seed':%2,'sigma':%3").arg(_mean,_Seed,_Sigma);
}
void  GTAFailProfileNoise::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{

    iSerialzationMap.insert(XATTR_EQ_SEED,_Seed);
    iSerialzationMap.insert(XATTR_EQ_SIGMA,_Sigma);
    iSerialzationMap.insert(XATTR_EQ_MEAN,_mean);
}
void  GTAFailProfileNoise::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{

    if (iSerialzationMap.contains(XATTR_EQ_SEED))
        _Seed=iSerialzationMap.value(XATTR_EQ_SEED);

    if (iSerialzationMap.contains(XATTR_EQ_SIGMA))
        _Sigma=iSerialzationMap.value(XATTR_EQ_SIGMA);

 
    if (iSerialzationMap.contains(XATTR_EQ_MEAN))
        _mean=iSerialzationMap.value(XATTR_EQ_MEAN);
    
}

QStringList GTAFailProfileNoise::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_mean))
         varList<<_mean;
    if (GTACommandBase::isVariable(_Seed))
        varList<<_Seed;
    if (GTACommandBase::isVariable(_Sigma))
        varList<<_Sigma;
   
    
    return varList;
}
void GTAFailProfileNoise:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
      
        if (GTACommandBase::isVariable(_mean))
            _mean.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Seed))
            _Seed.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Sigma))
           _Sigma.replace(tag,iterTag.value());
       

    }
}