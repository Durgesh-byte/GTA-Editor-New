/*****************************************************************//**
 * \file   GTAUtil.h
 * \brief  
 * 
 * \author 
 * \date   September 2022
 *********************************************************************/


#ifndef GTAUTIL_H
#define GTAUTIL_H
#include "GTACommon.h"
#include "GTAVersion.h"

#pragma warning(push, 0)
#include <qcolor.h>
#pragma warning(pop)

#define GTA_DATABASE_VERSION_STR STR(MAJOR_DATABASE_VERSION)"."STR(MINOR_DATABASE_VERSION) 
#define MAJOR_DATABASE_VERSION 1
#define MINOR_DATABASE_VERSION 0

//SYSTEM directory names 
#define LTRA_TEMPLATES_DIR "LTRA_Templates"
#define MICD_DIR "MICD"
#define SCXML_TEMPLATE_DIR "SCXML_Templates"
#define EXTERNAL_DATA_DIR "EXTERNAL_DATA"
#define LOG_DIR "LOG"
#define TEMPLATE_DIR "TEMPLATE"
#define DATA_DIR "DATA"
#define SCXML_DIR "SCXML"
#define FWC_DIR "FWC"
#define PIR_DIR "PIR"
#define PMR_DIR "PMR"
#define ICD_DIR "AC_ICD"
#define MODEL_ICD_DIR "MODEL_ICD"
#define VCOM_DIR "VCOM"
#define BENCH_DB_DIR "BenchDatabase"
#define TABLES_DIR "TABLES"

#define PIC_DIR "Picture"
#define LIB_TOOL_DIR "Tools"
#define LIB_TOOL_CONFIG_DIR "ToolsConfig"
#define LIB_GENERIC_DB_DIR "GenericDatabase"
#define HDR_TEMPLATE_DIR "Header_Templates"
#define DB_TEMPLATE_DIR "DBTemplate"
#define APP_TOOL_DIR "Tool_Data"
#define USER_DATA_DIR "UserData"
#define HDR_TEMPLATE_FILE "HEADER_TEMPLATE.xml"
#define DATABASE_FILE "PARAMETER_DB"
#define DATABASE_TEMPLATE_FILE "PARAMETERS.csv"
#define PARAMETER_TABLE "PARAMETERS"
#define DATABASE_VERSION "VERSION"
#define EQUIPMENT_VIEW "EQUIPMENT_VIEW"
#define PARAM_HISTORY_TABLE "PARAMETER_HISTORY"
#define USER_DATA_DATABASE "USER_DATA_DB"
#define SESSION_TABLE "SESSION_TABLE"

#define LTRA_TEMPLATE_FILE_NAME "GTALTRATemplate.xsl"
#define LAUNCH_SCXMLEXE_FILE_NAME "LaunchScxml.bat"
#define KILL_SCXMLEXE_FILE_NAME "KillScxml.bat"
#define INTERNAL_PARAM_INFO_XML "InternalParameterInfo.xml"
#define COMMAND_PROPERTY_XML "commandProperties.xml"
#define EXPORT_ELEMENT_DIR "Export"
#define FAV_FILENAME "Fav.xml"
#define CONF_TIME_SCXML "checkConfirmation.scxml"
#define TIMEOUT_SCXML "confirmationTime.scxml"
#define GENESTA_SYMBOL_FILE "GenestaSymbolMap.txt"
#define EQUIPMENT_TOOL_MAP_FILE "Equipment_Tool_Map.xml"
#define SCXML_TOOL_ID_FILE "SCXML_Tool_Ids.xml"
#define TEMP_EQUIPMENT_QUERY_FILE "TempEquipmentQuery.txt"
#define GTA_SESSION_INFO "GTASessionInfo.xml"
#define CHECK_REMOTE_SERVER "checkServerPortAliveorDead.PS1"
#define RECORDING_TOOL_FILE "RecordingVideoTool.xml"

#define LIVE_MODE_CONFIG "Live_Mode_Config"

//Element types
#define ELEM_OBJ "Object"
#define ELEM_FUNC "Function"
#define ELEM_PROC "Procedure"
#define ELEM_SEQ "Sequence"
#define ELEM_TMPL "Template"

//Log Level Type
#define LOG_INFO "Info"
#define LOG_DEBUG "Debug"
#define LOG_ALL "All"

#define ACT_CALL "call"
//#define ACT_GEN_TOOL "Generic Tool"
#define ACT_CALL_EXT "call_expanded"
#define ACT_CALL_PROCS "Parallel Call"
//Define the constants for action command
#define ACT_SET "set"
#define ACT_SET_INSTANCE "set_instance"
#define ACT_SET_INNERCALL "Input For Inner Call"
//FSSM values
#define ACT_FSSSM_NO    "NO"
#define ACT_FSSSM_ND    "ND"
#define ACT_FSSSM_FW    "FW"
#define ACT_FSSSM_NCD   "NCD"
#define ACT_FSSSM_FT    "FT"
#define ACT_FSSSM_MINUS "MINUS"
#define ACT_FSSSM_PLUS  "PLUS"
#define ACT_FSSSM_LOST  "DEAD"
#define ACT_FSSSM_NOCHANGE  "NO_CHANGE"
#define ACT_FSSSM_NOT_LOST  "NOT_LOST"

#define ACT_SUBSCRIBE "Init PIR List"
#define ACT_UNSUBSCRIBE "Release PIR List"
#define ACT_REL "release"
#define ACT_PRINT "print"
#define ACT_WAIT "wait"
#define ACT_MANUAL "manual action"
#define ACT_CONDITION "condition"
#define ACT_IRT "IRT Object"
#define ACT_ROBO_ARM "Robotic arm"
#define ACT_DEFAULT_PRECISION "0"
#define ACT_PRECISION_UNIT_PERCENT "%"
#define ACT_PRECISION_UNIT_VALUE "value"
#define ACT_DEFAULT_TIMEOUT "3"
#define ACT_DEFAULT_CONFTIME "0"
#define ACT_TIMEOUT_UNIT_MSEC "msec"
#define ACT_TIMEOUT_UNIT_SEC "sec"
#define ACT_CONF_TIME_UNIT_SEC "sec"
#define ACT_CONF_TIME_UNIT_MSEC "msec"
#define ACT_TITLE "title"
#define ACT_TITLE_END "title end"
#define ACT_FAIL_STOP "stop"
#define ACT_FAIL_CONTINUE "continue"
#define ACT_INVALID "invalid"
//#define ACT_PRINT_TABLE "print table"
#define ACT_FAILURE "failure"


#define ACT_FCTL "fctl"
#define ACT_GEN_FUNC "Generic Command"
#define ACT_ONECLICK "One Click"
#define ACT_MATH "Math"
#define ACT_AVC "AVC Data"
#define ACT_GEN_TOOL "External Tool"
#define ACT_MATHS "Maths" //to be removed after new math is implemented

#define ACT_TEST_CMD "Test Command"
#define COM_TEST_1 "Complement 1"
#define COM_TEST_2 "Complement 2"

