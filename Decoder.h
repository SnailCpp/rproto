//#ifndef __PROTO_DECODER_H__
//#define __PROTO_DECODER_H__
//
//#include "Value.h"
//
//#include "document.h"
//#include "writer.h"
//#include "stringbuffer.h"
//
//namespace proto {
//
//class Decoder {
//public:
//    Decoder();
//    virtual int decode(const rapidjson::Document& doc, const ByteArray& bytes, cocos2d::ValueMap& dict);
//
//private:
//    // 读取struct
//    int readStruct(const rapidjson::Value& fields, const ByteArray& bytes, cocos2d::ValueMap& dict);
//
//    // 读取数组
//    int readList(const std::vector<std::string>& subs, int index, const ByteArray& bytes, cocos2d::ValueVector& array);
//
//    // 读取枚举值
//    int readEnum(const rapidjson::Value& enums, const ByteArray& bytes, std::string& value);
//
//    // 读取基本类型（int, string等）
//    int readPrime(const std::string& type, const ByteArray& bytes, cocos2d::Value& value);
//
//    size_t _offset;
//};
//
//}
//
//#endif // __PROTO_DECODER_H__
