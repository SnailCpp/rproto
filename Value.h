#ifndef __PROTO_VALUE_H__
#define __PROTO_VALUE_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace proto {

enum class Type {
    TNull,
    TInt,
    TStr,
    TVec,
    TMap
};

class Value;

typedef std::string Str;
typedef std::vector<Value> Vec;
typedef std::unordered_map<std::string, Value> Map;

class Value
{
public:
    Value();
    ~Value();

    Value(int v);
    Value(Str* v);
    Value(Vec* v);
    Value(Map* v);
    Value(const Value& v);

//    Value& operator= (int v);
//    Value& operator= (const Value& other);
//    Value& operator= (const Value& other);
//    Value& operator= (const Value& other);
    Value& operator= (const Value& other);

    void set(int v);
    void set(Str* v);
    void set(Vec* v);
    void set(Map* v);
    void set(const Value& v);

    int getInt();
    Str& getStr();
    Vec& getVec();
    Map& getMap();

private:
    void clear();

    union {
        int _int;
        Str* _str;
        Vec* _vec;
        Map* _map;
    }_data;

    Type _type;
};

}

#endif // __PROTO_VALUE_H__
