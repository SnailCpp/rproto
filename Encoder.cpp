#include "Encoder.h"

using namespace std;
using namespace proto;

#define CHECK_RESULT(res) do {\
    int r = res;\
    if(r != 0) {\
        return r;\
    }\
} while(0);

Encoder::Encoder(Loader* loader) {
    _loader = loader;
}

Encoder::~Encoder() {
}

int Encoder::encode(ByteArray& bytes, const string& name, Map& dict) {
    auto proto = _loader->getProtoByName(name);
    if(proto == nullptr) {
        return -1;
    }

    bytes.wInt16(proto->id()); // id
    writeStruct(bytes, proto, dict); // proto
    return 0;
}

int Encoder::writeStruct(ByteArray& bytes, Proto* struc, Map& dict) {
    if(struc == nullptr) {
        return -1;
    }

    auto fields = struc->fields();
    for(auto& field : *fields) {
        auto& name = field.name();
        auto it = dict.find(name);
        if(it == dict.end()) {
            return -1;
        }

        auto type = field.typeIter();
        auto& value = it->second;

        if(*type == "struct") {
            auto sub_struc = _loader->getProtoByName(*(type+1));
            CHECK_RESULT(writeStruct(bytes, sub_struc, value.getMap()));
        }
        else if(*type == "list") {
            CHECK_RESULT(writeList(bytes, type+1, struc->enums(), value.getVec()));
        }
        else if(*type == "enum") {
            CHECK_RESULT(writeEnum(bytes, struc->enums(), value.getStr()));
        }
        else {
            CHECK_RESULT(writePrime(bytes, type, value));
        }
    }
    return 0;
}

int Encoder::writeList(ByteArray& bytes, const TypeIter type, const EnumMap* enums, Vec& values) {
    // array's length
    bytes.wInt16(values.size());

    // array's items
    if(*type == "list") {
        for(auto& item : values) {
            CHECK_RESULT(writeList(bytes, type+1, enums, item.getVec()));
        }
    }
    else if(*type == "struct") {
        auto struc = _loader->getProtoByName(*(type+1));
        for(auto& item : values) {
            CHECK_RESULT(writeStruct(bytes, struc, item.getMap()));
        }
    }
    else if(*type == "enum") {
        for(auto& item : values) {
            CHECK_RESULT(writeEnum(bytes, enums, item.getStr()));
        }
    }
    else {
        for(auto& item : values) {
            CHECK_RESULT(writePrime(bytes, type, item));
        }
    }
    return 0;
}

int Encoder::writeEnum(ByteArray& bytes, const EnumMap* enums, string& value) {
    if(enums == nullptr) {
        return -1;
    }

    auto it = enums->find(value);
    if(it == enums->end()) {
        return -1;
    }
    bytes.wInt8(static_cast<uint8_t>(it->second));
    return 0;
}

int Encoder::writePrime(ByteArray& bytes, const TypeIter type, Value& value) {
    if(*type == "int") {
        bytes.wInt32(value.getInt());
    }
    else if(*type == "string") {
        bytes.wString(value.getStr());
    }
    else {
        printf("unknown type: %s\n", type->c_str());
        return -1;
    }
    return 0;
}
