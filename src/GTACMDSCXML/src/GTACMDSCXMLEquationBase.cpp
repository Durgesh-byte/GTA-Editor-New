#include "GTACMDSCXMLEquationBase.h"
#include <QFile>
#include <QRegExp>

GTACMDSCXMLEquationBase::GTACMDSCXMLEquationBase()
{
}

GTACMDSCXMLEquationBase::~GTACMDSCXMLEquationBase()
{
}

/**
 * @brief Extracts data from a specified column and row in a CSV file.
 *
 * This function reads a CSV file and extracts the data from a specific column at a given row index.
 * The column is determined based on the `data` parameter which should contain the column name.
 * The function uses delimiter and file path provided by the `pForEach` object to read and parse the CSV file.
 *
 * @param data A QString containing the column name to extract data from.
 * @param pForEach A pointer to a GTAActionForEach object which provides the delimiter and file path.
 * @param targetRowIndex The row index from which to extract the column data.
 * @return QString containing the extracted data from the specified column and row. Returns an empty string if the column or row is not found, or in case of an error.
 */
QString extractColumnDataAtRow(const QString& data, GTAActionForEach* pForEach, int targetRowIndex)
{
    QString extractedData;
    QString columnName = data.split("__").last();
    int columnParam = 0;
    int lineCount = 0;
    QString line;
    QString delim = pForEach->getDelimiterValue();
    QString csvPath;
    if (pForEach->getRelativePath().isEmpty())
    {
        csvPath = pForEach->getPath();
    }
    else
    {
        QString repoPath = GTAUtil::getRepositoryPath();
        csvPath = QString("%1%2").arg(repoPath, pForEach->getRelativePath());
    }
    QFile fileObj(csvPath);

    if (fileObj.open(QFile::ReadOnly | QFile::Text))
    {
        while (!fileObj.atEnd() && lineCount <= targetRowIndex)
        {
            line = fileObj.readLine().trimmed();
            if (line.endsWith("\n")) {
                line.remove("\n");
            }
            QStringList lineList = line.split(delim);

            if (lineCount == 0)
            {
                columnParam = lineList.indexOf(columnName);
            }
            else if (lineCount == targetRowIndex)
            {
                if (columnParam >= 0 && columnParam < lineList.size()) {
                    extractedData = lineList.at(columnParam);
                }
                break;
            }
            lineCount++;
        }
        fileObj.close();
    }

    return extractedData;
}