#define FAILURE_START   "Start"
#define FAILURE_STOP    "Stop" 
#define FAILURE_PAUSE   "Pause" 
#define FAILURE_RESUME "Resume" 

#define START_RECORD "start_record"
#define STOP_RECORD "stop_record"

// constants for check command
#define CHK_VALUE "Value"
#define CHK_FWC_WARN "FWC Warning"
#define CHK_FWC_PROC "FWC Procedure"
#define CHK_FWC_DISP "FWC Display"
#define CHK_BITE_MSG "BITE Message"
#define CHK_ECAM_DISP "ECAM Display"
#define CHK_AUDIO_ALRM "Audio Alarm"
#define CHK_VISU_DISP "Visual Display"
#define CHK_MANUAL_CHECK "Manual Check"
#define CHK_AUDIO_RECOG "Audio Recognition"
#define CHK_DEFAULT_PRECISION "0"
#define CHK_DEFAULT_TIMEOUT "3"
#define CHK_VAL_BINARY_OR "or"
#define CHK_REFRESH "Refresh"
#define CHK_DUMPLIST_SRCH "DUMPLIST_SEARCH"

//Define the constants for complements associated with action command
#define SET_VMACFORCE_SOL "value (solid+continuous)"
#define SET_VMACFORCE_LIQ "value (liquid+continuous)"


#define COM_FCTL_KINEMATIC_MULTI_OUTPUT "kinematic multi output"
#define COM_FCTL_CONF "conf"


#define COM_SUBSCRIBE_PARAM "parameter"
#define COM_UNSUBSCRIBE_PARAM "parameter"

#define COM_REL_PAR "VMAC parameter"
#define COM_PRINT_MSG "message"
#define COM_PRINT_PARAM "parameter value"
#define COM_PRINT_FWC_BUFF "FWC Buffer"
#define COM_PRINT_BITE_BUFF "BITE Buffer"
#define COM_PRINT_TIME "time"
#define COM_PRINT_TABLE "table"
#define COM_WAIT_FOR "for"
#define COM_WAIT_UNTIL "until"
#define COM_CONDITION_IF "if"
#define COM_CONDITION_ELSE "else"
#define COM_CONDITION_ENDIF "end if"
#define COM_CONDITION_WHILE "while"
#define COM_CONDITION_ENDWHILE "end while"
#define COM_CONDITION_FOR_EACH "for each"
#define COM_CONDITION_FOR_EACH_END "end for each"
#define COM_MATHS_BINARY "binary"
#define COM_MATHS_ABS "abs"
#define COM_MATHS_POW "pow"
#define COM_MATHS_INCREMENT "inc"
#define COM_CALL_OBJ "object"
#define COM_CALL_MON "parallel"
#define COM_CALL_FUNC "function"
#define COM_CALL_PROC "procedure"
#define COM_CONDITION_DO_WHILE "do while"
#define COM_CONDITION_DO_WHILE_END "do while end"


#define COM_ONECLICK_PINPROG "Setup PinProg"
#define COM_ONECLICK_TITLE "Title"
#define COM_ONECLICK_APP "Application"
#define COM_ONECLICK_ENV "Environment"




//#define ACT_ONECLICK_LAUNCHAPP "One Click - Application"
//#define COM_ONECLICK_SET "Set"//#define ACT_ONECLICK_SETENV "One Click - Environment"
//#define ACT_ONECLICK_PPC "One Click - Setup PinProg"
#define ACT_ONECLICK_SET "One Click - Set" //DISABLED 

//Define the constants for equation
#define ACT_ONECLICK_SET_INSTANCE "One Click - Set instance"

#define ACT_PRINT_TIME_TYPE_NA "Formatted"
#define ACT_PRINT_TIME_TYPE_NOW "UTC"
#define ACT_CONST_EQN_TIME_TYPE_UTC "CurrentTimeUTC"
#define ACT_CONST_EQN_TIME_TYPE_FORMATTED "CurrentTimeFormatted"
#define ACT_FUNC_RETURN "Return Function"

#define EQ_CONST "Constants"
#define EQ_GAIN "Gain"
#define EQ_RAMP "Ramp"
#define EQ_TRAPEZE "Trapeze"
#define EQ_CRENELS "Crenels"
#define EQ_SINECRV "Sine Curve"
#define EQ_TRIANGLE "Triangle"
#define EQ_SAWTOOTH "SawTooth"
#define EQ_SINUS "Sinus"
#define EQ_SQUARE "Square"
#define EQ_STEP "Step"
#define EQ_IDLE "IDLE"
#define EQ_EXP "EXPONENT"
#define EQ_NOISE "Noise"
#define EQ_STOP_TRIGGER "Stop Trigger"
#define FEQ_CONST "Const"
#define FEQ_RAMPSLOPE "Ramp Slope"
//serialize or deserialize
#define XNODE_MODEL "MODEL"
#define XNODE_CREATOR "CREATOR"
#define XNODE_DATE "DATE"
#define XNODE_PROGRAM  "PROGRAM"
#define XNODE_EQUIPMENT "EQUIPMENT"
#define XNODE_STANDARD "STANDARD"
#define XNODE_TYPE "TYPE"
#define XNODE_PATH "PATH"
#define XNODE_HEADER "HEADER"
#define XNODE_CONFIG "CONFIG"
#define XNODE_CONFIGS "CONFIGURATIONS"
#define XNODE_COMMANDS "COMMANDS"
#define XNODE_ACTION_MULTI "ACTION_LIST"
#define XNODE_ACTION "ACTION"
#define XNODE_SET "SET"
#define XNODE_SET_INSTANCE "SET_INSTANCE"
#define XNODE_CHECK "CHECK"
#define XNODE_TRUE "TRUE"
#define XNODE_FALSE "FALSE"
#define XNODE_NAME "NAME"
#define XNODE_MON_NAME "MON_NAME"
#define XNODE_COMPLEMENT "COMPLEMENT"
#define XNODE_ONFAIL "ONFAIL"
#define XNODE_PRECISION "PRECISION"
#define XNODE_PRECISION_UNIT "PRECISION_TYPE"
#define XNODE_USER_FEEDBACK "USER_FEEDBACK"
#define XNODE_CHK_AUDIO_ALARM "CHK_AUDIO_ALARM"
#define XNODE_CHK_FS_ONLY "CHK_FS_ONLY"
#define XNODE_CHK_VALUE_ONLY "CHK_VALUE_ONLY"
#define XNODE_CHK_SDI_ONLY "CHK_SDI_ONLY"
#define XNODE_CHK_PARITY_ONLY "CHK_PARITY_ONLY"
#define XNODE_CHK_REFRESH_RATE_ONLY "CHK_REFRESH_RATE_ONLY"
#define XNODE_LOOP_SAMPLING_STATUS "LOOP_SAMPLING_STATUS"
#define XNODE_LOOP_SAMPLING_VALUE "LOOP_SAMPLING_VALUE"
#define XNODE_LOOP_SAMPLING_UNIT "LOOP_SAMPLING_UNIT"
#define XNODE_TIMEOUT "TIMEOUT"
#define XNODE_TIMEOUT_UNIT "UNIT"
#define XNODE_ORDER "ORDER"
#define XNODE_PARAMETER "PARAMETER"
#define XNODE_STATEMENT "STATEMENT"
#define XNODE_OBJECT "OBJECT"
#define XNODE_FUNCTION "FUNCTION"
#define XNODE_CONTENTS "CONTENTS"
#define XNODE_PROCEDURE "PROCEDURE"
#define XNODE_SEQUENCE "SEQUENCE"
#define XNODE_TEMPLATE "TEMPLATE"
#define XNODE_OPERATOR "OPERATOR"
#define XNODE_PARAMETER1 "PARAMETER1"
#define XNODE_PARAMETER2 "PARAMETER2"
#define XNODE_LOCATION "LOCATION"
#define XNODE_FUNCTION_NAME "FUNCTION_NAME"
#define XNODE_FUNCTION_DISPLAY_NAME "FUNCTION_DISPLAY_NAME"
#define XNODE_FUNCTION_ACKNOWLEDGEMENT "WAIT_FOR_ACKNOWLEDGEMENT"
#define XNODE_ARG_HMI_LABEL "HMI_LABEL"
#define XNODE_ARG_CONST_VALS "CONST_VALS"
#define XNODE_ARG_DEF_VALS "DEFAULT_VALS"
#define XNODE_ARG_MANDATORY "MANDATORY"
#define XNODE_EXP_TXT "ExpressionText"
#define XNODE_TOOL_NAME "TOOL_NAME"
#define XNODE_EXEC_RESULT "EXECUTION_RESULT"
#define XNODE_EXEC_TIME "EXECUTION_TIME"
#define XNODE_RES_REF "RESULT_REFERENCES"
#define XNODE_LIST_PATH "LIST_PATH"
#define XNODE_LIST_RELATIVE_PATH "LIST_RELATIVE_PATH"
#define XNODE_RELATIVE_PATH_STATUS "RELATIVE_PATH_STATUS"
#define XNODE_FOR_DELIMITER_VAL "FOR_DELIMITER_VAL"
#define XNODE_FOR_DELIMITER_IDX "FOR_DELIMITER_IDX"
#define XNODE_FOR_OVERWRITE_RESULTS "FOR_OVERWRITE_RESULTS"

