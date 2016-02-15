#include "Loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rproto;

Loader::Loader() {
}

Loader::~Loader() {
}

void Loader::setProtoPath(const string& path) {
    char end = path[path.size()-1];
    _path = (end != '/' && end != '\\') ? path + "/" : path;
}

void Loader::loadAllProtos() {
    string path_ALL(_path + "_ALL");

    // only for unix
    string cmd("ls " + _path + " -1 > " + path_ALL);
    system(cmd.c_str());

    FILE *fp = fopen(path_ALL.c_str(), "r");
    if(!fp) {
        printf("Error Read _ALL");
        return;
    }
    char buffer[1024];
    while(fgets(buffer, 1024, fp) != NULL) {
        int len = strlen(buffer);
        if(buffer[len-1] == '\n') {
            buffer[len-1] = 0;
        }
        if(buffer[0] != '_') {
            addProtos(fileToString(_path + string(buffer)));
        }
    }
    fclose(fp);
}

void Loader::addProtos(const string& content) {
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if(doc.HasParseError()) {
        printf("proto parse error: %s\n", content.c_str());
        return;
    }
    auto& protos = doc["protos"];
    for(unsigned int i = 0; i < protos.Size(); i++) {
        auto& item = protos[i];
        int id = item["id"].GetInt();
        string name(item["name"].GetString());
        // parse to Proto
        Proto* proto = &_protos[id];
        proto->setId(id);
        proto->setName(&name);
        if(item.HasMember("enum")) {
            auto enums = new map<string, int>();
            auto& jenum = item["enum"];
            for(auto it = jenum.MemberBegin(); it != jenum.MemberEnd(); it++) {
                enums->insert(pair<string, int>(it->name.GetString(), it->value.GetInt()));
            }
            proto->setEnums(enums);
        }
        if(item.HasMember("field")) {
            auto fields = new vector<Field>();
            auto& jfield = item["field"];
            for(unsigned int i = 0; i < jfield.Size(); i++) {
                fields->push_back(Field(jfield[i].GetString()));
            }
            proto->setFields(fields);
        }
        _name_to_id.insert(pair<string, int>(name, id));
        printf("load proto: %d %s\n", id, name.c_str());
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
