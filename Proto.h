#ifndef __PROTO_PROTO_H__
#define __PROTO_PROTO_H__

#include <string>
#include <vector>
#include <map>
#include "ByteArray.h"

namespace proto {

typedef std::vector<std::string>::iterator TypeIter;

class Field
{
public:
    Field(const std::string& rawstr);
    ~Field();

    const std::string& name();
    const std::vector<std::string>& type();
    const TypeIter typeIter();

private:
    std::string _name;
    std::vector<std::string> _type;
};

typedef std::map<std::string, int> EnumMap;

class Proto
{
public:
    Proto();
    ~Proto();

    int id() const;
    const std::string& name() const;
    EnumMap* enums() const;
    std::vector<Field>* fields() const;

    void setId(int id);
    void setName(const std::string& name);
    void setEnums(EnumMap* enums);
    void setFields(std::vector<Field>* fields);

private:
    int _id;
    std::string _name;
    EnumMap* _enums;
    std::vector<Field>* _fields;
};

}

#endif // __PROTO_PROTO_H__