#define XNODE_FILE "FILE"
#define XNODE_CONDITION "CONDITION"
#define XNODE_ARG_SEARCH_TYPE "ARG_SEARCH_TYPE"
#define XNODE_ARG_SEARCH_PATH "ARG_SEARCH_PATH"
#define XNODE_ARG_USER_DB_NAME "ARG_USER_DB_NAME"
#define XNODE_IS_ARG_CONST "IS_ARG_CONST"
#define XNODE_REPORTON "REPORT"
#define XNODE_MESSAGETYPE "MESSAGETYPE"
//#define XNODE_WAIT_FOR_ACK "WAITFORACK"
#define XNODE_WAIT_BUFFER_DEPTH "WAITBUFFERDEPTH"
#define XNODE_FWC_WARN_HEADER "FWC_HEADER"
#define XNODE_FWC_WARN_COLOR "FWC_COLOR"
#define XNODE_DISPLAYON "DISPLAY"
#define XNODE_COMMENT "COMMENT"
#define XNODE_REQUIREMENTS "REQUIREMENTS"
#define XNODE_REQUIREMENT "REQUIREMENT"
#define XNODE_VAL "Val"
#define XNODE_IMAGE "IMAGE"
#define XNODE_ID "id"
#define XNODE_WAIT "WAIT"
#define XNODE_FREE_TEXT "FREE_TEXT"
#define MAIN_FINAL_STATE "main_final"
#define MAIN_FINAL_DONE "main_final_done"
#define XNODE_TESTRIG "TEST_RIG"
#define XNODE_TESTRIG_TYPE "TEST_RIG_TYPE"
#define XNODE_ONE_CLICK_APP "APP_NAME"
#define XNODE_ONE_CLICK_OPTIONS "OPTIONS"
#define XNODE_ONE_CLICK_DESCRIPTION "DESCRIPTION"
#define XNODE_ONE_CLICK_OPTION "OPTION"
#define XNODE_ONE_CLICK_OPTION_TYPE "TYPE"
#define XNODE_ONE_CLICK_OPTION_NAME "NAME"
#define XNODE_ONE_CLICK_OPTION_VALUE "VALUE"
#define XNODE_ONE_CLICK "ONE_CLICK"
#define XNODE_ONE_CLICK_DESCRIPTION_VALUE "VALUE"
#define XNODE_UUID "UUID"
#define XNODE_GTA_VER "GTA_VER"
#define XNODE_ELEMENT "ELEMENT"
#define XNODE_ITEM "ITEM"
#define XNODE_ERROR "ERROR"
#define XNODE_DUMPLIST "DUMP_LIST"
#define XNODE_DUMP_PARAM "DUMP_PARAMETER"
#define XNODE_ONE_CLICK_LAUNCHAPP_OPTS "OPTIONS"
#define XNODE_ONE_CLICK_LAUNCHAPP_OPT "OPTION"
#define XNODE_ONE_CLICK_LAUNCHAPP_ARG "ARGUMENT"
#define XNODE_ONE_CLICK_SEL_FOLDER "FOLDER"
#define XNODE_VIEW_PROPERTY "VIEW_PROPERTY"
#define XNODE_ARG "ARGUMENT"
#define XNODE_TOOL_TYPE "TOOL_TYPE"
#define XNODE_TOOL_ID "TOOL_ID"
#define XNODE_FUNCTIONS "FUNCTIONS"
#define XNODE_DEFINITIONS "DEFINITIONS"
#define XNODE_DATABASE "DATABASE"
#define XNODE_DATA "DATA"
#define XNODE_ATTRIBUTE "ATTRIBUTE"
#define XNODE_OCCURENCE "OCCURENCE"
#define XNODE_RETURN_CODE "RETURN_CODE"
#define XNODE_TRUE_COND "TRUE_COND"
#define XNODE_FALSE_COND "FALSE_COND"
#define XNODE_ATTR_WAIT_UNTIL "ARG_WAIT_UNTIL"
#define XNODE_ARG_DESCRIPTION "ARG_DESCRIPTION"
#define XNODE_FILE_SEARCH_TYPE "FILE_database"
#define XNODE_FOLDER_SEARCH_TYPE "FOLDER_database"
#define XNODE_USER_DEF_DB_SEARCH_TYPE "USER_database"
#define XNODE_AUTHOR_NAME "AUTHOR_NAME"
#define XNODE_VALIDATOR_NAME "VALIDATOR_NAME"
#define XNODE_VALIDATION_TIME "VALIDATION_TIME"
#define XNODE_VALIDATION_STATUS "VALIDATION_STATUS"
#define XNODE_VALIDATED "Validated"
#define XNODE_CREATED_DATE "CREATED_DATE"
#define XNODE_NOT_VALIDATED "Not Validated"
#define XNODE_MAX_TIME_ESTIMATED "MAX_TIME_ESTIMATED"
#define XNODE_MAX_TIME_ESTIMATED_UNIT "s"
#define XNODE_INPUT_CSV "INPUT_CSV"
#define XNODE_OUTPUT_CSV "OUTPUT_CSV"
#define XNODE_LAST_EDITED_ROW "LAST_EDITED_ROW"
#define XNODE_LAST_EDITED_COL "LAST_EDITED_COL"
#define XATTR_TMPL_NAME "TMPL_NAME"

