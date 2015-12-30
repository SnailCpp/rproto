#ifndef __PROTO_DECODER_H__
#define __PROTO_DECODER_H__

#include <string>
#include "Value.h"
#include "ByteArray.h"
#include "Loader.h"

namespace proto {

class Decoder {
public:
    Decoder(Loader* loader);
    ~Decoder();

    int decode(ByteArray& bytes, std::string& name, Map* dict);

private:
    int readStruct(ByteArray& bytes, Proto* struc, Map* dict);
    int readList(ByteArray& bytes, const TypeIter type, const EnumMap* enums, Vec* values);
    int readEnum(ByteArray& bytes, const EnumMap* enums, std::string* value);
    int readPrime(ByteArray& bytes, const TypeIter type, Value* value);

    Loader* _loader;
};

}

#endif // __PROTO_DECODER_H__
