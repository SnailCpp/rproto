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

void Loader::loadFromFolder(const string& path) {
    char endc = path[path.size()-1];
    string fpath = (endc != '/' && endc != '\\') ? path + "/" : path;

    string path_ALL(fpath + "_ALL");
    // only for unix
    string cmd("ls -1 " + path + ">" + path_ALL);
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
            string content(fileToContent(fpath + package));
            loadFromString(package, content);
        }
    }
    fclose(fp);
}

void Loader::loadFromString(const string& package, const string& str) {
    _protoStr = str;
    addProtos(package);
}

string Loader::fullName(const string& package, const string& name, const string& type) {
    if(type == "") {
        return package + "." + name;
    } else {
        return package + "." + name + "." + type;
    }
}

int Loader::getIdByFullName(const string& fullname) {
    auto it = _name2Id.find(fullname);
    if(it == _name2Id.end()) {
        return -1;
    }
    return it->second;
}

int Loader::getIdByName(const string& package, const string& name, const string& type) {
    return getIdByFullName(fullName(package, name, type));
}

const Proto* Loader::getProtoByFullName(const string& fullname) {
    int id = getIdByFullName(fullname);
    return getProtoById(id);
}

const Proto* Loader::getProtoByName(const string& package, const string& name, const string& type) {
    return getProtoByFullName(fullName(package, name, type));
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
            string name = getWord();
            string type;
            while((type=getWord()) != "}") {
                //auto name = rename(package, rpcname, subname);
                auto id = strToInt(getWord());
                addOneProto(package, name, type, id, true);
            }
        }
        else {
            auto name = getWord();
            //auto name = rename(package, getWord());
            auto id = strToInt(getWord());
            addOneProto(package, name, "", id, false);
        }
    }
}

string Loader::fileToContent(const string& path) {
    FILE *fp = fopen(path.c_str(), "r");
    if(!fp) {
        _protoStr = "";
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
    auto& fc = _protoStr;
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
        if(word[i] != _protoStr[beg+i]) {
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
    return string(_protoStr, beg, len);
}

string Loader::getLine() {
    auto& fc = _protoStr;
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

void Loader::addOneProto(const string& package,
                         const string& name,
                         const string& type,
                         int id,
                         bool isrpc) {
    auto fName = fullName(package, name, type);
    _name2Id.insert(pair<string, int>(fName, id));

    // parse to Proto
    Proto* proto = &_protos[id];
    proto->setId(id);
    proto->setPackage(new string(package));
    proto->setName(new string(name));
    proto->setType(new string(type));
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
    if(isrpc) { // rpc has one more field, _session
        fields->push_back(Field("_session int"));
    }
    string line;
    while((line=getLine()) != "}") {
        fields->push_back(Field(line));
    }
    proto->setFields(fields);
    // debug
    printf("load proto: %d %s\n", id, fName.c_str());
}
