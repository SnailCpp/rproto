#ifndef __PROTO_LOADER_H__
#define __PROTO_LOADER_H__

#include <string>
#include <map>
#include <vector>
#include "Proto.h"

namespace rproto {

const int Proto_Index_Min = 100;
const int Proto_Index_Max = 65499;

class Loader {
public:
    Loader();
    ~Loader();

    void loadFromFolder(const std::string& path);
    void loadFromString(const std::string& package,
                        const std::string& str);

    std::string fullName(const std::string& package,
                          const std::string& name,
                          const std::string& type);
    int getIdByFullName(const std::string& fullname);
    int getIdByName(const std::string& package,
                    const std::string& name,
                    const std::string& type);
    const Proto* getProtoByFullName(const std::string& fullname);
    const Proto* getProtoByName(const std::string& package,
                                const std::string& name,
                                const std::string& type);
    const Proto* getProtoById(int id);

private:
    void addProtos(const std::string& package);
    std::string fileToContent(const std::string& path);
    uint32_t strToInt(const std::string& str);
    bool isspace(char c);
    bool peekWord(size_t& beg, size_t& len);
    bool hasNextWord();
    bool peekAndCmpWord(const std::string& word);
    std::string getWord();
    std::string getLine();
    void addOneProto(const std::string& package,
                     const std::string& name,
                     const std::string& type,
                     int id,
                     bool isrpc);

    std::string _protoStr;
    size_t _offset;

    Proto _protos[Proto_Index_Max+1];
    std::map<std::string, int> _name2Id;
};

}

#endif // __PROTO_LOADER_H__
