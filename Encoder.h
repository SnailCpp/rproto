#ifndef __PROTO_ENCODER_H__
#define __PROTO_ENCODER_H__

#include <string>
#include "Value.h"
#include "ByteArray.h"
#include "Loader.h"

namespace proto {

class Encoder {
public:
    Encoder(Loader* loader);
    ~Encoder();

    int encode(ByteArray& bytes, const std::string& name, Map& dict);

private:
    int writeStruct(ByteArray& bytes, Proto* struc, Map& dict);
    int writeList  (ByteArray& bytes, const TypeIter type,  const EnumMap* enums, Vec& values);
    int writeEnum  (ByteArray& bytes, const EnumMap* enums, std::string& value);
    int writePrime (ByteArray& bytes, const TypeIter type,  Value& value);

    Loader* _loader;
};

}

#endif // __PROTO_ENCODER_H__
