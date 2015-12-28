#include "Proto.h"

using namespace std;
using namespace proto;

Field::Field(const string& rawstr) {
    int start = 0;
    auto end = rawstr.find(' ', start);
    if(end != string::npos) {
        _name = string(rawstr.substr(start, end-start));
        start = end + 1;
    }
    while((end = rawstr.find(' ', start)) != string::npos) {
        _type.push_back(rawstr.substr(start, end-start));
        start = end + 1;
    }
    _type.push_back(rawstr.substr(start));
}

Field::~Field() {
}

const string& Field::name() {
    return _name;
}

const vector<string>& Field::type() {
    return _type;
}

const TypeIter Field::typeIter() {
    return _type.begin();
}


Proto::Proto()
: _enums(nullptr)
, _fields(nullptr) {

}

Proto::~Proto() {
    if(_enums) {
        delete _enums;
        _enums = nullptr;
    }
    if(_fields) {
        delete _fields;
        _fields = nullptr;
    }
}

int Proto::id() const {
    return _id;
}

const std::string& Proto::name() const {
    return _name;
}

EnumMap* Proto::enums() const {
    return _enums;
}

std::vector<Field>* Proto::fields() const {
    return _fields;
}

void Proto::setId(int id) {
    _id = id;
}

void Proto::setName(const string& name) {
    _name = name;
}

void Proto::setEnums(map<string, int>* enums) {
    _enums = enums;
}

void Proto::setFields(vector<Field>* fields) {
    _fields = fields;
}
