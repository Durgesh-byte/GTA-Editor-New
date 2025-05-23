#pragma once

#define MAJOR_TOOL_VERSION 31
#define MINOR_TOOL_VERSION 05
#define RELEASE_TOOL_VERSION 20
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define TOOL_VERSION_STR QString(STR(MAJOR_TOOL_VERSION)) + "." + QString(STR(MINOR_TOOL_VERSION)) + "." + QString(STR(RELEASE_TOOL_VERSION))
#define TOOL_VERSION_RESOURCE STR(MAJOR_TOOL_VERSION.MINOR_TOOL_VERSION.RELEASE_TOOL_VERSION)
