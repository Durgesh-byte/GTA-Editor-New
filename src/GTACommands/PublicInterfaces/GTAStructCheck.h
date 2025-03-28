#ifndef STRUCT_CHECK  //begin header guard
#define STRUCT_CHECK

struct StructCheck {
	//Struct to allow outside objects to get all parameters at once.
	QStringList		lstParam;
	QStringList		lstValue;
	QStringList		lstCondition;
	QStringList		lstBinaryOperators;
	QStringList		lstFunctionalStatus;
	QList<double>	lstPrecisionValue;
	QStringList		lstPrecisionValueFE;
	QList<bool>		isCheckOnlyValue;
	QStringList		lstSDIStatus;
	QStringList		lstParityStatus;
	QList<bool>		isSDIOnly;
	QList<bool>		isParityOnly;
	QList<bool>		isRefreshRateOnly;
	QStringList		lstPrecisionType;
	QList<bool>		isCheckFS;
};

#endif //STRUCT_HEADER