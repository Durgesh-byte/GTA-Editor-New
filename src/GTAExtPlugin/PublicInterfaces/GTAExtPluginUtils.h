#ifndef AINGTAEXTPLUGINUTILS_H
#define AINGTAEXTPLUGINUTILS_H


/**
  Definitions of FSII commands and Attributes.
**/

#define FSII_ELEM_ROOT  "fs:flatscriptII"
#define FSII_ELEM_SOURCES "fs:sources"
#define FSII_ELEM_ICD "fs:icd"
#define FSII_ELEM_TP "fs:test_procedure"
#define FSII_ELEM_CALL "fs:function_call"
#define FSII_ELEM_FUNCTION "fs:function"
#define FSII_ELEM_DEF "fs:definitions"
#define FSII_ELEM_SIGNAL "fs:signal"
#define FSII_ELEM_REFERENCE "fs:reference"
#define FSII_ELEM_FUNC_BLOCK "fs:function_block"
#define FSII_ELEM_LOG "fs:log_entry"
#define FSII_ELEM_CONNECT "fs:connect"
#define FSII_ELEM_SET "fs:set"
#define FSII_ELEM_WAIT "fs:wait"
#define FSII_ELEM_WRITE "fs:write"
#define FSII_ELEM_READ "fs:read"
#define FSII_ELEM_CONST "fs:const"
#define FSII_ELEM_CHECK "fs:check"
#define FSII_ELEM_CONDITIONS "fs:conditions"
#define FSII_ELEM_CONDITION "fs:condition"
#define FSII_ELEM_LEFT "fs:left"
#define FSII_ELEM_RIGHT "fs:right"
#define FSII_ELEM_MARKUP "fs:markup"
#define FSII_ELEM_CATEGORY "fs:category"
#define FSII_ELEM_DISCONNECT "fs:disconnect"
#define FSII_ELEM_PARAMETER "fs:parameter"

#define FSII_ATTR_FUNC "function"
#define FSII_ATTR_NAME "name"
#define FSII_ATTR_SUT "sut"
#define FSII_ATTR_APP "application"
#define FSII_ATTR_SOF "stop_on_fail"
#define FSII_ATTR_DEACTIVATE "deactivate"
#define FSII_ATTR_LOG "log"
#define FSII_ATTR_TEXT "text"
#define FSII_ATTR_VALUE "value"
#define FSII_ATTR_UNIT "unit"
#define FSII_ATTR_OPERATOR "operator"
#define FSII_ATTR_LOGIC_CONNECTOR "logical_connector"

#define FSII_ATTR_UNIT_MS "ms"

/**
  Definitions of AVATAR commands and Attributes.
**/

#define AVATAR_CMD_SET ""
#define AVATAR_CMD_MSG "MSG:"
#define AVATAR_CMD_CALL "MACRO:"
#define AVATAR_CMD_WAIT_FOR "WAIT("
#define AVATAR_CMD_ACK "ACK:"
#define AVATAR_CMD_CHK "CHK:"
#define AVATAR_CMD_VER "VER:"



/**
  Definitions of Auto TR commands and Attributes.
**/

#define AUTOTR_CMD_TEXT "TEXT" //import as print and title both
#define AUTOTR_CMD_SET "SET"
#define AUTOTR_CMD_WAIT "WAIT"
#define AUTOTR_CMD_CHECK "CHECK"
#define AUTOTR_CMD_OBSERVE "OBSERVE" // import as print command
#define AUTOTR_CMD_MATLAB_CMD ">>" //import as manual action //>>A380_SimulinkDTS_BCS_L2_INI or  >>AmbientTemp = 25;


#define AUTOTR_ATTR_WAIT_OR "OR" // imported as wait until with timeout
#define AUTOTR_ATTR_CHECK_FOR "FOR" //confiramtion time in check
#define AUTOTR_ATTR_CHECK_WITHIN "WITHIN" //timeout option
#define AUTOTR_ATTR_CHECK_FATAL "FATAL" // Action on Fail == stop
#define AUTOTR_ATTR_CHECK_TOL "tol"


#define AUTOTR_OPERATOR_EQ "="
#define AUTOTR_OPERATOR_NOTEQ "!="
#define AUTOTR_OPERATOR_GTEQ ">="
#define AUTOTR_OPERATOR_LTEQ "<="
#define AUTOTR_OPERATOR_LT "<"
#define AUTOTR_OPERATOR_GT ">"


#define AUTOTR_TAG_LOG "#LOG" // import as print table
#define AUTOTR_TAG_LOOP "#LOOP"
#define AUTOTR_TAG_PLOT "#PLOT" //import as print table



#define AUTOTR_TAG_BEGIN_IC "#BEGIN IC"
#define AUTOTR_TAG_END_IC "#END IC"
#define AUTOTR_TAG_BEGIN_TEXT "#BEGIN TEXT"
#define AUTOTR_TAG_END_TEXT "#END TEXT"
#define AUTOTR_TAG_BEGIN_TEST "#BEGIN TEST"
#define AUTOTR_TAG_END_TEST "#END TEST"

#define AUTOTR_VAL_TYPE_RANGE "RANGE"
#define AUTOTR_VAL_TYPE_VECTOR "VECTOR"
#define AUTOTR_VAL_TYPE_CONST "CONST"
#define AUTOTR_VAL_TYPE_ENUM "ENUM"
#define AUTOTR_VAL_TYPE_PARAM "PARAM"
#define AUTOTR_VAL_TYPE_UNKNOWN "UNKNOWN"

//--------------------------------------------------

#define ERR_PLUGIN "Plugin Error :"


#endif // AINGTAEXTPLUGINUTILS_H