//equation
#define XNODE_EQUATION              "EQUATION"
#define XNODE_FAILURE_PROFILE       "FAILPROFILE"
#define XATTR_EQ_RISETIME           "RiseTime"
#define XATTR_EQ_START              "Start"
#define XATTR_EQ_STOP               "Stop"
#define XATTR_EQ_TAU                "Tau"
#define XATTR_EQ_END                "end"
#define XATTR_EQ_IS_OFFSET          "IsOffset"
#define XATTR_EQ_BOOLMODE			"boolMode"
#define XATTR_EQ_PERIOD             "Period"
#define XATTR_EQ_FREQUENCY			"frequency"
#define XATTR_EQ_AMP                "Amplitude"
#define XATTR_EQ_MINVALUE           "minValue"
#define XATTR_EQ_MAXVALUE           "maxValue"
#define XATTR_EQ_STARTVALUE         "startValue"
#define XATTR_EQ_ENDVALUE           "endValue"
#define XATTR_EQ_OFFSET             "Offset"
#define XATTR_EQ_PHASE              "Phase"
#define XATTR_EQ_DUTYCYCLE          "DutyCycle"
#define XATTR_EQ_RAMPMODE           "RampMode"
#define XATTR_EQ_DIRECTION          "direction"
#define XATTR_EQ_VAL                "Value"
#define XATTR_EQ_GAIN               "gain"
#define XATTR_EQ_SQUARE              "pulse"
#define XATTR_EQ_TRIG_OP            "operator"
#define XATTR_EQ_MEAN               "mean"
#define XATTR_EQ_SEED               "seed"
#define XATTR_EQ_SIGMA              "sigma"
#define XATTR_EQ_TIMEOUT            "timeout"
#define XATTR_EQ_SLOPE              "slope"
#define XATTR_NAME "NAME"
#define XATTR_TYPE "TYPE"
#define XATTR_PATH "PATH"
#define XATTR_EQUIPMENT "EQUIPMENT"
#define XATTR_APP "APPLICATION"
#define XATTR_MANDATORY "MANDATORY"
#define XATTR_DESCRIPTION "DESCRIPTION"
#define XATTR_VAL "VALUE"
#define XATTR_PARALLEL "EXECUTE_PARALLEL"
#define XATTR_OPERATOR "OPERATOR"
#define XATTR_FS "FUNCTIONSTATUS"
#define XATTR_PRECISION_VAL "PRECISIONVALUE"
#define XATTR_PRECISION_TYPE "PRECISIONTYPE"
#define XATTR_ISONLYFS_SET_FIXED "IsSetOnlyFSFixed"
#define XATTR_ISONLYFS_SET_VARIABLE "IsSetOnlyFSVariable"
#define XATTR_ISONLYSDI_SET "IsSetOnlySDI"
#define XATTR_ISONLYVALUE_SET "IsSetOnlyValue"
#define XATTR_ISPARAMWITHOUTFS "IsParamWithoutFS"
#define XATTR_CHK_ONLY_VALUE "IsChkOnlyValue"
#define XATTR_CHK_FS "IsChkFS"
#define XATTR_CONF_TIME "CONFIRMATIONTIME"
#define XATTR_CONF_UNIT "CONFIRMATIONTIMEUNIT"
#define XATTR_ONE_CLICK_LAUNCHAPP_TESTRIG "TESTRIG"
#define XATTR_ONE_CLICK_LAUNCHAPP_TOOLID "TOOL_ID"
#define XATTR_ONE_CLICK_LAUNCHAPP_TOOLTYPE "TOOL_TYPE"
#define XATTR_ONE_CLICK_LAUNCHAPP_APPNAME "APP_NAME"
#define XATTR_ONE_CLICK_LAUNCHAPP_OPT "OPTION"
#define XATTR_ONE_CLICK_LAUNCHAPP_CONFFILE "CONFIG_FILE"
#define XATTR_ONE_CLICK_LAUNCHAPP_OPTIONNAME "NAME"
#define XATTR_ONE_CLICK_LAUNCHAPP_OPTIONVAL "VALUE"
#define XATTR_ONE_CLICK_LAUNCHAPP_KILL_MODE "KILL_MODE"
#define XATTR_ONELCICK_SETENV_START_ENV "START_ENV"
#define XATTR_ONELCICK_SETENV_STOP_ALTO "STOP_ALTO"
#define XATTR_ONELCICK_SETENV_STOP_TESTCONFIG "STOP_TESTCONFIG"
#define XATTR_TABLE_NAME "TABLE_NAME"
#define XATTR_CHK_STATUS "CHECK_STATUS"
#define XATTR_EPSILON1 "EPSILON1"
#define XATTR_EPSILON2 "EPSILON2"
#define XATTR_NO_OF_LINES "NO_OF_LINES"
#define XATTR_PILE_ORDER "WAIT_PILE_ORDER"
#define XATTR_OUTPUT_VAR "OUTPUT_VARIABLE"
#define XATTR_MAP_VAR "MAP_VARIABLE"
#define XATTR_INPUT_VAR "INPUT_VARIABLE"
#define XATTR_REF_PATH "REF_FILE_PATH"
#define XATTR_ORDER "order"
#define XATTR_DURATION "duration"
#define XATTR_REF_FILE_DATA "REF_DATA"
#define XATTR_TITLE "TITLE_NAME"
#define XATTR_FAIL_NAME "FAILURE_NAME"
#define XATTR_FAIL_TYPE "FAILURE_TYPE"
#define XATTR_CONFIG_LIST "CONFIGUARATION_LIST"
#define XATTR_REFRESH_RATE "REFRESH_RATE"
#define XATTR_HIDDEN "HIDE_STATUS"
#define XATTR_IGNORE_SCXML "IGNORE_IN_SCXML"
#define XATTR_READ_ONLY "READ_ONLY"
#define XATTR_BREAKPOINT "BREAKPOINT"
#define XATTR_TAGVALUE "TAG_VALUES"
#define XATTR_HIDDEN_ROWS "HIDDEN_ROW"
#define XATTR_REF_UUID "REF_UUID"

#define XATTR_INVOKE_DELAY "SCXML_INVOKE_DELAY"
#define XATTR_INVOKE_DELAY_UNIT "SCXML_INVOKE_DELAY_UNIT"

#define XATTR_SHOW_IN_LTRA "SHOW_IN_LTRA"

