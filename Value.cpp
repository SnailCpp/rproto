#include <assert.h>
#include "Value.h"

using namespace std;
using namespace proto;

Value::Value()
: _type(Type::TNull) {
}

Value::~Value() {
    clear();
}

Value::Value(int v)  { set(v); }
Value::Value(Str* v) { set(v); }
Value::Value(Vec* v) { set(v); }
Value::Value(Map* v) { set(v); }
Value::Value(const Value& v) { set(v); }

Value& Value::operator= (const Value& other) {
    set(other);
    return *this;
}

void Value::set(int v) {
    clear();
    _type = Type::TInt;
    _data._int = v;
}

void Value::set(Str* v) {
    clear();
    _type = Type::TStr;
    _data._str = v;
}

void Value::set(Vec* v) {
    clear();
    _type = Type::TVec;
    _data._vec = v;
}

void Value::set(Map* v) {
    clear();
    _type = Type::TMap;
    _data._map = v;
}

void Value::set(const Value& v) {
    if(this == &v) {
        return;
    }
    clear();
    switch(v._type) {
    case Type::TInt:
        _data._int = v._data._int;
        break;
    case Type::TStr:
        _data._str = new string();
        *_data._str = *v._data._str;
        break;
    case Type::TVec:
        _data._vec = new Vec();
        *_data._vec = *v._data._vec;
        break;
    case Type::TMap:
        _data._map = new Map();
        *_data._map = *v._data._map;
        break;
    default:
        break;
    }
    _type = v._type;
}

int Value::getInt() {
    assert(_type == Type::TInt);
    return _data._int;
}

Str& Value::getStr() {
    assert(_type == Type::TStr);
    return *(_data._str);
}

Vec& Value::getVec() {
    assert(_type == Type::TVec);
    return *(_data._vec);
}

Map& Value::getMap() {
    assert(_type == Type::TMap);
    return *(_data._map);
}

void Value::clear() {
    switch(_type) {
    case Type::TInt:
        _data._int = 0;
        break;
    case Type::TStr:
        delete _data._str;
        _data._str = nullptr;
        break;
    case Type::TVec:
        delete _data._vec;
        _data._vec = nullptr;
        break;
    case Type::TMap:
        delete _data._map;
        _data._map = nullptr;
        break;
    default:
        break;
    }
    _type = Type::TNull;
}
