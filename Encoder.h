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

    int encode(ByteArray* bytes, const std::string& name, const Map& dict);

private:
    int writeStruct(ByteArray* bytes, const Map& value, const Proto* struc);
    int writeList  (ByteArray* bytes, const Vec& values, const EnumMap* enums, const TypeIter type);
    int writeEnum  (ByteArray* bytes, const std::string& value, const EnumMap* enums);
    int writePrime (ByteArray* bytes, const Value& value,  const TypeIter type);

    Loader* _loader;
};

}

#endif // __PROTO_ENCODER_H__
