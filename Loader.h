#ifndef __PROTO_LOADER_H__
#define __PROTO_LOADER_H__

#include <string>
#include <map>
#include <vector>
#include "Proto.h"

namespace proto {

const int Proto_Index_Min = 100;
const int Proto_Index_Max = 65499;

class Loader {
public:
    Loader();
    ~Loader();

    void setProtoPath(const std::string& path);
    void loadAllProtos();

    Proto* getProtoByName(const std::string& name);
    Proto* getProtoById(int id);

private:
    std::string fileToString(const std::string& path);

    std::string _path;
    Proto _protos[Proto_Index_Max+1];
    std::map<std::string, int> _name_to_id;
};

}

#endif // __PROTO_LOADER_H__