#define	XVALUE_FCTL_CONFCONFIGNAME	"ConfigName"
#define	XVALUE_FCTL_CONFLAW	        "Law"
#define	XVALUE_FCTL_CONFSFCONF	    "SFConf"
#define	XVALUE_FCTL_CONFGRNDFLIGHT	"GrndFlight"
#define	XVALUE_FCTL_CONFENGINES	    "Engines"
#define	XVALUE_FCTL_CONFLANDINGGEAR	"LandingGear"
#define	XVALUE_FCTL_CONFCG	        "CG"
#define	XVALUE_FCTL_CONFGROSSWT	    "GrossWt"
#define	XVALUE_FCTL_CONFVCAS	    "VCAS"
#define	XVALUE_FCTL_CONFALTITUDE	"Altitude"
#define	XVALUE_FCTL_CONFYHYD	    "YHYD"
#define	XVALUE_FCTL_CONFGHYD	    "GHYD"
#define	XVALUE_FCTL_CONFBHYD	    "BHYD"
#define	XVALUE_FCTL_CONFALPHA	    "Alpha"
#define	XVALUE_FCTL_CONFFCPC1	    "FCPC1"
#define	XVALUE_FCTL_CONFFCPC2	    "FCPC2"
#define	XVALUE_FCTL_CONFFCPC3	    "FCPC3"
#define	XVALUE_FCTL_CONFFCSC1	    "FCSC1"
#define	XVALUE_FCTL_CONFFCSC2	    "FCSC2"
#define	XVALUE_FCTL_CONFBCM	        "BCM"
#define	XVALUE_FCTL_CONFFMGEC1	    "FMGEC1"
#define	XVALUE_FCTL_CONFFMGEC2	    "FMGEC2"
#define	XVALUE_FCTL_CONFAP1	        "AP1"
#define	XVALUE_FCTL_CONFAP2	        "AP2"


//Manual Action
#define XNODE_MACHINE "MACHINE"
#define XNODE_TITLE "TITLE"
#define XNODE_MESSAGE "MESSAGE"
#define XNODE_ACKNOWLEDGEMENT "ACKNOWLEDGEMENT"
#define XNODE_RETURNTYPE "RETURN"
#define XNODE_FEEDBACK "FEEDBACK"
#define XNODE_ISLOCAL "ISLOCAL"
//condition
#define XNODE_IF "IF"
#define XNODE_ELSE "ELSE"
//set command
#define XNODE_VMACFORCETYPE "VMAC_FORCE_TYPE"
#define XATTR_VMACFORCETYPE "VMAC_FORCE_TYPE"
//init configuration constant
#define XNODE_INITCONF "INITCONFIG"
#define XNODE_VALUE "VALUE"
#define XNODE_USER_VALUE "USER_SELECTED"
#define XNODE_USER_SEL_INDEX "USER_SELECTED_INDEX"
#define XNODE_FS "FUNCTIONSTATUS"
#define XNODE_SDI "SDISTATUS"
#define XNODE_ALTSPEED "ALT/SPEED"
#define XNODE_ALTGROUND "ALT GROUND"
#define XNODE_SLATFLAP "SLAT/FLAP"
#define XNODE_ALTTARGET "ALT TARGET"
#define XNODE_SPEEDTARGET "SPEED TARGET"
#define XNODE_PARKBRAKES "PARK BRAKES"
#define XNODE_ATHR "ATHR"
#define XNODE_AUTOPILOT1 "AUTO PILOT 1"
#define XNODE_AUTOPILOT2 "AUTO PILOT 2"
#define XNODE_DISA "DISA"
#define XNODE_ACPOWERED "A/C POWERED"
#define XNODE_BAT "BAT 1+2"
#define XNODE_EXTPOWER "EXT POWER"
#define XNODE_APU "APU"
#define XNODE_APUGEN "APU GEN"
#define XNODE_APUGBLEED "APU BLEED"
#define XNODE_ENGINESTATE "ENIGINE STATE"
#define XNODE_ROTARYSEL "ROTARY SELECTOR"
#define XNODE_MASTERLEVEL "MASTER LEVEL 1+2"
#define XNODE_THROTTLESTATE "THROTTLE STATE"
#define XNODE_HEARDAFTER "HEARDAFTER"
#define XNODE_FUNC_STATUS "FUNCTIONAL_STATUS"
#define XNODE_SDI_STATUS "SDI_VALUE"
#define XNODE_PARITY_STATUS "PARITY_VALUE"
#define XNODE_ARG_COLOR "COLOR"
#define XNODE_ARG_BACKCOLOR "BACKCOLOR"

//Tag Variables
#define XNODE_TAGVARIABLES "TAGVARIABLES"
#define XNODE_TAGVARIABLE "TAG_VARIABLE"
#define XATTR_UNIT "UNIT"
#define XATTR_MIN "MIN"
#define XATTR_MAX "MAX"

//init config
#define INIT_CONF1 "A/C ground, not powered(dark cockpit)"
#define INIT_CONF2 "A/C ground, engine off, A/C powered"
#define INIT_CONF3 "A/C ground, engine running"
#define INIT_CONF4 "A/C in flight"

//scxml
#define SXN_PARALLEL "parallel"
#define SXN_SCXML "scxml"
#define SXA_INITIALSTATE "initial"
#define SXN_DATAMODEL "datamodel"
#define SXN_DATA "data"
#define SXN_STATE "state"
#define SXN_ONENTRY  "onentry"
#define SXN_ONEXIT "onexit"
#define SXN_SEND "send"
#define SXA_ID "id"
#define SXN_TRANSITION "transition"
#define SXA_TARGET "target"
#define SXA_EVENT "event"
#define SXN_INVOKE "invoke"
#define SXN_CONTENT "content"
#define SXN_XMLRPC "xmlrpc"
#define SXA_TOOLTYPE "tool_type"
#define SXA_TOOLID "tool_id"
#define SXN_FUNCTION "function"
#define SXA_NAME "name"
#define SXN_ARGUMENT "argument"
#define SXA_VALUE "value"
#define SXA_VERSION "version"
#define SXN_FINALIZE "finalize"
#define SXN_FINAL "final"
#define SXN_LOG "log"
#define SXA_EXPR "expr"
#define SXN_RETURNVALUE "returnvalue"
#define SXN_ASSIGN "assign"
#define SXA_LOCATION "location"
#define SXA_CONDITION "cond"
#define SXA_LABEL "label"
#define SXA_DELAY "delay"
#define SXA_STATUS "status"
#define SXA_TYPE "type"
#define SXA_ELEMENT_NAME "ElementName"

#define SX_INVOKE_DEFAULT_DELAY "100"
#define SX_INVOKE_DEFAULT_DELAY_UNIT "ms"
//scxml log Flag
#define SX_FLAG_EXECUTED "Executed"
#define SX_FLAG_LOGGED "Logged"
#define SX_FLAG_TBE "TBE"

#define SX_RET_PARAM_NAME "Return_Parameter_Structure"

//scxml channel in control flag
# define CHANNEL_IN_CONTROL_FLAG  "ChannelInControlFlag"


//Arithmatic operator
#define ARITH_EQ "="
#define ARITH_NOTEQ "NOT="
#define ARITH_LTEQ "<="
#define ARITH_GTEQ ">="
#define ARITH_GT ">"
#define ARITH_LT "<"

//checking FS only
#define FS_ONLY "check FS only"


