#include "Loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    string cmd("ls -1 " + _path + ">" + path_ALL);
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
            string package(buffer);
            fileToContent(_path + package);
            addProtos(package);
        }
    }
    fclose(fp);
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

void Loader::addProtos(const string& package) {
    _offset = 0;
    while(hasNextWord()) {
        if(peekAndCmpWord("RPC")) {
            string rpcname = getWord();
            string subname;
            while((subname=getWord()) != "}") {
                auto name = rename(package, rpcname, subname);
                auto id = strToInt(getWord());
                addOneProto(name, id);
            }
        }
        else {
            auto name = rename(package, getWord());
            auto id = strToInt(getWord());
            addOneProto(name, id);
        }
    }
}

void Loader::fileToContent(const string& path) {
    FILE *fp = fopen(path.c_str(), "r");
    if(!fp) {
        _fileContent = "";
        return;
    }
    fseek(fp,0,SEEK_END);
    auto sz = ftell(fp);
    fseek(fp,0,SEEK_SET);

    auto buffer = (char*)malloc(sizeof(char) * sz);
    auto rsz = fread(buffer, sizeof(char), sz, fp);
    fclose(fp);
    _fileContent = string(buffer, 0, rsz);
}

uint32_t Loader::strToInt(const std::string& str) {
    uint32_t res = 0;
    for(size_t i = 0; i < str.size(); i++) {
        int n = str[i] - '0';
        res = res * 10 + n;
    }
    return res;
}

inline bool Loader::isspace(char c) {
    bool ischar = (c >= '0' && c <= '9') ||
                 (c >= 'a' && c <= 'z') ||
                 (c >= 'A' && c <= 'Z') ||
                 (c == '_') ||
                 (c == '}');
    return !ischar;
}

bool Loader::peekWord(size_t& beg, size_t& len) {
    auto& fc = _fileContent;
    if(_offset >= fc.size()) {
        return false;
    }
    size_t p = _offset;
    while(isspace(fc[p])) {
        if(++p >= fc.size()) {
            return false;
        }
    }
    beg = p;
    while(p < fc.size() && !isspace(fc[p])) {
        ++p;
    }
    len = p - beg;
    return true;
}

bool Loader::hasNextWord() {
    size_t beg, len;
    return peekWord(beg, len);
}

bool Loader::peekAndCmpWord(const string& word) {
    size_t beg, len;
    if(!peekWord(beg, len)) {
        return false;
    }
    if(len != word.size()) {
        return false;
    }
    for(size_t i = 0; i < len; i++) {
        if(word[i] != _fileContent[beg+i]) {
            return false;
        }
    }
    _offset = beg + len;
    return true;
}

string Loader::getWord() {
    size_t beg, len;
    if(!peekWord(beg, len)) {
        return "";
    }
    _offset = beg + len;
    return string(_fileContent, beg, len);
}

string Loader::getLine() {
    auto& fc = _fileContent;
    size_t p = _offset;
    while(isspace(fc[p])) {
        ++p;
    }
    size_t beg = p;
    while(fc[p] != '\n') {
        ++p;
    }
    while(isspace(fc[p])) {
        --p;
    }
    size_t len = p + 1 - beg;
    _offset = beg + len;
    return string(fc, beg, len);
}

string Loader::rename(const string& package, const string& subname1, const string& subname2) {
    if(subname2 == "") {
        return package + "_" + subname1;
    }
    else {
        return package + "_" + subname1 + "_" + subname2;
    }
}

void Loader::addOneProto(const string& name, int id) {
    _name_to_id.insert(pair<string, int>(name, id));
    // parse to Proto
    Proto* proto = &_protos[id];
    proto->setId(id);
    proto->setName(new string(name));
    // enum
    if(peekAndCmpWord("enum")) {
        auto enums = new map<string, int>();
        string key;
        int val;
        while((key=getWord()) != "}") {
            val = strToInt(getWord());
            enums->insert(pair<string, int>(key, val));
        }
        proto->setEnums(enums);
    }
    // field
    auto fields = new vector<Field>();
    string line;
    while((line=getLine()) != "}") {
        fields->push_back(Field(line));
    }
    proto->setFields(fields);
    printf("load proto: %d %s\n", id, name.c_str());
}