/**
 * This function fills the SingleSetCommandStructure and rhsParamList for assign generation later.
 * @ipEq: EquationBase command that contains constant, ramp, sinus and all other equation children attributes
 * @ioSetStruct: pointer reference to the single set structure that is to be filled
 * irhsParamList: list of GTAICDParameter which is appended if the parameter is needed to be fetched
 * @return: void
*/
void GTACMDSCXMLEquationBase::getSetStructureFilled(GTAEquationBase *& ipEq, SingleSetCommandStructure *&ioSetStruct, QList<GTAICDParameter> &irhsParamList, GTAActionForEach* pForEach, int forEachRowNumber)
{
	bool dbParam = false;
	QString functionType, functionValue;
    ipEq->getFunctionStatement(functionType, functionValue);
    functionType = ipEq->getSimulationName();

    ioSetStruct->funcType = functionType;
    ioSetStruct->funcValue = functionValue;
    QString rhsVar;
    if(functionType == EQ_CONST)
    {
        GTAEquationConst *pConstEq = dynamic_cast<GTAEquationConst*>(ipEq);
        if(pConstEq != NULL)
        {
            rhsVar = pConstEq->getConstant();
            if (pConstEq->getConstant() == ACT_CONST_EQN_TIME_TYPE_UTC)
            {
                rhsVar = "Date.now()";
            }
            else if (pConstEq->getConstant() == ACT_CONST_EQN_TIME_TYPE_FORMATTED)
            {
                rhsVar = "Date()";
            }
        }


        GTAICDParameter rhsParam;
        //rhsParam.setSignalName(rhsVar);
        rhsParam.setTempParamName(rhsVar);
		rhsParam.setName(rhsVar);
		rhsParam.setValue(rhsVar);
		dbParam = resolveParams(rhsParam);
        //GTACMDSCXMLUtils::updateWithGenToolParam(rhsParam);
        ioSetStruct->rhsParam = rhsParam;
		if (rhsParam.getToolName() != "NA")
		{
			irhsParamList.append(rhsParam);
		}
    }

    else if(functionType == EQ_RAMP)
    {
        GTAEquationRamp *pRampEq = dynamic_cast<GTAEquationRamp*>(ipEq);
		if (ioSetStruct->lhsParam.getParameterType() != "VMAC")
		{
            if(pRampEq != NULL)
            {
                // This is to figure out if the RAMP is a complex one with start value as DB param with a combination of
                // end value or offset
                QString currentValue = pRampEq->getStart();
                GTAICDParameter currentParam;
                currentValue = currentValue.trimmed();
                if (currentValue.startsWith("line.") && pForEach != NULL)
                {
                    QString currentValueAtPosition = extractColumnDataAtRow(currentValue, pForEach, forEachRowNumber);
                    currentParam.setSignalName(currentValueAtPosition);
                    currentParam.setTempParamName(currentValueAtPosition);
                }
                else
                {
                    currentParam.setSignalName(currentValue);
                    currentParam.setTempParamName(currentValue);
                }
            
                bool isValid = resolveParams(currentParam);
                if(isValid)
                {
                    //GTACMDSCXMLUtils::updateWithGenToolParam(currentParam);
                    if (currentParam.getToolName() != "NA")
                    {
                        irhsParamList.append(currentParam);
                    }
                }

                //keep the JSON inputs blank to allow local parameter assignment
                QString osFunctionStatement=QString("%1{'%2':'','%3':'','%4':'','Solid':'%5','Continuous':'%6'}").arg("'FunctionValue':",
                                                                                        XATTR_EQ_RISETIME, "StartValue", "EndValue",
                                                                                        pRampEq->getSolidState(),pRampEq->getContinousState());

                //keep the JSON inputs blank to allow local parameter assignment
                ioSetStruct->funcValue = osFunctionStatement;
                ioSetStruct->EqnRampStruct->rampStartParam = currentParam;
                QString riseTime = pRampEq->getRiseTime();
                if (riseTime.startsWith("line.") && pForEach != NULL)
                {
                    ioSetStruct->EqnRampStruct->rampRiseValue = extractColumnDataAtRow(riseTime, pForEach, forEachRowNumber);
                }
                else {
                    ioSetStruct->EqnRampStruct->rampRiseValue = riseTime;
                }
             

                ioSetStruct->EqnRampStruct->isRampEndValueOffset = pRampEq->isEndValueOffset();
                QString endValue = pRampEq->getEnd();
                if (endValue.startsWith("line.") && pForEach != NULL && !ioSetStruct->EqnRampStruct->isRampEndValueOffset)
                {
                    endValue = extractColumnDataAtRow(endValue, pForEach, forEachRowNumber);
                }
                if(ioSetStruct->EqnRampStruct->isRampEndValueOffset)
                {
                    QStringList sepValues = endValue.split("+");
                    if(!sepValues.isEmpty())
                    {
                        endValue = sepValues.last();
                        endValue = endValue.trimmed();
                        if (endValue.contains("line") && pForEach != NULL)
                        {
                            endValue = extractColumnDataAtRow(endValue, pForEach, forEachRowNumber);
                        }
                    }
                }
                ioSetStruct->EqnRampStruct->rampEndValue = endValue;

			    }
		    }
		else
		{
			GTAEquationTrepeze * pTrapezeEq = new GTAEquationTrepeze;
			ioSetStruct->funcType = EQ_TRAPEZE;
			QString riseTime = pRampEq->getRiseTime();
			QString startValue = pRampEq->getStart();
			QString endValue = pRampEq->getEnd();
			QString level1 = startValue;
			QString level23 = endValue;
			QString ramp1 = QString::number((endValue.toDouble() - startValue.toDouble()) / riseTime.toDouble());
			QString zero = QString::number(0);
			pTrapezeEq->insertRamp(1, ramp1);
			pTrapezeEq->insertRamp(2, zero);
			pTrapezeEq->insertLevel(1, level1);
			pTrapezeEq->insertLevel(2, level23);
			pTrapezeEq->insertLevel(3, level23);
			pTrapezeEq->insertLength(1, zero);
			pTrapezeEq->insertLength(2, zero);
			pTrapezeEq->setSolidState(pRampEq->getSolidState());
			pTrapezeEq->setContinousState(pRampEq->getContinousState());
			if (pTrapezeEq != NULL)
			{
				QString args = pTrapezeEq->getArgumentStatement();
				if (!args.isEmpty())
					args.resize(args.size() - 1);
				QString osFunctionStatement = QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg("'FunctionValue':",
					args, pTrapezeEq->getSolidState(), pTrapezeEq->getContinousState());
				ioSetStruct->funcValue = osFunctionStatement;
				ioSetStruct->EqnTrapezeStruct->trapezeValues = pTrapezeEq->getArgumentStatement();
			}
        }
    }
	// @27/04/2021
    else if(functionType == EQ_SAWTOOTH)
	{
		GTAEquationSawTooth *pSawToothEq = dynamic_cast<GTAEquationSawTooth*>(ipEq);
		if (pSawToothEq != NULL)
		{
			//keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
                XATTR_EQ_PERIOD, XATTR_EQ_MAXVALUE, XATTR_EQ_MINVALUE, XATTR_EQ_STARTVALUE, XATTR_EQ_RAMPMODE, pSawToothEq->getSolidState(), pSawToothEq->getContinousState());

			//keep the JSON inputs blank to allow local parameter assignment
			ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnSawToothStruct->sawPeriodValue    = pSawToothEq->getPeriod();
            ioSetStruct->EqnSawToothStruct->sawMaxValue       = pSawToothEq->getMaxValue();
            ioSetStruct->EqnSawToothStruct->sawMinValue       = pSawToothEq->getMinValue();
            ioSetStruct->EqnSawToothStruct->sawStartValue     = pSawToothEq->getStartValue();
            ioSetStruct->EqnSawToothStruct->sawRampModeValue  = pSawToothEq->getRampMode();
		}
	}
    // @15/07/2021
    else if(functionType == EQ_SINUS)
    {
        GTAEquationSinus *pSinusEq = dynamic_cast<GTAEquationSinus*>(ipEq);
		if (ioSetStruct->lhsParam.getParameterType() != "VMAC")
		{	
            if (pSinusEq != NULL)
            {
                //keep the JSON inputs blank to allow local parameter assignment
                QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
                    XATTR_EQ_PERIOD, XATTR_EQ_MAXVALUE, XATTR_EQ_MINVALUE, XATTR_EQ_STARTVALUE, XATTR_EQ_DIRECTION, pSinusEq->getSolidState(), pSinusEq->getContinousState());

                //keep the JSON inputs blank to allow local parameter assignment
                ioSetStruct->funcValue = osFunctionStatement;

                // Period
                QString periodValue = pSinusEq->getPeriod();
                if (periodValue.startsWith("line.") && pForEach != NULL)
                {
                    if (periodValue.contains("1/"))
                    {
                        QString periodValueRow = extractColumnDataAtRow(periodValue, pForEach, forEachRowNumber);
                        if (periodValueRow.toDouble() != 0)
                        {
                            periodValue = QString::number(1 / periodValueRow.toDouble());
                        }
                    }
                    else
                    {
                        periodValue = extractColumnDataAtRow(periodValue, pForEach, forEachRowNumber);
                    }
                }
                ioSetStruct->EqnSinusStruct->sinPeriodValue = periodValue;

                // Maximum Value
                QString maxValue = pSinusEq->getMaxValue();
                if (maxValue.contains("line.") && pForEach != NULL)
                {
                    if (maxValue.contains("+"))
                    {
                        QStringList parts = maxValue.split('+', QString::SkipEmptyParts);
                        QString offset;
                        QString amplitude;
                        double offsetDouble;
                        double amplitudeDouble;
                        if (parts.size() == 2)
                        { 
                            offset = parts[0].trimmed(); 
                            amplitude = parts[1].trimmed(); 
                        }
                        if (offset.contains("line."))
                            offsetDouble = extractColumnDataAtRow(offset, pForEach, forEachRowNumber).toDouble();
                        else
                            offsetDouble = offset.toDouble();

                        if (amplitude.contains("line."))
                            amplitudeDouble = extractColumnDataAtRow(amplitude, pForEach, forEachRowNumber).toDouble();
                        else
                            amplitudeDouble = amplitude.toDouble();

                        maxValue = QString::number(offsetDouble + amplitudeDouble);
                    }
                    else
                    {
                        maxValue = extractColumnDataAtRow(maxValue, pForEach, forEachRowNumber);
                    }
                }
                ioSetStruct->EqnSinusStruct->sinMaxValue = maxValue;

                // Minimum Value
                QString minValue = pSinusEq->getMinValue();
                if (minValue.contains("line.") && pForEach != NULL)
                {
                    bool isInBrackets = false;
                    int lastHyphenIndex = -1;
                    for (int i = minValue.length() - 1; i >= 0; --i)
                    {
                        if (minValue[i] == ']')
                        {
                            isInBrackets = true;
                        }
                        else if (minValue[i] == '[')
                        {
                            isInBrackets = false;
                        }
                        else if (minValue[i] == '-' && !isInBrackets)
                        {
                            lastHyphenIndex = i;
                            break;
                        }
                    }
                    if (lastHyphenIndex != -1)
                    {
                        QString offset = minValue.left(lastHyphenIndex);
                        QString amplitude = minValue.mid(lastHyphenIndex + 1);
                        double offsetDouble;
                        double amplitudeDouble;
                    
                        if (offset.contains("line."))
                            offsetDouble = extractColumnDataAtRow(offset, pForEach, forEachRowNumber).toDouble();
                        else
                            offsetDouble = offset.toDouble();

                        if (amplitude.contains("line."))
                            amplitudeDouble = extractColumnDataAtRow(amplitude, pForEach, forEachRowNumber).toDouble();
                        else
                            amplitudeDouble = amplitude.toDouble();

                        minValue = QString::number(offsetDouble - amplitudeDouble);
                    }
                
                    else
                    {
                        minValue = extractColumnDataAtRow(minValue, pForEach, forEachRowNumber);
                    }
                }
                ioSetStruct->EqnSinusStruct->sinMinValue = minValue;

                // Start Value
                QString startValue = pSinusEq->getStartValue();
                if (startValue.contains("line.") && pForEach != NULL)
                {
                    if (startValue.contains("sin("))
                    {
                        QString phase;
                        QString amplitude;
                        QString offset;
                        double phaseDouble;
                        double amplitudeDouble;
                        double offsetDouble;
                        QRegExp regex("sin\\(([^)]*)\\)\\*([^+]+)\\+([^+]+)");
                        if (regex.indexIn(startValue) != -1) {
                            phase = regex.cap(1); 
                            amplitude = regex.cap(2); 
                            offset = regex.cap(3); 
                        }
                        if (phase.contains("line."))
                            phaseDouble = extractColumnDataAtRow(phase, pForEach, forEachRowNumber).toDouble();
                        else
                            phaseDouble = phase.toDouble();

                        if (amplitude.contains("line."))
                            amplitudeDouble = extractColumnDataAtRow(amplitude, pForEach, forEachRowNumber).toDouble();
                        else
                            amplitudeDouble = amplitude.toDouble();

                        if (offset.contains("line."))
                            offsetDouble = extractColumnDataAtRow(offset, pForEach, forEachRowNumber).toDouble();
                        else
                            offsetDouble = offset.toDouble();

                        startValue = QString::number(qSin(phaseDouble) *amplitudeDouble + offsetDouble);
                    }
                    else
                    {
                        startValue = extractColumnDataAtRow(startValue, pForEach, forEachRowNumber);

                    }
                }
                ioSetStruct->EqnSinusStruct->sinStartValue = startValue;

                //Direction
                QString direction = pSinusEq->getDirection();
                if (direction.contains("line.") && pForEach != NULL)
                {
                    direction = extractColumnDataAtRow(direction, pForEach, forEachRowNumber);
                }
                ioSetStruct->EqnSinusStruct->sinDirectionValue = direction; 
            }
        }
		else
		{
			GTAEquationSineCrv *pSineCurveEq = new GTAEquationSineCrv;
			ioSetStruct->funcType = EQ_SINECRV;
			QString period = pSinusEq->getPeriod();
			QString amplitude = pSinusEq->getAmplitude();
			QString offset = pSinusEq->getOffset();
			QString phase = pSinusEq->getPhase();
			pSineCurveEq->setGain(amplitude);
			pSineCurveEq->setPulsation(period);
			pSineCurveEq->setPhase(phase);
			pSineCurveEq->setOffset(offset);
			pSineCurveEq->setTrignometryOperator(QString("sin"));
			pSineCurveEq->setSolidState(pSinusEq->getSolidState());
			pSineCurveEq->setContinousState(pSinusEq->getContinousState());
			if (pSineCurveEq != NULL)
			{
				QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
					XATTR_EQ_AMP, XATTR_EQ_TRIG_OP, XATTR_EQ_PERIOD, XATTR_EQ_PHASE, XATTR_EQ_OFFSET, pSineCurveEq->getSolidState(), pSineCurveEq->getContinousState());
				ioSetStruct->funcValue = osFunctionStatement;
				ioSetStruct->EqnSineCurveStruct->sCurveGainValue = pSineCurveEq->getGain();
				ioSetStruct->EqnSineCurveStruct->sCurveOperatorValue = pSineCurveEq->getTrignometryOperator();
				ioSetStruct->EqnSineCurveStruct->sCurvePulsationValue = pSineCurveEq->getPulsation();
				ioSetStruct->EqnSineCurveStruct->sCurvePhaseValue = pSineCurveEq->getPhase();
				ioSetStruct->EqnSineCurveStruct->sCurveOffsetValue = pSineCurveEq->getOffset();
			}
		}
    }
    // @19/07/2021
    else if(functionType == EQ_SQUARE)
    {
        GTAEquationPulse *pPulseEq = dynamic_cast<GTAEquationPulse*>(ipEq);
        if (pPulseEq != NULL)
        {
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
                XATTR_EQ_PERIOD, XATTR_EQ_MAXVALUE, XATTR_EQ_MINVALUE, XATTR_EQ_DUTYCYCLE, XATTR_EQ_DIRECTION, pPulseEq->getSolidState(), pPulseEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;

            // Period
            QString periodValue = pPulseEq->getPeriod();
            if (periodValue.startsWith("line.") && pForEach != NULL)
            {
                periodValue = extractColumnDataAtRow(periodValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnPulseStruct->pulPeriodValue = periodValue;

            // Max Value
            QString maxValue = pPulseEq->getMaxValue();
            if (maxValue.startsWith("line.") && pForEach != NULL)
            {
                maxValue = extractColumnDataAtRow(maxValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnPulseStruct->pulMaxValue = maxValue;

            // Min Value
            QString minValue = pPulseEq->getMinValue();
            if (minValue.startsWith("line.") && pForEach != NULL)
            {
                minValue = extractColumnDataAtRow(minValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnPulseStruct->pulMinValue = minValue;

            // Duty Cycle
            QString dutyCycle = pPulseEq->getDutyCycle();
            if (dutyCycle.startsWith("line.") && pForEach != NULL)
            {
                dutyCycle = extractColumnDataAtRow(dutyCycle, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnPulseStruct->pulDutyCycleValue = dutyCycle;

            //Direction
            QString directionValue = pPulseEq->getDirection();
            if (directionValue.startsWith("line.") && pForEach != NULL)
            {
                directionValue = extractColumnDataAtRow(directionValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnPulseStruct->pulDirectionValue = directionValue;
        }
    }
    // Added by BVU @19/07/2021
    else if(functionType == EQ_TRIANGLE)
    {
        GTAEquationTriangle *pTriangleEq = dynamic_cast<GTAEquationTriangle*>(ipEq);
        if (pTriangleEq != NULL)
        {
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
                XATTR_EQ_PERIOD, XATTR_EQ_MAXVALUE, XATTR_EQ_MINVALUE, XATTR_EQ_STARTVALUE, XATTR_EQ_DIRECTION, pTriangleEq->getSolidState(), pTriangleEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnTriangleStruct->trianglePeriodValue		= pTriangleEq->getPeriod();
            ioSetStruct->EqnTriangleStruct->triangleMaxValue		= pTriangleEq->getMaxValue();
            ioSetStruct->EqnTriangleStruct->triangleMinValue		= pTriangleEq->getMinValue();
            ioSetStruct->EqnTriangleStruct->triangleStartValue      = pTriangleEq->getStartValue();
            ioSetStruct->EqnTriangleStruct->triangleDirectionValue  = pTriangleEq->getDirection();
        }
    }
    // @20/07/2021
    else if(functionType == EQ_STEP)
    {
        GTAEquationStep *pStepEq = dynamic_cast<GTAEquationStep*>(ipEq);
		if (ioSetStruct->lhsParam.getParameterType() != "VMAC")
		{
        if (pStepEq != NULL)
        {
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','Solid':'%5','Continuous':'%6'}").arg("'FunctionValue':",
                XATTR_EQ_PERIOD, XATTR_EQ_STARTVALUE, XATTR_EQ_ENDVALUE, pStepEq->getSolidState(), pStepEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;

            // Period Value
            QString periodValue = pStepEq->getPeriod();
            if (periodValue.startsWith("line.") && pForEach != NULL)
            {
                periodValue = extractColumnDataAtRow(periodValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnStepStruct->stepPeriodValue = periodValue;

            // Start Value
            QString startValue = pStepEq->getStartValue();
            if (startValue.startsWith("line.") && pForEach != NULL)
            {
                startValue = extractColumnDataAtRow(startValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnStepStruct->stepStartValue = startValue;

            // End Value
            QString endValue = pStepEq->getEndValue();
            if (endValue.startsWith("line.") && pForEach != NULL)
            {
                endValue = extractColumnDataAtRow(endValue, pForEach, forEachRowNumber);
            }
            ioSetStruct->EqnStepStruct->stepEndValue = endValue;
			}
		}
		else
		{
			GTAEquationCrenels *pCrenelsEq = new GTAEquationCrenels;
			ioSetStruct->funcType = EQ_CRENELS;
			QString period = pStepEq->getPeriod();
			QString startValue = pStepEq->getStartValue();
			QString endValue = pStepEq->getEndValue();
			QString level1 = startValue;
			QString level2345 = endValue;
			QString length1 = period;
			QString zero = QString::number(0);
			pCrenelsEq->insertLevel(1, level1);
			pCrenelsEq->insertLevel(2, level2345);
			pCrenelsEq->insertLevel(3, level2345);
			pCrenelsEq->insertLevel(4, level2345);
			pCrenelsEq->insertLevel(5, level2345);
			pCrenelsEq->insertLength(1, period);
			pCrenelsEq->insertLength(2, zero);
			pCrenelsEq->insertLength(3, zero);
			pCrenelsEq->insertLength(4, zero);
			pCrenelsEq->setSolidState(pStepEq->getSolidState());
			pCrenelsEq->setContinousState(pStepEq->getContinousState());
			if (pCrenelsEq != NULL)
			{
				QString args = pCrenelsEq->getArgumentStatement();
				if (!args.isEmpty())
					args.resize(args.size() - 1);
				QString osFunctionStatement = QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg("'FunctionValue':",
					args, pCrenelsEq->getSolidState(), pCrenelsEq->getContinousState());
				ioSetStruct->funcValue = osFunctionStatement;
				ioSetStruct->EqnCrenelsStruct->crenelsValues = pCrenelsEq->getArgumentStatement();
			}
        }
    }
    // @21/07/2021
    else if(functionType == EQ_CRENELS)
    {
        GTAEquationCrenels *pCrenelsEq = dynamic_cast<GTAEquationCrenels*>(ipEq);
        if (pCrenelsEq != NULL)
        {
			QString args = pCrenelsEq->getArgumentStatement();
			if(!args.isEmpty())
				args.resize(args.size() - 1);
			//keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg("'FunctionValue':",
				args, pCrenelsEq->getSolidState(), pCrenelsEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnCrenelsStruct->crenelsValues = pCrenelsEq->getArgumentStatement();
            for (int i = 0; i <= ioSetStruct->EqnCrenelsStruct->crenelsListLevel.count()-1; i++) {
                ioSetStruct->EqnCrenelsStruct->crenelsListLevel[i] = pCrenelsEq->getLevel(i+1);
            }
            for (int i = 0; i <= ioSetStruct->EqnCrenelsStruct->crenelsListLength.count()-1; i++) {
                ioSetStruct->EqnCrenelsStruct->crenelsListLength[i] = pCrenelsEq->getLength(i + 1);
            }
        }
    }
    // @23/07/2021
    else if(functionType == EQ_TRAPEZE)
    {
        GTAEquationTrepeze *pTrapezeEq = dynamic_cast<GTAEquationTrepeze*>(ipEq);
        if (pTrapezeEq != NULL)
        {
            QString args = pTrapezeEq->getArgumentStatement();
            if(!args.isEmpty())
                args.resize(args.size() - 1);
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg("'FunctionValue':",
                args, pTrapezeEq->getSolidState(), pTrapezeEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnTrapezeStruct->trapezeValues = pTrapezeEq->getArgumentStatement();
            for (int i = 0; i <= ioSetStruct->EqnTrapezeStruct->trapezeListLevel.count() - 1; i++) {
                ioSetStruct->EqnTrapezeStruct->trapezeListLevel[i] = pTrapezeEq->getLevel(i + 1);
            }
            for (int i = 0; i <= ioSetStruct->EqnTrapezeStruct->trapezeListLength.count() - 1; i++) {
                ioSetStruct->EqnTrapezeStruct->trapezeListLength[i] = pTrapezeEq->getLength(i + 1);
            }
            for (int i = 0; i <= ioSetStruct->EqnTrapezeStruct->trapezeListRamp.count() - 1; i++) {
                ioSetStruct->EqnTrapezeStruct->trapezeListRamp[i] = pTrapezeEq->getRamp(i + 1);
            }
        }
    }
    // @31/08/2021
    else if(functionType == EQ_SINECRV)
    {
        GTAEquationSineCrv *pSineCurveEq = dynamic_cast<GTAEquationSineCrv*>(ipEq);
        if (pSineCurveEq != NULL)
        {
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','%3':'','%4':'','%5':'','%6':'','Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",
                XATTR_EQ_AMP, XATTR_EQ_TRIG_OP, XATTR_EQ_PERIOD, XATTR_EQ_PHASE, XATTR_EQ_OFFSET, pSineCurveEq->getSolidState(), pSineCurveEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnSineCurveStruct->sCurveGainValue        = pSineCurveEq->getGain();
            ioSetStruct->EqnSineCurveStruct->sCurveOperatorValue    = pSineCurveEq->getTrignometryOperator();
            ioSetStruct->EqnSineCurveStruct->sCurvePulsationValue	= pSineCurveEq->getPulsation();
            ioSetStruct->EqnSineCurveStruct->sCurvePhaseValue       = pSineCurveEq->getPhase();
            ioSetStruct->EqnSineCurveStruct->sCurveOffsetValue      = pSineCurveEq->getOffset();
        }
    }
    // @01/09/2021
    else if(functionType == EQ_GAIN)
    {
        GTAEquationGain *pGainEq = dynamic_cast<GTAEquationGain*>(ipEq);
        if (pGainEq != NULL)
        {
            //keep the JSON inputs blank to allow local parameter assignment
            QString osFunctionStatement = QString("%1{'%2':'','Solid':'%3','Continuous':'%4'}").arg("'FunctionValue':",
                XATTR_EQ_GAIN, pGainEq->getSolidState(), pGainEq->getContinousState());

            //keep the JSON inputs blank to allow local parameter assignment
            ioSetStruct->funcValue = osFunctionStatement;
            ioSetStruct->EqnGainStruct->gainValue = pGainEq->getGain();
        }
    }
}

/**
 * This function creates a list of assigns to be used in SCXML generation
 * @iparamArray: name of the array. used to create the location attribute for the assign
 * @iCount: index of the array in the loop. used to create the location attribute for the assign
 * @return: List of GTASCXMLAssign(s)
*/
QList<GTASCXMLAssign> GTACMDSCXMLEquationBase::getAssignList(QString iparamArray,int iCount,SingleSetCommandStructure *&ioSetStruct)
{
    QList<GTASCXMLAssign> assignLst;

    if (ioSetStruct->funcType == EQ_CONST)
    {

        GTASCXMLAssign assign1;
        assign1.setLocation(QString("%1[%2].FunctionValue.Value").arg(iparamArray,QString::number(iCount)));

        if (ioSetStruct->rhsParam.getToolName() == "NA") //if rhs is a local parameter
        {
            if(((ioSetStruct->lhsParam.getValueType().toLower() == "string") ||(ioSetStruct->lhsParam.getValueType().toLower() == "char")) && (ioSetStruct->rhsParam.getToolType() != INTERNAL_PARAM))
				assign1.setExpression(QString("%1").arg(ioSetStruct->rhsParam.getTempParamName()));
            else
				assign1.setExpression(ioSetStruct->rhsParam.getTempParamName());
		}
        else
        {
			assign1.setExpression(QString("%1.Value").arg(ioSetStruct->rhsParam.getName()));
        }
        assignLst.append(assign1);
    }

    else if((ioSetStruct->funcType == EQ_RAMP))
    {
        GTASCXMLAssign assignRiseTime;
        GTASCXMLAssign assignStartTime;
        GTASCXMLAssign assignEndTime;

        //Rise Time
        assignRiseTime.setLocation(QString("%1[%2].FunctionValue.RiseTime").arg(iparamArray, QString::number(iCount)));
        bool isOK;
        //check if the rise time is a number. Multiply with 1000 if it is a number.
        ioSetStruct->EqnRampStruct->rampRiseValue.toDouble(&isOK);
        if (isOK)
            assignRiseTime.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnRampStruct->rampRiseValue.toDouble() * 1000)));
        else
            assignRiseTime.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnRampStruct->rampRiseValue));
        if (ioSetStruct->EqnRampStruct->rampStartParam.getToolName() != "NA")
        {
            assignStartTime.setLocation(QString("%1[%2].FunctionValue.StartValue").arg(iparamArray, QString::number(iCount)));
            assignStartTime.setExpression(QString("%1.Value").arg(ioSetStruct->EqnRampStruct->rampStartParam.getName()));
            //assignStartTime.setExpression(QString("%1.Value").arg(ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName()));

            //End Time || Offset
            if (ioSetStruct->EqnRampStruct->isRampEndValueOffset)
            {
                assignEndTime.setLocation(QString("%1[%2].FunctionValue.EndValue").arg(iparamArray, QString::number(iCount)));
                QString paramName;
                if (ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName().isEmpty())
                    paramName = ioSetStruct->EqnRampStruct->rampStartParam.getName();
                else
                    paramName = ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName();
                assignEndTime.setExpression(QString("%1.Value + %2").arg(paramName, ioSetStruct->EqnRampStruct->rampEndValue));

            }
            else
            {
                assignEndTime.setLocation(QString("%1[%2].FunctionValue.EndValue").arg(iparamArray, QString::number(iCount)));
                assignEndTime.setExpression(ioSetStruct->EqnRampStruct->rampEndValue);
            }
        }
        else
        {
            assignStartTime.setLocation(QString("%1[%2].FunctionValue.StartValue").arg(iparamArray, QString::number(iCount)));
            QString paramName;
            if (ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName().isEmpty())
                paramName = ioSetStruct->EqnRampStruct->rampStartParam.getName();
            else
                paramName = ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName();
            assignStartTime.setExpression(paramName);

            //End Time || Offset
            if (ioSetStruct->EqnRampStruct->isRampEndValueOffset)
            {
                assignEndTime.setLocation(QString("%1[%2].FunctionValue.EndValue").arg(iparamArray, QString::number(iCount)));
                QString subParamName;
                if (ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName().isEmpty())
                    subParamName = ioSetStruct->EqnRampStruct->rampStartParam.getName();
                else
                    subParamName = ioSetStruct->EqnRampStruct->rampStartParam.getTempParamName();
                assignEndTime.setExpression(QString("%1 + %2").arg(subParamName, ioSetStruct->EqnRampStruct->rampEndValue));
            }
            else
            {
                assignEndTime.setLocation(QString("%1[%2].FunctionValue.EndValue").arg(iparamArray, QString::number(iCount)));
                assignEndTime.setExpression(ioSetStruct->EqnRampStruct->rampEndValue);
            }
        }        
        
        assignLst.append(assignRiseTime);
        assignLst.append(assignStartTime);
        assignLst.append(assignEndTime);
    }
    else if((ioSetStruct->funcType == EQ_SAWTOOTH))
    {
        GTASCXMLAssign assignPeriod;
        GTASCXMLAssign assignMaxValue;
        GTASCXMLAssign assignMinValue;
        GTASCXMLAssign assignStartValue;
        GTASCXMLAssign assignRampMode;

        // Period
        assignPeriod.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnSawToothStruct->sawPeriodValue.toDouble(&isOK);
        if (isOK)
            assignPeriod.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnSawToothStruct->sawPeriodValue.toDouble() * 1000)));
        else
            assignPeriod.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnSawToothStruct->sawPeriodValue));
            
        // Amplitude, assignMinValue, Phase, RampMode
        assignMaxValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MAXVALUE));
        assignMaxValue.setExpression(ioSetStruct->EqnSawToothStruct->sawMaxValue);

        assignMinValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MINVALUE));
        assignMinValue.setExpression(ioSetStruct->EqnSawToothStruct->sawMinValue);

        assignStartValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_STARTVALUE));
        assignStartValue.setExpression(ioSetStruct->EqnSawToothStruct->sawStartValue);

        assignRampMode.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_RAMPMODE));
        assignRampMode.setExpression(QString("'%1'").arg(ioSetStruct->EqnSawToothStruct->sawRampModeValue));

        assignLst.append(assignPeriod);
        assignLst.append(assignMaxValue);
        assignLst.append(assignMinValue);
        assignLst.append(assignStartValue);
        assignLst.append(assignRampMode);
    }
    else if((ioSetStruct->funcType == EQ_SINUS))
    {
        GTASCXMLAssign assignPeriod;
        GTASCXMLAssign assignMaxValue;
        GTASCXMLAssign assignMinValue;
        GTASCXMLAssign assignStartValue;
        GTASCXMLAssign assignDirection;

        // Period
        assignPeriod.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnSinusStruct->sinPeriodValue.toDouble(&isOK);
        if (isOK)
            assignPeriod.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnSinusStruct->sinPeriodValue.toDouble() * 1000)));
        else
            assignPeriod.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnSinusStruct->sinPeriodValue));

        // MaxValue, MinValue, StartValue, Direction
        assignMaxValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MAXVALUE));
        assignMaxValue.setExpression(ioSetStruct->EqnSinusStruct->sinMaxValue);

        assignMinValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MINVALUE));
        assignMinValue.setExpression(ioSetStruct->EqnSinusStruct->sinMinValue);

        assignStartValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_STARTVALUE));
        assignStartValue.setExpression(ioSetStruct->EqnSinusStruct->sinStartValue);

        assignDirection.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_DIRECTION));
        assignDirection.setExpression(QString("'%1'").arg(ioSetStruct->EqnSinusStruct->sinDirectionValue));

        assignLst.append(assignPeriod);
        assignLst.append(assignMaxValue);
        assignLst.append(assignMinValue);
        assignLst.append(assignStartValue);
        assignLst.append(assignDirection);
    }
    else if((ioSetStruct->funcType == EQ_SQUARE))
    {
        GTASCXMLAssign assignPeriod;
        GTASCXMLAssign assignMaxValue;
        GTASCXMLAssign assignMinValue;
        GTASCXMLAssign assignDutyCycle;
        GTASCXMLAssign assignDirection;

        // Period
        assignPeriod.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnPulseStruct->pulPeriodValue.toDouble(&isOK);
        if (isOK)
            assignPeriod.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnPulseStruct->pulPeriodValue.toDouble() * 1000)));
        else
            assignPeriod.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnPulseStruct->pulPeriodValue));

        // MaxValue, MinValue, DutyCycle, Direction
        assignMaxValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MAXVALUE));
        assignMaxValue.setExpression(ioSetStruct->EqnPulseStruct->pulMaxValue);

        assignMinValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MINVALUE));
        assignMinValue.setExpression(ioSetStruct->EqnPulseStruct->pulMinValue);

        assignDutyCycle.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_DUTYCYCLE));
        assignDutyCycle.setExpression(ioSetStruct->EqnPulseStruct->pulDutyCycleValue);

        assignDirection.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_DIRECTION));
        assignDirection.setExpression(QString("'%1'").arg(ioSetStruct->EqnPulseStruct->pulDirectionValue));

        assignLst.append(assignPeriod);
        assignLst.append(assignMaxValue);
        assignLst.append(assignMinValue);
        assignLst.append(assignDutyCycle);
        assignLst.append(assignDirection);
    }
    else if((ioSetStruct->funcType == EQ_TRIANGLE))
    {
        GTASCXMLAssign assignPeriod;
        GTASCXMLAssign assignMaxValue;
        GTASCXMLAssign assignMinValue;
        GTASCXMLAssign assignStartValue;
        GTASCXMLAssign assignDirection;

        // Period
        assignPeriod.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnTriangleStruct->trianglePeriodValue.toDouble(&isOK);
        if (isOK)
            assignPeriod.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnTriangleStruct->trianglePeriodValue.toDouble() * 1000)));
        else
            assignPeriod.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnTriangleStruct->trianglePeriodValue));

        // MaxValue, MinValue, StartValue, Direction
        assignMaxValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MAXVALUE));
        assignMaxValue.setExpression(ioSetStruct->EqnTriangleStruct->triangleMaxValue);

        assignMinValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_MINVALUE));
        assignMinValue.setExpression(ioSetStruct->EqnTriangleStruct->triangleMinValue);

        assignStartValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_STARTVALUE));
        assignStartValue.setExpression(ioSetStruct->EqnTriangleStruct->triangleStartValue);

        assignDirection.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_DIRECTION));
        assignDirection.setExpression(QString("'%1'").arg(ioSetStruct->EqnTriangleStruct->triangleDirectionValue));

        assignLst.append(assignPeriod);
        assignLst.append(assignMaxValue);
        assignLst.append(assignMinValue);
        assignLst.append(assignStartValue);
        assignLst.append(assignDirection);
    }
    else if ((ioSetStruct->funcType == EQ_CRENELS))
    {
        int ref_level_int = 1;
        foreach(QString level, ioSetStruct->EqnCrenelsStruct->crenelsListLevel) {
            GTASCXMLAssign assign_level;
            QString ref_level = QString("Level%1").arg(QString::number(ref_level_int));
            assign_level.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray, QString::number(iCount), ref_level));
            assign_level.setExpression(level);
            assignLst.append(assign_level);
            ref_level_int += 1;
        }

        int ref_length_int = 1;
        foreach(QString length, ioSetStruct->EqnCrenelsStruct->crenelsListLength) {
            GTASCXMLAssign assign_length;
            QString ref_length = QString("Length%1").arg(QString::number(ref_length_int));
            assign_length.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray, QString::number(iCount), ref_length));
            assign_length.setExpression(length);
            assignLst.append(assign_length);
            ref_length_int += 1;
        }
    }
    else if((ioSetStruct->funcType == EQ_STEP))
    {
        GTASCXMLAssign assignPeriod;
        GTASCXMLAssign assignStartValue;
        GTASCXMLAssign assignEndValue;

        // Period
        assignPeriod.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnStepStruct->stepPeriodValue.toDouble(&isOK);
        if (isOK)
            assignPeriod.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnStepStruct->stepPeriodValue.toDouble() * 1000)));
        else
            assignPeriod.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnStepStruct->stepPeriodValue));

        // startValue, endValue
        assignStartValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_STARTVALUE));
        assignStartValue.setExpression(ioSetStruct->EqnStepStruct->stepStartValue);

        assignEndValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_ENDVALUE));
        assignEndValue.setExpression(QString("%1").arg(ioSetStruct->EqnStepStruct->stepEndValue));

        assignLst.append(assignPeriod);
        assignLst.append(assignStartValue);
        assignLst.append(assignEndValue);
    }
    else if((ioSetStruct->funcType == EQ_SINECRV))
    {
        GTASCXMLAssign assignGainValue;
        GTASCXMLAssign assignOperator;
        GTASCXMLAssign assignPulsation;
        GTASCXMLAssign assignPhaseValue;
        GTASCXMLAssign assignOffsetValue;

        // Pulsation
        assignPulsation.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PERIOD));
        bool isOK;
        // Check if the Period is a number. Multiply with 1000 if it is a number in order to get value in milliseconds.
        ioSetStruct->EqnSineCurveStruct->sCurvePulsationValue.toDouble(&isOK);
        if (isOK)
            assignPulsation.setExpression(QString("%1").arg(QString::number(ioSetStruct->EqnSineCurveStruct->sCurvePulsationValue.toDouble() * 1000)));
        else
            assignPulsation.setExpression(QString("%1%2").arg(QString::fromStdString("1000*"), ioSetStruct->EqnSineCurveStruct->sCurvePulsationValue));


        // Offset, Gain, Phase, Operator
        assignOffsetValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_OFFSET));
        assignOffsetValue.setExpression(ioSetStruct->EqnSineCurveStruct->sCurveOffsetValue);

        assignGainValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_AMP));
        assignGainValue.setExpression(ioSetStruct->EqnSineCurveStruct->sCurveGainValue);

        assignPhaseValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_PHASE));
        assignPhaseValue.setExpression(ioSetStruct->EqnSineCurveStruct->sCurvePhaseValue);

        assignOperator.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_TRIG_OP));
        assignOperator.setExpression(QString("'%1'").arg(ioSetStruct->EqnSineCurveStruct->sCurveOperatorValue));

        assignLst.append(assignGainValue);
        assignLst.append(assignOperator);
        assignLst.append(assignPulsation);
        assignLst.append(assignPhaseValue);
        assignLst.append(assignOffsetValue);
    }
    else if((ioSetStruct->funcType == EQ_GAIN))
    {
        GTASCXMLAssign assignGainValue;

        // Gain
        assignGainValue.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray,QString::number(iCount),XATTR_EQ_GAIN));
        assignGainValue.setExpression(ioSetStruct->EqnGainStruct->gainValue);

        assignLst.append(assignGainValue);
    }
    else if ((ioSetStruct->funcType == EQ_TRAPEZE)) {
        int ref_level_int = 1;
        foreach(QString level, ioSetStruct->EqnTrapezeStruct->trapezeListLevel) {
            GTASCXMLAssign assign_level;
            QString ref_level = QString("Level%1").arg(QString::number(ref_level_int));
            assign_level.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray, QString::number(iCount), ref_level));
            assign_level.setExpression(level);
            assignLst.append(assign_level);
            ref_level_int += 1;
        }

        int ref_length_int = 1;
        foreach(QString length, ioSetStruct->EqnTrapezeStruct->trapezeListLength) {
            GTASCXMLAssign assign_length;
            QString ref_length = QString("Length%1").arg(QString::number(ref_length_int));
            assign_length.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray, QString::number(iCount), ref_length));
            assign_length.setExpression(length);
            assignLst.append(assign_length);
            ref_length_int += 1;
        }

        int ref_ramp_int = 1;
        foreach(QString ramp, ioSetStruct->EqnTrapezeStruct->trapezeListRamp) {
            GTASCXMLAssign assign_ramp;
            QString ref_ramp = QString("Ramp%1").arg(QString::number(ref_ramp_int));
            assign_ramp.setLocation(QString("%1[%2].FunctionValue.%3").arg(iparamArray, QString::number(iCount), ref_ramp));
            assign_ramp.setExpression(ramp);
            assignLst.append(assign_ramp);
            ref_ramp_int += 1;
        }
    }
    return assignLst;
}