//Define Theme Parameters
#define THEME_BACKGROUND "background"
#define THEME_WINDOWTEXT "windowText"
#define THEME_BUTTON "buttonText"
#define THEME_USERTEXT "userText"

#define TAG_IDENTIFIER '@'

#define INTERNAL_PARAM_SEP "__"
#define INTERNAL_PARAM "INT_PARAM"

#define CHECK_BITE_MESSAGE_TYPE "Maintenance Message;MCDU Display"


/*! Global background color of IRISV3
  QColor irsBlueGray = QColor( 173, 181, 210 );
*/
#define GLOBAL_BACK_COLOR  QColor( 173, 181, 210 ) //QColor( 200, 202, 211 )

/*! Global background color of listview, table.....
  QColor irsLightGray = QColor( 215, 217, 223 );
*/
#define GLOBAL_INSIDE_COLOR  QColor( 230, 232, 238 )

/*! Global color of items selected (in listview, table, combobox.....)
  QColor irsDarkBlue = QColor( 49, 62, 111 );
*/
#define GLOBAL_SELECTED_COLOR  QColor( 49, 62, 111 )

/*! Global color of items selected but not active (in listview, table, combobox.....)
   QColor irsLightBlue = QColor( 113, 133, 206 );
*/
#define GLOBAL_UNSELECTED_COLOR  QColor( 113, 133, 206 )

//! orange color used for tree view selection item
#define SELECTION_HIGHLIGHT_COLOR QColor( 255, 125, 0 )

//! yellow color used in compare session, and attribute edition
#define GLOBAL_LIGHT_YELLOW_COLOR QColor( 253, 242, 168 )

//! Background color of the Text editor window
#define TEXT_EDITOR_BACK_COLOR  QColor( 205, 208, 228 )

//! Background color of the Title
#define TITLE_EDITOR_BACK_COLOR  QColor( 0, 128, 192, 180 )

//! Foreground color of the Title
#define TITLE_EDITOR_FORE_COLOR  QColor( 255, 255, 255 )

//! Background color of the Attribute Report window
#define ATTR_REPORT_BACK_COLOR  QColor( 206, 218, 214 )

//! Background color of group in the Attribute Report window
#define ATTR_REPORT_GROUP_COLOR  QColor( 208, 230, 255 )

//! Background color of group in the Attribute Report window
#define ATTR_REPORT_BACK_LIST_COLOR  QColor( 242, 243, 246 )

//! Background color of the compare session window
#define COMPARE_SESS_BACK_COLOR  QColor( 209, 212, 208 )

//! Background color of the Risks analysis window
#define RISKS_BACK_COLOR  QColor( 215, 205, 212 )

//! Background color of the gradien of the 3D viewer
#define VIEWER_BACK_COLOR_1  QColor( 49, 62, 111 )
#define VIEWER_BACK_COLOR_2  QColor( 113, 133, 206 )


//migration from Genesta to GTA
#define GENESTA_GAIN_EQN_IDENTIFIER "(value*gain) with gain"
#define GENESTA_RAMP_EQN_IDENTIFIER "(ramp*time+offset) with"
#define GENESTA_TRAPEZE_EQN_IDENTIFIER "trapeze"
#define GENESTA_CRENELS_EQN_IDENTIFIER "crenels"
#define GENESTA_SINE_EQN_IDENTIFIER "sine"
#define GENESTA_CRENELS_EQN_ARG_SIZE 9
#define GENESTA_TRAPEZE_EQN_ARG_SIZE 7
#define GTA_RAMP_DEF_RISE_TIME "10"
#define GTA_RAMP_DEF_END_TIME "1000"
#define GTA_CIC_IDENTIFIER "`"
#define GTA_MCDU_SEND "MCDU_REQUEST"
#define GTA_MCDU_IDENTIFIER "MCDU"
#define GENESTA_OBJ_BEGIN_IDENTIFIER "Object code:"
#define GENESTA_PROC_BEGIN_IDENTIFIER "Procedure code:"
#define GENESTA_CHANNEL_VAR_IDENTIFIER1 "×"
#define GENESTA_CHANNEL_VAR_IDENTIFIER2 "₧"

#define GENESTA_TITLE_IDENTIFIER "add title -"
#define GTA_CHANNEL_VARIABLE "Channel"
#define GENESTA_VAR1_IDENTIFIER QString("¤")
#define GENESTA_VAR2_IDENTIFIER QString("╧")
#define GTA_VAR1_VARIABLE "VAR1"
#define GTA_VAR2_VARIABLE "VAR1"
#define GENESTA_ENGINE_VAR_IDENTIFIER1 "Ø"
#define GENESTA_ENGINE_VAR_IDENTIFIER2 "+"
#define GENESTA_ENGINE_VAR_IDENTIFIER3 "¥"
#define GTA_ENGINE_VARIABLE "Engine"
#define GENESTA_COLON_IDENTIFIER "‡"
#define GENESTA_CONDITION_SEPERATOR_IDENTIFIER "†"
#define GENESTA_DELIMITER ":"
#define GENESTA_PROC_COLSIZE 8
#define GENESTA_OBJ_COLSIZE 7
#define GENESTA_ACT_MSG_SEP "-"

#define GENESTA_IMPORT_CHANNEL_SUBSTITUTE "@CH@"
#define GENESTA_IMPORT_VAR_SUBSTITUTE "@VA@"
#define GENESTA_IMPORT_ENGIN_SUBSTITUTE "@EN@"

#define GENESTA_CMD_SET "set value"
#define GENESTA_CMD_SET_SSM "Set SSM"
#define GENESTA_CMD_IF "if"
#define GENESTA_CMD_WHILE "while"
#define GENESTA_CMD_WAIT "wait"
#define GENESTA_CMD_PRINT "print value"
#define GENESTA_CMD_CALL "call object"
#define GENESTA_CMD_RELEASE "release"
#define GENESTA_CMD_MANUAL_ACT "manual action"
#define GENESTA_CMD_CHECK_FWC_WARNING "check FWC Warning"
#define GENESTA_CMD_CHECK_BITE_MESSAGE "check BITE Message"
#define GENESTA_CMD_CHECK_VAL "check Value"
#define GENESTA_CMD_CHECK_SSM "Check SSM"
#define GENESTA_CMD_CHECK_FWC_PROC  "check FWC Procedure"
#define GENESTA_CMD_CHECK_FWC_DISP  "check FWC Display"
#define GENESTA_CMD_CHECK_ECAM_DISP "check ECAM Display"
#define GENESTA_CMD_CHECK_VIZ_PROC  "check Visual Display"
#define GENESTA_CMD_CHECK_MANUAL    "Check that"
#define GENESTA_CMD_DUMP_SEPERATOR    ","
#define GENESTA_PRINT_TABLE_IDENTIFIER "PRINT_TABLE"

