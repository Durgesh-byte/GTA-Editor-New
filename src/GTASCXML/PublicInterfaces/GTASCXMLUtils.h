#ifndef GTASCXMLUTILS_H
#define GTASCXMLUTILS_H

#include "GTASCXML.h"

#pragma warning(push, 0)
#include <qdom.h>
#include <qlist.h>
#include <qstringlist.h>
#pragma warning(pop)

//SCXML LOG Node and Attributes
#define SCXML_LOG "log"
#define SCXML_LOG_EXPR "expr"
#define SCXML_LOG_LABEL "label"

//SCXML TRANISTION Node and Attributes
#define SCXML_TRANSITION "transition"
#define SCXML_TRANSITION_COND "cond"
#define SCXML_TRANSITION_EVENT "event"
#define SCXML_TRANSITION_STATUS "status"
#define SCXML_TRANSITION_TARGET "target"

//SCXML ASSIGN Node and Attributes
#define SCXML_ASSIGN "assign"
#define SCXML_ASSIGN_EXPR "expr"
#define SCXML_ASSIGN_LOCATION "location"


//SCXML SEND Node and Attributes
#define SCXML_SEND "send"
#define SCXML_SEND_EVENT "event"
#define SCXML_SEND_DELAY "delay"
#define SCXML_SEND_DELAYEXPR "delayexpr"
#define SCXML_SEND_ID "id"

//SCXML ONENTRY Node
#define SCXML_ONENTRY "onentry"


//SCXML ONEXIT Node
#define SCXML_ONEXIT "onexit"


//SCXML FINAL Node And Attributes
#define SCXML_FINAL  "final"
#define SCXML_MAIN_TP_FINAL  "TestProcedure_final"
#define SCXML_MAIN_TP  "TestProcedure"
#define SCXML_FINAL_ID "id"

//SCXML STATE Node and Attributes
#define SCXML_STATE_STATE "state"
#define SCXML_STATE_ID "id"
#define SCXML_STATE_INITID "initial"
#define SCXML_STATE_PARALLEL "parallel"



//SCXML INVOKE Node And Attributes
#define SCXML_INVOKE "invoke"
#define SCXML_INVOKE_ID "id"
#define SCXML_INVOKE_TYPE "type"
#define SCXML_INVOKE_CONTENT "content"
#define SCXML_INVOKE_FINALIZE "finalize"


//SCXML SCXML Node and Attributes
#define SCXML_SCXML "scxml"
#define SCXML_SCXML_INITID "initial"
#define SCXML_SCMXL_ELEMNAME "ElementName"
//#define SCXML_SCMXL_TOOLVER "ToolVersion"

#define SCXML_SCMXL_TOOLVER "GTAVersion"


//SCXML CONSTANTS
#define SCXML_TRUE "true"
#define SCXML_FALSE "false"
#define SCXML_STATUS_PASS "success"
#define SCXML_STATUS_FAIL "fail"
#define SCXML_STATUS_TIMEOUT "Timeout"
#define SCXML_FINAL_STATE_ID "scxml_final"

//SCXML XMLRPC Node and Attributes.
#define SCXML_XMLRPC "xmlrpc"
#define SCXML_XMLRPC_TOOL_ID "tool_id"
#define SCXML_XMLRPC_TOOL_TYPE "tool_type"

#define SCXML_XMLRPC_FUNCTION "function"
#define SCXML_XMLRPC_FUNC_NAME "name"

//SCXML XMLRPC Function attributes and arguments.
#define SCXML_XMLRPC_FUNC_ARG "argument"
#define SCXML_XMLRPC_FUNC_ARG_NAME "name"
#define SCXML_XMLRPC_FUNC_ARG_TYPE "type"
#define SCXML_XMLRPC_FUNC_ARG_VALUE "value"

#define SCXML_XMLRPC_FUNC_RET "returnvalue"
#define SCXML_XMLRPC_FUNC_RET_NAME  "name"


//SCXML DATA Model Node and Attributes;
#define SCXML_DATA_MODEL "datamodel"
#define SCXML_DATA_MODEL_DATA "data"
#define SCXML_DATA_MODEL_DATA_ID "id"
#define SCXML_DATA_MODEL_DATA_EXPR "expr"

// SCXML CANCEL NODE and Attributes
#define SCXML_CANCEL "cancel"
#define SCXML_CANCEL_SENDID "sendid"


//META INFO Header Data
#define META_INFO_HEADER_LIST "HEADERS"
#define META_INFO_HEADER "HEADER"

class GTASCXML_SHARED_EXPORT GTASCXMLUtils
{

public:
    static void validateExpression(QString &oExpression);
};



#endif // GTASCXMLUTILS_H
