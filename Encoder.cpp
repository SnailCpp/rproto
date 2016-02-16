#include "Encoder.h"

using namespace std;
using namespace rproto;

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

int Encoder::encode(ByteArray* bytes, const string& name, const Map& dict) {
    auto proto = _loader->getProtoByName(name);
    if(proto == nullptr) {
        return -1;
    }

    bytes->wInt16(proto->id()); // id
    int result = writeStruct(bytes, dict, proto); // proto
    return result;
}

int Encoder::writeStruct(ByteArray* bytes, const Map& value, const Proto* struc) {
    if(struc == nullptr) {
        return -1;
    }

    auto fields = struc->fields();
    if(fields == nullptr) {
        return 0;
    }
    for(auto& field : *fields) {
        auto& name = field.name();
        auto it = value.find(name);
        if(it == value.end()) {
            return -1;
        }

        auto type = field.typeIter();
        auto& value = it->second;

        if(*type == "struct") {
            auto sub_struc = _loader->getProtoByName(*(type+1));
            CHECK_RESULT(writeStruct(bytes, value.getMap(), sub_struc));
        }
        else if(*type == "list") {
            CHECK_RESULT(writeList(bytes, value.getVec(), struc->enums(), type+1));
        }
        else if(*type == "enum") {
            CHECK_RESULT(writeEnum(bytes, value.getStr(), struc->enums()));
        }
        else {
            CHECK_RESULT(writePrime(bytes, value, type));
        }
    }
    return 0;
}

int Encoder::writeList(ByteArray* bytes, const Vec& values, const EnumMap* enums, const TypeIter type) {
    // array's length
    bytes->wInt16(values.size());
    // array's items
    if(*type == "list") {
        for(auto& item : values) {
            CHECK_RESULT(writeList(bytes, item.getVec(), enums, type+1));
        }
    }
    else if(*type == "struct") {
        auto struc = _loader->getProtoByName(*(type+1));
        for(auto& item : values) {
            CHECK_RESULT(writeStruct(bytes, item.getMap(), struc));
        }
    }
    else if(*type == "enum") {
        for(auto& item : values) {
            CHECK_RESULT(writeEnum(bytes, item.getStr(), enums));
        }
    }
    else {
        for(auto& item : values) {
            CHECK_RESULT(writePrime(bytes, item, type));
        }
    }
    return 0;
}

int Encoder::writeEnum(ByteArray* bytes, const string& value, const EnumMap* enums) {
    if(enums == nullptr) {
        return -1;
    }

    auto it = enums->find(value);
    if(it == enums->end()) {
        return -1;
    }
    bytes->wInt8(static_cast<uint8_t>(it->second));
    return 0;
}

int Encoder::writePrime(ByteArray* bytes, const Value& value, const TypeIter type) {
    if(*type == "int" || *type == "int32") {
        bytes->wInt32(value.getInt());
    }
    else if(*type == "int16") {
        bytes->wInt16(value.getInt());
    }
    else if(*type == "int8") {
        bytes->wInt8(value.getInt());
    }
    else if(*type == "string") {
        bytes->wString(value.getStr());
    }
    else {
        printf("unknown type: %s\n", type->c_str());
        return -1;
    }
    return 0;
}