#define  SVN_WC_STATUS_MISSING     "missing"
#define  SVN_WC_STATUS_ADDED       "added"
#define  SVN_WC_STATUS_CONFLICTED  "conflicted"
#define  SVN_WC_STATUS_DELETED     "deleted"
#define  SVN_WC_STATUS_IGNORED     "ignored"
#define  SVN_WC_STATUS_MODIFIED    "modified"
#define  SVN_WC_STATUS_REPLACED    "replaced"
#define  SVN_WC_STATUS_EXTERNAL    "external"
#define  SVN_WC_STATUS_UNVERSIONED "unversioned"
#define  SVN_WC_STATUS_INCOMPLETE  "incomplete"
#define  SVN_WC_STATUS_OBSTRUCTED "obstructed"
#define  SVN_WC_STATUS_NORMAL     "normal"
#define  SVN_WC_STATUS_NONE        "none"

#define SVN_REMOTE_STATUS_ADDED    "added"
#define SVN_REMOTE_STATUS_DELETED  "deleted"
#define SVN_REMOTE_STATUS_MODIFIED "modified"
#define SVN_REMOTE_STATUS_REPLACED "replaced"
#define SVN_REMOTE_STATUS_NONE     "none"

#define SVN_WC_STATUS          "wc-status"
#define SVN_REMOTE_STATUS      "repos-status"
#define SVN_ITEM_ATTR          "item"


#define SVN_COMMIT_TYPE_NODE       "commit"
#define SVN_COMMIT_TYPE_AUTHOR     "author"

#define SVN_LOCK_TYPE_NODE         "lock"
#define SVN_LOCK_TYPE_OWNER        "owner"
#define SVN_LOCK_TYPE_COMMENT      "comment"

#define CHANNEL_IN_CONTROL "CIC"
#define CHANNEL_NOT_IN_CONTROL "CNIC"
#define BOTH_CHANNEL "both"

#define MATH_FUN_EXP    "exp"
#define MATH_FUN_ABS    "abs"
#define MATH_FUN_SQRT   "sqrt"
#define MATH_FUN_SIN    "sin"
#define MATH_FUN_COS    "cos"
#define MATH_FUN_TAN    "tan"
#define MATH_FUN_ASIN   "asin"
#define MATH_FUN_ACOS   "acos"
#define MATH_FUN_ATAN   "atan"
#define MATH_FUN_SINH   "sinh"
#define MATH_FUN_COSH   "cosh"
#define MATH_FUN_TANH   "tanh"
#define MATH_FUN_LOG10  "log10"
#define MATH_FUN_LOG2   "log2"
#define MATH_FUN_LOGE   "ln"
#define MATH_FUN_LOG1P  "log1p"
#define MATH_FUN_CEIL   "ceil"
#define MATH_FUN_RAND   "random"
#define MATH_FUNC_RAND_BRACES "random( )"
#define MATH_FUN_CBRT   "cbrt"
#define MATH_FUN_FLOOR  "floor"
#define MATH_FUN_ROUND  "round"
#define MATH_FUN_MAX  "max"
#define MATH_FUN_MIN  "min"
#define MATH_FUN_POW  "pow"

#define PARAM_TYPE_LOCAL    "LOCAL"
#define PARAM_TYPE_DIS      "DIS"
#define PARAM_TYPE_VMAC     "VMAC"
#define PARAM_TYPE_VCOM     "VCOM"
#define PARAM_TYPE_PIR      "PIR"
#define PARAM_TYPE_CAN      "CAN"
#define PARAM_TYPE_ANA      "ANA"
#define PARAM_TYPE_MODEL    "MODEL"
#define PARAM_TYPE_AFDX     "AFDX"


#define DB_NAME "NAME"
#define DB_UPPER_NAME "UPPERNAME"
#define DB_PARAM_TYPE "PARAMTYPE"
#define DB_SIGNAL_TYPE "SIGNALTYPE"
#define DB_UNIT "UNIT"
#define DB_VALUE_TYPE "VALUETYPE"
#define DB_MNVAL "MNVAL"
#define DB_MXVAL "MXVAL"
#define DB_LABEL "LABEL"
#define DB_PARAMETER_LOCAL_NAME "PARAMETER_LOCAL_NAME"
#define DB_BIT_POS "BIT_POS"
#define DB_EQUIPNAME "EQUIPNAME"
#define DB_FILE "FILE"
#define DB_FILE_LAST_MODIFIED "FILELASTMODIFIED"
#define DB_MESSAGE "MESSAGE"
#define DB_FS_NAME "FS_NAME"
#define DB_SUFFIX "SUFFIX"
#define DB_PRECISION "PRECISION"
#define DB_DIRECTION "DIRECTION"
#define DB_MEDIA_TYPE "MEDIA_TYPE"
#define DB_TOOL_NAME "TOOL_NAME"
#define DB_TOOL_TYPE "TOOL_TYPE"
#define DB_REFRESH_RATE "REFRESH_RATE"

#define DB_NAME_COL_ID 0
#define DB_PARAM_TYPE_COL_ID 1
#define DB_SIGNAL_TYPE_COL_ID 2
#define DB_UNIT_COL_ID 3
#define DB_VALUE_TYPE_COL_ID 4
#define DB_MNVAL_COL_ID 5
#define DB_MXVAL_COL_ID 6
#define DB_LABEL_COL_ID 7
//#define DB_IDENTIFIER_COL_ID 8
#define DB_PARAMETER_LOCAL_NAME_COL_ID 8
#define DB_BIT_POS_COL_ID 9
#define DB_EQUIPNAME_COL_ID 10
#define DB_FILE_COL_ID 11
#define DB_FILE_LAST_MODIFIED_COL_ID 12
#define DB_MESSAGE_COL_ID 13
#define DB_FS_NAME_COL_ID 14
#define DB_SUFFIX_COL_ID 15
#define DB_PRECISON_COL_ID 16
#define DB_DIRECTION_COL_ID 17
#define DB_MEDIA_TYPE_COL_ID 18
#define DB_TOOL_NAME_COL_ID 19
#define DB_TOOL_TYPE_COL_ID 20
#define DB_REFRESH_RATE_COL_ID 21

/*#define DB_NAME_COL_ID 0
#define DB_PARAM_TYPE_COL_ID 1
#define DB_SIGNAL_TYPE_COL_ID 2
#define DB_UNIT_COL_ID 3
#define DB_VALUE_TYPE_COL_ID 4
#define DB_MNVAL_COL_ID 5
#define DB_MXVAL_COL_ID 6
#define DB_LABEL_COL_ID 7
#define DB_IDENTIFIER_COL_ID 8
#define DB_BIT_POS_COL_ID 9
#define DB_EQUIPNAME_COL_ID 10
#define DB_FILE_COL_ID 11
#define DB_FILE_LAST_MODIFIED_COL_ID 12
#define DB_FILE_TYPE_COL_ID 13
#define DB_APPLICATION_COL_ID 14
#define DB_BUSNAME_COL_ID 15
#define DB_SIGNAL_NAME_COL_ID 16
#define DB_MEDIA_COL_ID 17
#define DB_MESSAGE_COL_ID 18
#define DB_PARAMETER_LOCAL_NAME_COL_ID 19
#define DB_FS_NAME_COL_ID 20
#define DB_SUFFIX_COL_ID 21
#define DB_PRECISON_COL_ID 22
#define DB_TEMP_PARAM_NAME_COL_ID 23
#define DB_DIRECTION_COL_ID 24
#define DB_MEDIA_TYPE_COL_ID 25
#define DB_TOOL_NAME_COL_ID 26
#define DB_TOOL_TYPE_COL_ID 27
#define DB_REFRESH_RATE_COL_ID 28*/

