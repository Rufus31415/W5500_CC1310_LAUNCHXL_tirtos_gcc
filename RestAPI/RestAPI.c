#include "APITypes.h"

/*
APIShares Shares;

bool intToString(const char* string, void* decodedValue){


}

bool intFromString(const char* string, void* valueToEncode){

}


bool floatToString(const char* string, void* decodedValue){


}

bool floatFromString(const char* string, void* valueToEncode){

}



APITPrimitifDescriptor intDescriptor = {"int", intToString, intFromString};
APITPrimitifDescriptor floatDescriptor = {"float", floatToString, floatFromString};

typedef struct{
    int aa;
    int bb;
}MyStruct2;

typedef struct {
    int a;
    float b;
    MyStruct2 struct2;
    int* ptra;
    float arrayFloat[10];
    int str2Nb;
    MyStruct2 arrayStruct[20];
} MyStruct;

MyStruct myInstance;

void InitAPI(){
    APIFieldDescriptor MStructFields[7];

    MStructFields[0].typeKind = Primitif;
    MStructFields[0].Type.Primitif = &intDescriptor;
    MStructFields[0].countField = 0;
    MStructFields[0].name = "a";
    MStructFields[0].offset =  0;
//    MStructFields[0].size = -1;

    APIStructDescriptor MyStructDescriptor;
    MyStructDescriptor.name = "MyStructDescriptor";
    MyStructDescriptor.count = 7;
    MyStructDescriptor.fields = MStructFields;

    Shares.item[Shares.count].name = "myInstance";
    Shares.item[Shares.count].instance = & myInstance;
    Shares.item[Shares.count].descriptor = &MyStructDescriptor;
}*/
