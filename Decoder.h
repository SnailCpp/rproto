#ifndef __PROTO_DECODER_H__
#define __PROTO_DECODER_H__

#include <string>
#include "Value.h"
#include "ByteArray.h"
#include "Loader.h"

namespace rproto {

class Decoder {
public:
    Decoder(Loader* loader);
    ~Decoder();

    int decode(const ByteArray& bytes, std::string& name, Map* dict);

private:
    int readStruct(const ByteArray& bytes, Map* value, const Proto* struc);
    int readList  (const ByteArray& bytes, Vec* values, const EnumMap* enums, const TypeIter type);
    int readEnum  (const ByteArray& bytes, std::string* value, const EnumMap* enums);
    int readPrime (const ByteArray& bytes, Value* value, const TypeIter type);

    Loader* _loader;
};

}

#endif // __PROTO_DECODER_H__
