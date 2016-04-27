#ifndef __PROTO_PROTO_H__
#define __PROTO_PROTO_H__

#include <string>
#include <vector>
#include <map>
#include "ByteArray.h"

namespace rproto {

typedef std::vector<std::string>::const_iterator TypeIter;

class Field
{
public:
    Field(const std::string& rawstr);
    ~Field();

    const std::string& name() const;
    const std::vector<std::string>& type() const;
    TypeIter typeIter() const;

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
    const std::string& package() const;
    const std::string& name() const;
    const std::string& type() const;
    const EnumMap* enums() const;
    const std::vector<Field>* fields() const;

    void setId(int id);
    void setPackage(std::string* package);
    void setName(std::string* name);
    void setType(std::string* type);
    void setEnums(EnumMap* enums);
    void setFields(std::vector<Field>* fields);

private:
    int _id;
    std::string* _package;
    std::string* _name;
    std::string* _type;
    EnumMap* _enums;
    std::vector<Field>* _fields;
};

}

#endif // __PROTO_PROTO_H__
