#include "Decoder.h"

using namespace std;
using namespace rproto;

#define CHECK_RESULT(res) do {\
    int r = res;\
    if(r != 0) {\
        return r;\
    }\
} while(0);

#define CHECK_RESULT_FREE(res, item) do {\
    int r = res;\
    if(r != 0) {\
        delete item;\
        return r;\
    }\
} while(0);

Decoder::Decoder(Loader* loader) {
    _loader = loader;
}

Decoder::~Decoder() {
}

int Decoder::decode(const ByteArray& bytes, string& name, Map* dict) {
    int id = bytes.rInt16();
    auto proto = _loader->getProtoById(id);
    if(proto == nullptr) {
        return -1;
    }

    name = proto->name();
    int result = readStruct(bytes, dict, proto);
    return result;
}

int Decoder::readStruct(const ByteArray& bytes, Map* value, const Proto* struc) {
    if(struc == nullptr) {
        return -1;
    }
    auto fields = struc->fields();
    if(fields == nullptr) {
        return 0;
    }
    for(auto& field : *fields) {
        auto& name = field.name();
        auto type = field.typeIter();

        if(*type == "struct") {
            auto sub_struc = _loader->getProtoByName(*(type+1));
            auto item = new Map();
            CHECK_RESULT_FREE(readStruct(bytes, item, sub_struc), item);
            value->insert(Map::value_type(name, Value(item)));
        }
        else if(*type == "list") {
            auto item = new Vec();
            CHECK_RESULT_FREE(readList(bytes, item, struc->enums(), type+1), item);
            value->insert(Map::value_type(name, Value(item)));
        }
        else if(*type == "enum") {
            auto item = new string();
            CHECK_RESULT_FREE(readEnum(bytes, item, struc->enums()), item);
            value->insert(Map::value_type(name, Value(item)));
        }
        else {
            Value item;
            CHECK_RESULT(readPrime(bytes, &item, type));
            value->insert(Map::value_type(name, item));
        }
    }
    return 0;
}

int Decoder::readList(const ByteArray& bytes, Vec* values, const EnumMap* enums, const TypeIter type) {
    int len = bytes.rInt16();
    if(*type == "struct") {
        auto struc = _loader->getProtoByName(*(type+1));
        for(int i = 0; i < len; i++) {
            auto item = new Map();
            CHECK_RESULT_FREE(readStruct(bytes, item, struc), item);
            values->push_back(Value(item));
        }
    }
    else if(*type == "list") {
        for(int i = 0; i < len; i++) {
            auto item = new Vec();
            CHECK_RESULT_FREE(readList(bytes, item, enums, type+1), item);
            values->push_back(Value(item));
        }
    }
    else if(*type == "enum") {
        for(int i = 0; i < len; i++) {
            auto item = new string();
            CHECK_RESULT_FREE(readEnum(bytes, item, enums), item);
            values->push_back(Value(item));
        }
    }
    else {
        for(int i = 0; i < len; i++) {
            Value item;
            CHECK_RESULT(readPrime(bytes, &item, type));
            values->push_back(item);
        }
    }
    return 0;
}

int Decoder::readEnum(const ByteArray& bytes, string* value, const EnumMap* enums) {
    int value_int = static_cast<int>(bytes.rInt8());
    for(auto& e : *enums) {
        if(e.second == value_int) {
            *value = e.first;
            return 0;
        }
    }
    return -1;
}

int Decoder::readPrime(const ByteArray& bytes, Value* value, const TypeIter type) {
    if(*type == "int" || *type == "int32") {
        value->set(static_cast<int>(bytes.rInt32()));
    }
    else if(*type == "int16") {
        value->set(static_cast<int>(bytes.rInt16()));
    }
    else if(*type == "int8") {
        value->set(static_cast<int>(bytes.rInt8()));
    }
    else if(*type == "string") {
        auto str = new string(bytes.rString());
        value->set(str);
    }
    else {
        printf("unknown type: %s\n", type->c_str());
        return -1;
    }
    return 0;
}
