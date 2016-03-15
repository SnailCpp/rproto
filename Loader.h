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

    void setProtoPath(const std::string& path);
    void loadAllProtos();

    const Proto* getProtoByName(const std::string& name);
    const Proto* getProtoById(int id);

private:
    void addProtos(const std::string& package);
    void fileToContent(const std::string& path);
    uint32_t strToInt(const std::string& str);
    bool isspace(char c);
    bool peekWord(size_t& beg, size_t& len);
    bool hasNextWord();
    bool peekAndCmpWord(const std::string& word);
    std::string getWord();
    std::string getLine();
    std::string rename(const std::string& package,
                        const std::string& subname1,
                        const std::string& subname2 = "");
    void addOneProto(const std::string& name, int id, bool isrpc);

    std::string _fileContent;
    size_t _offset;

    std::string _path;
    Proto _protos[Proto_Index_Max+1];
    std::map<std::string, int> _name_to_id;
};

}

#endif // __PROTO_LOADER_H__
