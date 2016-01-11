#include "Loader.h"
#include <stdio.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace proto;

Loader::Loader() {
}

Loader::~Loader() {
}

void Loader::setProtoPath(const string& path) {
    char end = path[path.size()-1];
    _path = (end != '/' && end != '\\') ? path + "/" : path;
}

void Loader::loadAllProtos() {
    auto data = fileToString(_path + "ALL");

    rapidjson::Document all;
    all.Parse<0>(data.c_str());
    if(all.HasParseError()) {
        printf("ALL parse error!\n");
        return;
    }

    auto& protos_conf = all["protos"];
    for(unsigned int i = 0; i < protos_conf.Size(); i++) {
        auto& item = protos_conf[i];

        int id = int(item["id"].GetUint());
        auto name = new string(item["name"].GetString());

        _name_to_id.insert(pair<string, int>(*name, id));

        // read proto file
        auto proto_data = fileToString(_path + *name);
        addProto(id, name, proto_data);
    }
}

void Loader::addProto(int id, std::string* name, const std::string& content) {
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if(doc.HasParseError()) {
        printf("proto parse error: %d %s\n", id, name->c_str());
    } else {
        printf("load proto: %d %s\n", id, name->c_str());
    }

    // parse to Proto
    Proto* proto = &_protos[id];
    proto->setId(id);
    proto->setName(name);
    if(doc.HasMember("enum")) {
        auto enums = new map<string, int>();
        auto& jenum = doc["enum"];
        for(auto it = jenum.MemberBegin(); it != jenum.MemberEnd(); it++) {
            enums->insert(pair<string, int>(it->name.GetString(), it->value.GetInt()));
        }
        proto->setEnums(enums);
    }
    if(doc.HasMember("field")) {
        auto fields = new vector<Field>();
        auto& jfield = doc["field"];
        for(unsigned int i = 0; i < jfield.Size(); i++) {
            fields->push_back(Field(jfield[i].GetString()));
        }
        proto->setFields(fields);
    }
}

const Proto* Loader::getProtoByName(const std::string& name) {
    auto it = _name_to_id.find(name);
    if(it == _name_to_id.end()) {
        return nullptr;
    }
    return getProtoById(it->second);
}

const Proto* Loader::getProtoById(int id) {
    if(id >= Proto_Index_Min && id <= Proto_Index_Max) {
        return &_protos[id];
    }
    return nullptr;
}

string Loader::fileToString(const string& path) {
    FILE *fp = fopen(path.c_str(), "r");
    if(!fp) {
        return "";
    }
    fseek(fp,0,SEEK_END);
    auto sz = ftell(fp);
    fseek(fp,0,SEEK_SET);

    auto buffer = (char*)malloc(sizeof(char) * sz);
    auto rsz = fread(buffer, sizeof(char), sz, fp);
    fclose(fp);
    return string(buffer, 0, rsz);
}
