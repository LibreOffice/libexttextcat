#ifndef _TEXTCAT_PROPERTIES_H_
#define _TEXTCAT_PROPERTIES_H_

enum textcat_Property_s
{
    TCPROP_UTF8AWARE = 0,
    TCPROP_MINIMUM_DOCUMENT_SIZE = 1,
    TCPROP_LAST
};

enum textcat_Bool_s
{
    TC_FALSE = 0,
    TC_TRUE = 1
};

typedef enum textcat_Property_s textcat_Property;

#endif
