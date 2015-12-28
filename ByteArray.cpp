#include "ByteArray.h"
#include <string.h>
#include <algorithm>
#include <assert.h>

using namespace proto;

// big 64k
ByteArray::ByteArray()
: ByteArray(ByteEndian::BIG, 64*1024) {
}

ByteArray::ByteArray(ByteEndian endian, size_t cap)
: _endian(endian)
, _read_offset(0)
, _write_offset(0) {
    _buffer = new byte[cap];
}

ByteArray::~ByteArray() {
    delete[] _buffer;
    _buffer = nullptr;
}

void ByteArray::wInt8(uint8_t data) {
    auto buf = _buffer + _write_offset;
    buf[0] = data & 0xff;
    _write_offset += 1;
}

void ByteArray::wInt16(uint16_t data) {
    auto buf = _buffer + _write_offset;
    if(_endian == ByteEndian::LITTLE) {
        buf[0] = data & 0xff;
        buf[1] = data >> 8 & 0xff;
    } else {
        buf[0] = data >> 8 & 0xff;
        buf[1] = data & 0xff;
    }
    _write_offset += 2;
}

void ByteArray::wInt32(uint32_t data) {
    auto buf = _buffer + _write_offset;
    if(_endian == ByteEndian::LITTLE) {
        buf[0] = data & 0xff;
        buf[1] = data >> 8 & 0xff;
        buf[2] = data >> 16 & 0xff;
        buf[3] = data >> 24 & 0xff;
    } else {
        buf[0] = data >> 24 & 0xff;
        buf[1] = data >> 16 & 0xff;
        buf[2] = data >> 8 & 0xff;
        buf[3] = data & 0xff;
    }
    _write_offset += 4;
}

void ByteArray::wString(const std::string& data) {
    for(auto& d : data) {
        _buffer[_write_offset++] = uint8_t(d);
    }
    // end with sep
    _buffer[_write_offset++] = uint8_t('\0');
}

void ByteArray::wBytes(const byte* bytes, size_t count) {
    memcpy(_buffer+_write_offset, bytes, count);
    _write_offset += count;
}

uint8_t ByteArray::rInt8() {
    auto buf = _buffer + _read_offset;
    _read_offset += 1;
    return buf[0];
}

uint16_t ByteArray::rInt16() {
    auto buf = _buffer + _read_offset;
    _read_offset += 2;
    if(_endian == ByteEndian::LITTLE) {
        return (buf[1] << 8) | (buf[0]);
    } else {
        return (buf[0] << 8) | (buf[1]);
    }
}

uint32_t ByteArray::rInt32() {
    auto buf = _buffer + _read_offset;
    _read_offset += 4;
    if(_endian == ByteEndian::LITTLE) {
        return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
    } else {
        return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]);
    }
}

std::string ByteArray::rString() {
    char* buf = (char*)(_buffer+_read_offset);
    int len = 0;
    while(buf[len] != '\0') {
        ++len;
    }
    _read_offset += len+1;
    return std::string(buf, 0, len);
}

size_t ByteArray::size() const {
    return _write_offset;
}

const byte* ByteArray::first() const {
    return _buffer;
}

ByteEndian ByteArray::endian() const {
    return _endian;
}

void ByteArray::clear() {
    _write_offset = 0;
    _read_offset = 0;
}

void ByteArray::erase(size_t len) {
    // memmove
    assert(len <= _write_offset);
    auto src = _buffer;
    auto dst = _buffer + len;
    auto sz = _write_offset - len;
    for(size_t i = 0; i < sz; i++) {
        src[i] = dst[i];
    }
    _write_offset -= len;
    _read_offset = std::max(_read_offset-len, size_t(0));
}

void ByteArray::readMv(size_t offset) {
    _read_offset += offset;
}

void ByteArray::writeMv(size_t offset) {
    _write_offset += offset;
}

void ByteArray::readReset() {
    _read_offset = 0;
}

void ByteArray::writeReset() {
    _write_offset = 0;
}
