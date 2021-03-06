#ifndef __PROTO_BYTEARRAY_H__
#define __PROTO_BYTEARRAY_H__

#include <string>

namespace rproto {

#ifdef _MSC_VER
typedef char byte;
#else
typedef uint8_t byte;
#endif

enum class ByteEndian {
    BIG,
    LITTLE
};

class ByteArray {
public:
    ByteArray();
    ByteArray(ByteEndian endian, size_t cap);
    ~ByteArray();

    void wInt8(uint8_t data);
    void wInt16(uint16_t data);
    void wInt32(uint32_t data);
    void wString(const std::string& data);
    void wBytes(const byte* bytes, size_t count);

    uint8_t rInt8() const;
    uint16_t rInt16() const;
    uint32_t rInt32() const;
    std::string rString() const;

    size_t size() const;
    const byte* first() const;
    ByteEndian endian() const;

    // unsafe
    void readMv(size_t offset);
    void writeMv(size_t offset);

    void readReset() const;
    void writeReset();

    void clear();
    void erase(size_t len);

private:
    ByteEndian _endian;
    byte* _buffer;
    mutable size_t _read_offset;
    size_t _write_offset;
};

}

#endif // __PROTO_BYTEARRAY_H__
