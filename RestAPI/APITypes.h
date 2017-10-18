#ifndef APITypes_h
#define APITypes_h

#define bool    int
#define true    1
#define false   0
#define APISHARES_MAX   10


typedef bool(*FromStringFunction)(const char* string, void* decodedValue);
typedef bool(*ToStringFunction)(const char* string, void* valueToEncode);

typedef struct{
    char* name;
    FromStringFunction fromString;
    ToStringFunction toString;
} APIPrimitifDescriptor;

typedef struct APIStructDescriptor_ APIStructDescriptor;
typedef struct APIFieldDescriptor_ APIFieldDescriptor;

typedef enum {
    FieldTypeKindPrimitif,
    FieldTypeKindStructure
}FieldTypeKind;

typedef union {
    APIPrimitifDescriptor* Primitif;
    APIStructDescriptor* Structure;
} FieldType;


typedef enum {
    FieldCountKindNone, // il s'agit d'une valeur donc pas de champs count
    FieldCountKindStatic, // il s'agit d'un tableau de taille fixe
    FieldCountKindField // La taille du tableau est donné par un autre champs
}FieldCountKind;

typedef union {
    APIFieldDescriptor* field;
    int count;
} FieldCount;


typedef struct APIFieldDescriptor_{
    char* name;
    int offset;
    FieldTypeKind typeKind;
    FieldType Type;
    FieldCountKind countKind;
    FieldCount count;
};

typedef struct APIStructDescriptor_{
    char* name;
    int count;
    APIFieldDescriptor* fields;
} APIStructDescriptor;

typedef struct {
    char* name;
    APIStructDescriptor* descriptor;
    void* instance;
} APIShare;

typedef struct {
    int count;
    APIShare item[APISHARES_MAX];
} APIShares;

#endif
