/*
 * RestAPITest.c
 *
 *  Created on: 15 oct. 2017
 *      Author: flore
 */
   #include <stddef.h>
   #include <stdio.h>
   #include <stdlib.h>
#include "APITypes.h"
#include "json.h"
#include "IPAddress.h"
#include "string.h"
#include "Serial.h"
#include "Printfg.h"

APIShares Shares;



bool intToString(const char* string, void* decodedValue){


}

bool intFromString(const char* string, void* valueToEncode){

}


bool floatToString(const char* string, void* decodedValue){


}

bool floatFromString(const char* string, void* valueToEncode){

}

bool stringToString(const char* string, void* decodedValue){


}

bool stringFromString(const char* string, void* valueToEncode){

}

bool boolToString(const char* string, void* decodedValue){


}

bool boolFromString(const char* string, void* valueToEncode){

}

bool IPAddressToString(const char* string, void* decodedValue){


}

bool IPAddressFromString(const char* string, void* valueToEncode){

}



void StructToJSON(APIShare* share){

}


#define AUTOCREATE_PRIMITIF_DESCRIPTOR(name) APIPrimitifDescriptor name ## Descriptor = {#name, name ## ToString, name ## FromString};

#define string char*

AUTOCREATE_PRIMITIF_DESCRIPTOR(int)
AUTOCREATE_PRIMITIF_DESCRIPTOR(float)
AUTOCREATE_PRIMITIF_DESCRIPTOR(bool)
AUTOCREATE_PRIMITIF_DESCRIPTOR(string)
AUTOCREATE_PRIMITIF_DESCRIPTOR(IPAddress)

/*
typedef struct{
    char* DeviceName;
    bool UseDHCP;
    IPAddress IP;
    int WebServerPort;
}NetworkConfigurationStruct;
*/

#define AUTOCREATE_TYPE_INSTANCE(STRUCT_NAME, X_DEFINE) typedef struct { \
    X_DEFINE \
    } STRUCT_NAME ## Struct; \
    STRUCT_NAME ## Struct STRUCT_NAME;


#define X_NetworkConfiguration \
    X(string, DeviceName, NetworkConfiguration) \
    X(bool, UseDHCP, NetworkConfiguration) \
    X(IPAddress, IP, NetworkConfiguration) \
    X(int, WebServerPort, NetworkConfiguration)


#define X(type, member, ...) type member;

AUTOCREATE_TYPE_INSTANCE(NetworkConfiguration, X_NetworkConfiguration)

#undef X


#define AUTOCREATE_STRUCT_SHARE(STRUCT_NAME, X_DEFINE) \
    int STRUCT_NAME ## FieldCount = 0;\
    X_DEFINE \
    APIFieldDescriptor STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount]; \
    APIStructDescriptor STRUCT_NAME ## Descriptor; \
    STRUCT_NAME ## Descriptor.name = #STRUCT_NAME  "Struct"; \
    STRUCT_NAME ## Descriptor.count = STRUCT_NAME ## FieldCount; \
    STRUCT_NAME ## Descriptor.fields = STRUCT_NAME ## Fields; \
    Shares.item[Shares.count].name = #STRUCT_NAME; \
    Shares.item[Shares.count].instance = & STRUCT_NAME; \
    Shares.item[Shares.count].descriptor = & STRUCT_NAME ## Descriptor; \
    Shares.count ++; \
    STRUCT_NAME ## FieldCount = 0; // remettre à 0 pour la déclaration des types



void InitRestAPITests(void){
    Shares.count = 0;

    NetworkConfiguration.DeviceName = "Delta";
    NetworkConfiguration.IP = (IPAddress){{192,168,0,43}};
    NetworkConfiguration.UseDHCP = false;
    NetworkConfiguration.WebServerPort = 80;


#define X(type, member, STRUCT_NAME) STRUCT_NAME ## FieldCount++;

    AUTOCREATE_STRUCT_SHARE(NetworkConfiguration, X_NetworkConfiguration)

#undef X


#define X(type, member, STRUCT_NAME) \
    STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount].typeKind = FieldTypeKindPrimitif; \
    STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount].Type.Primitif = & type ## Descriptor; \
    STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount].countKind = FieldCountKindNone; \
    STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount].name = #member; \
    STRUCT_NAME ## Fields[STRUCT_NAME ## FieldCount].offset = offsetof(STRUCT_NAME ## Struct, member) ; \
    STRUCT_NAME ## FieldCount++;


#define STRUCT_NAME NetworkConfiguration
    X_NetworkConfiguration
#undef STRUCT_NAME


#undef X

/*

StructToJSON(Shares.item[0]);

*/
}






void LoopRestAPITests(void){


}
