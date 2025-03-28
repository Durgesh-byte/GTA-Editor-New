#ifndef AINCMNTEMPLATEFUXNS_H_
#define AINCMNTEMPLATEFUXNS_H_

#include <QHash>
/*
template<class KEY, class VALUE>
class AINCMNTemplateFuxns
{
public:
	//AINCMNTemplateFuxns();
	static void clearMemory(QHash<KEY, VALUE *> & iHashOfData);
};*/

#include <QHashIterator>

template<class KEY, class VALUE>
void clearMemory(QHash<KEY, VALUE *>& iHashOfData)
{
	QHashIterator<KEY, VALUE *> itr(iHashOfData);
	while (itr.hasNext())
	{
		itr.next();
		VALUE * pDATA = itr.value();
		if (pDATA != 0)
		{
			delete pDATA;
			pDATA = 0;
		}
	}
	iHashOfData.clear();
}

template<class DATA>
void clearMemory(QList<DATA *>& iListOfData)
{
	for (int i=0; i<iListOfData.count(); i++)
	{
		DATA * pCurObject = iListOfData.at(i);
		if (pCurObject != 0)
		{
			delete pCurObject;
			pCurObject = 0;
		}
	}
	
	iListOfData.clear();
}

template<class KEY, class VALUE>
QList<QList<VALUE>> getHashValuesAsPerKeySeq(QList<KEY> lstSequenceOfKeys, 
														QList<QHash<KEY, VALUE>>& iListOfHashValues)
{
	QList<QList<VALUE>> lstOfLstOfValues;
	for (int i=0; i<iListOfHashValues.count(); i++)
	{
		QList<VALUE> lstOfValues;
		QHash<KEY, VALUE> hshOfValues = iListOfHashValues.at(i);
		for (int j=0; j<lstSequenceOfKeys.count(); j++)
		{
			KEY key = lstSequenceOfKeys.at(j);
			VALUE value = hshOfValues.value(key);

			lstOfValues.append(value);
		}

		lstOfLstOfValues.append(lstOfValues);
	}

	return lstOfLstOfValues;
}

template<class KEY>
QList<QStringList> getHashValuesAsPerKeySeq(QList<KEY> lstSequenceOfKeys, 
														QList<QHash<KEY, QString>>& iListOfHashValues)
{
	QList<QStringList> lstOfLstOfValues;
	for (int i=0; i<iListOfHashValues.count(); i++)
	{
		QStringList lstOfValues;
		QHash<KEY, QString> hshOfValues = iListOfHashValues.at(i);
		for (int j=0; j<lstSequenceOfKeys.count(); j++)
		{
			KEY key = lstSequenceOfKeys.at(j);
			QString value = hshOfValues.value(key);

			lstOfValues.append(value);
		}

		lstOfLstOfValues.append(lstOfValues);
	}

	return lstOfLstOfValues;
}

template<class KEY, class VALUE>
QList<QStringList> getHashValuesinStringFormatAsPerKeySeq(QList<KEY> lstSequenceOfKeys, 
											QList<QHash<KEY, VALUE>>& iListOfHashValues)
{
	QList<QStringList> lstOfLstOfValues;
	for (int i=0; i<iListOfHashValues.count(); i++)
	{
		QStringList lstOfValues;
		QHash<KEY, VALUE> hshOfValues = iListOfHashValues.at(i);
		QHash<QString, QString> hashOfRowValues = getHashValuesinStringFormat(hshOfValues);

		for (int j=0; j<lstSequenceOfKeys.count(); j++)
		{
			KEY key = lstSequenceOfKeys.at(j);
			QString value = hashOfRowValues.value(key);

			lstOfValues.append(value);
		}

		lstOfLstOfValues.append(lstOfValues);
	}

	return lstOfLstOfValues;
}

template<class VALUE>
QHash<QString, QString> getHashValuesinStringFormat(QHash<QString,VALUE> & iHashOfRvalues)
{
	QHash<QString, QString> hashOfRowValues;
	QStringList listOfKeys = iHashOfRvalues.keys();
	for (int i=0; i<listOfKeys.count(); i++)
	{
		QString sKey = listOfKeys.at(i);
		QString sColValue = iHashOfRvalues.value(sKey).toString();
		hashOfRowValues.insert(sKey,sColValue);
	}

	return hashOfRowValues;
}



#endif // AINCMNTEMPLATEFUXNS_H_