// V26 editor view column enhancement #322480 ------>
#define COLUMN_NAME 0
#define COLUMN_DESCRIPTION 1
#define COLUMN_FAVOURITES 10
#define COLUMN_UUID 3
#define COLUMN_CREATED_DATE 4
#define COLUMN_AUTHOR 5
#define COLUMN_MODIFIED_DATE 6
#define COLUMN_VALIDATION_STATUS 7
#define COLUMN_VALIDATED_BY 8
#define COLUMN_VALIDATION_DATE 9
#define COLUMN_GTA_VERSION 2
#define COLUMN_GIT_STATUS 11


#define DV_NAME_COL "Name"
#define DV_DESCRIPTION_COL "Description"
#define DV_FAVOURITES_COL "Favourite"
#define DV_UUID_COL "UUID"
#define DV_CREATED_DATE_COL "Date Created"
#define DV_AUTHOR_COL "Author"
#define DV_MODIFIED_DATE_COL "Last Modified"
#define DV_VALIDATION_STATUS_COL "Validation Status"
#define DV_VALIDATED_BY_COL "Validator"
#define DV_VALIDATION_DATE_COL "Date Validated"
#define DV_GTA_VERSION_COL "GTA Version"
#define DV_DB_VERSION_COL "DB_Version"
#define DV_GIT_STATUS_COL = "Git status"

// <-------V26 editor view column enhancement #322480

//"AFDX"<<"A429"<<"CAN"<<"" <<"" << "" << "PIR"<<""<<"";

//plugin macros
#define GENESTA_STR "Genesta"

#define EQ_CURRENT_VALUE "Current Value"

#define USCXML_SCHEDULER "USCXML"
#define S2I_SCHEDULER "S2I"

//GTA Live Mode RPC Calls for BISG
#define SUBSCRIBE_PARAM_LIST "SubscribeParamList"
#define GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE "GetMultiParamValueSingleSample"
#define GET_PARAM_VALUE_SINGLE_SAMPLE "GetParamValueSingleSample"
#define SET_PARAM "SetParam"
#define RELEASE_PARAM "ReleaseParam"
#define GET_PARAM_SDI "GetParamSDI"
#define UNSUBSCRIBE "UnSubscribe"
#define UNSUBSCRIBE_PARAMLIST "UnSubscribeParamList"
#define GET_ALL_PARAM_VALUE_SINGLE_SAMPLE "GetAllParamValueSingleSample"
#define BLOCKED_CALL "BlockedCall_GTA"
#define DISPLAY_MESSAGE "displayMessage"
#define GET_PARAM_VALUE_SINGLE_SAMPLE_MANACT "getParamValueSingleSample"

#define SCRIPT_VERSION_TOOLS "GTAToolVersion.exe"
#define FILE_VERSIONS_TOOLS_BENCH "fileVersionsToolsBench.txt"


enum class GitStatus {
    Unknown,
    Current,
    IndexNew,
    IndexModified,
    IndexDeleted,
    IndexRenamed,
    IndexTypechange,
    WtNew,
    WtModified,
    WtDeleted,
    WtTypechange,
    WtRenamed,
    WtUnreadable,
    Ignored,
    Conflicted,
    NotFound
};

namespace cv
{
class Mat;
}

class GTACommon_SHARED_EXPORT GTAUtil
{

public:
    enum ElementType{OBJECT,FUNCTION,PROCEDURE,SEQUENCE,TEMPLATE,IMAGE,PARAMETER,UNKNOWN};

    GTAUtil();



    //    static encode;// = { "", "==", "=" };

    //cv::Mat* p;
    static bool encodeImageFile(const QString& iFileName, QString& oEncodedImage);
    static bool decodeImage(const QString& imageRep, QString& oDecodedImage);
    static bool encodeImageFile(const QString& iFileName, QByteArray& oEncodedImage);
    static bool decodeImage(const QString& imageRep, QByteArray& oDecodedImage);
    static bool writeImage(const QString& imageRep, const QString& iFilePath);
    static QString getListOfForbiddenChars();

    static void clearDirectory(const QString& iFolderPath);
    static QHash<QString,QString> getNomenclatureOfArithSymbols();
    static QStringList getParamChannelIdentifiers();
    static QStringList getParamEngIdentifiers();

    static QList<QRegExp> getParameterEngineIdentifiers();
    static QList<QRegExp> getParameterChannelIdentifiers();

    static QStringList getProcessedParameterForChannel(const QString& iParameter,bool bTreatCICAsBoth=false, bool bTreatCNICAsBoth=false);
    static void getChannelSelectionVariable(QString& ioParameter,QStringList& oChvar,QStringList& oChVal);
    static QStringList getChannelSelectionSignals(const QString &iEngine,QHash<QString,QString> &oEngineVarMap, const QString &iChannelID="A");

/**
  * This function provides a custom element grouping by managing Calls under titles for multiple logs
  * @return: List of all engines available
  */
    static QStringList getAvailableEngines();
    static bool copyFilesRecursively(QString sSourcePath, QString destPath ,bool iReplaceFile = false);
    static QString getNewInstanceName(const QString &iInstanceName);
    static bool CompressDirectory(const QString &iOutDirectory,const QString &iDirectoryToCompress);

    static bool resolveChannelParam(const QString &iParamName, QStringList &oResolvedParams);
    static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
    {
        return s1.toLower() < s2.toLower();
    }

    static void setRepositoryPath(const QString &iLibPath);
    static QString getRepositoryPath();
    static void updateWithGenToolParam(QString &ioParam,QHash<QString, QString> ioGenToolReturnMap);
    static void setSchedulerType(bool isUSCXML);
    static bool isUSCXMLScheduler();

    /**
      * This function sets the Tool Data Path
      * @iToolDataPath: tool data path(If path exists in settings ini file else the default Tool_Data folder path in application directory.
      */
    static void setToolDataPath(const QString &iToolDataPath);

    /**
      * This function returns the Tool Data Path
      * @return: Tool data path (if path exists in settings ini file else the default Tool_Data folder path in application directory)
      */
    static QString getToolDataPath();
    static QString getGlobalPathExportScxml();
    static void setGlobalPathExportScxml(QString& _path);


    static double getMinTimeOutValue(QString timeOutUnit = ACT_TIMEOUT_UNIT_SEC, QString confTimeUnit = ACT_CONF_TIME_UNIT_SEC, QString confTime = ACT_DEFAULT_CONFTIME, double delta = 3);

    static QColor getTextColor(const QString& textColor);
    static QColor getBackGroundColorCellule(const QString& fontColor);

private:
    static QString _LibraryPath;
    static QString _SchedulerType;
    static QString _ToolDataPath;
    static QString _memExportPath; //chosen ExportSCXML path by the User
};

#endif // GTAUTIL_H
