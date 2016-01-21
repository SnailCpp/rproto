#include <lua.hpp>
#include <assert.h>

#include "Value.h"
#include "ByteArray.h"
#include "Proto.h"
#include "Loader.h"
#include "Encoder.h"
#include "Decoder.h"

bool isMap(lua_State* L);
void tableToMap(lua_State* L, int index, proto::Map* map);
void tableToVec(lua_State* L, int index, proto::Vec* vec);
void pushValue(lua_State* L, proto::Value& val);
void mapToTable(lua_State* L, proto::Map& map);
void vecToTable(lua_State* L, proto::Vec& vec);

bool isMap(lua_State* L) {
    if(!lua_istable(L, -1)) {
        return false;
    }

    bool ret = false;
    // table[1]
    lua_pushnumber(L, 1);
    lua_gettable(L, -2);
    // table[1] == nil
    if(lua_isnil(L, -1)) {
        ret = true;
    }
    lua_pop(L, 1);
    return ret;
}

void tableToMap(lua_State* L, int index, proto::Map* map) {
    lua_pushnil(L);
    while(lua_next(L, index) != 0) {
        auto key = lua_tostring(L, -2);
        if(lua_istable(L, -1)) {
            if(isMap(L)) {
                auto val = new proto::Map();
                tableToMap(L, lua_gettop(L), val);
                map->insert(proto::Map::value_type(key, proto::Value(val)));
            }
            else {
                auto val = new proto::Vec();
                tableToVec(L, lua_gettop(L), val);
                map->insert(proto::Map::value_type(key, proto::Value(val)));
            }
        }
        else if(lua_type(L, -1) == LUA_TSTRING) {
            auto val = new std::string(lua_tostring(L, -1));
            map->insert(proto::Map::value_type(key, proto::Value(val)));
        }
        else if(lua_type(L, -1) == LUA_TNUMBER) {
            auto val = lua_tointeger(L, -1);
            map->insert(proto::Map::value_type(key, proto::Value(val)));
        }
        else {
            assert("type error!");
        }
        lua_pop(L, 1);
    }
}

void tableToVec(lua_State* L, int index, proto::Vec* vec) {
    size_t len = lua_rawlen(L, index);
    for(size_t i = 1; i <= len; i++) {
        lua_pushnumber(L, i);
        lua_gettable(L, index);
        if(lua_istable(L, -1)) {
            if(isMap(L)) {
                auto val = new proto::Map();
                tableToMap(L, lua_gettop(L), val);
                vec->push_back(proto::Value(val));
            }
            else {
                auto val = new proto::Vec();
                tableToVec(L, lua_gettop(L), val);
                vec->push_back(proto::Value(val));
            }
        }
        else if(lua_type(L, -1) == LUA_TSTRING) {
            vec->push_back(proto::Value(new std::string(lua_tostring(L, -1))));
        }
        else if(lua_type(L, -1) == LUA_TNUMBER) {
            vec->push_back(proto::Value(lua_tointeger(L, -1)));
        }
        else {
            assert("type error!");
        }
        lua_pop(L, 1);
    }
}

void pushValue(lua_State* L, proto::Value& val) {
    switch(val.getType()) {
        case proto::Type::TMap:
            mapToTable(L, val.getMap());
            break;
        case proto::Type::TVec:
            vecToTable(L, val.getVec());
            break;
        case proto::Type::TInt:
            lua_pushinteger(L, val.getInt());
            break;
        case proto::Type::TStr:
            lua_pushstring(L, val.getStr().c_str());
            break;
        default:
            assert("type error!");
            break;
    }
}

void mapToTable(lua_State* L, proto::Map& map) {
    lua_newtable(L);
    for(auto& it : map) {
        lua_pushstring(L, it.first.c_str());
        pushValue(L, it.second);
        lua_rawset(L, -3);
    }
}

void vecToTable(lua_State* L, proto::Vec& vec) {
    lua_newtable(L);
    for(int i = 0; i < vec.size(); i++) {
        lua_pushnumber(L, i+1);
        pushValue(L, vec[i]);
        lua_rawset(L, -3);
    }
}

extern "C"
{
    static int lnewByteArray(lua_State* L) {
        proto::ByteArray** p = (proto::ByteArray**)lua_newuserdata(L, sizeof(proto::ByteArray*));
        *p = new proto::ByteArray();
        luaL_getmetatable(L, "red.rproto.ByteArray");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelByteArray(lua_State* L) {
        printf(".......... del ByteArray\n");
        proto::ByteArray** p = (proto::ByteArray**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewLoader(lua_State* L) {
        proto::Loader** p = (proto::Loader**)lua_newuserdata(L, sizeof(proto::Loader*));
        *p = new proto::Loader();
        luaL_getmetatable(L, "red.rproto.Loader");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelLoader(lua_State* L) {
        printf(".......... del loader\n");
        proto::Loader** p = (proto::Loader**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewEncoder(lua_State* L) {
        proto::Loader* loader = *(proto::Loader**)lua_touserdata(L, 1);
        proto::Encoder** p = (proto::Encoder**)lua_newuserdata(L, sizeof(proto::Encoder*));
        *p = new proto::Encoder(loader);
        luaL_getmetatable(L, "red.rproto.Encoder");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelEncoder(lua_State* L) {
        printf(".......... del encoder\n");
        proto::Encoder** p = (proto::Encoder**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewDecoder(lua_State* L) {
        proto::Loader* loader = *(proto::Loader**)lua_touserdata(L, 1);
        proto::Decoder** p = (proto::Decoder**)lua_newuserdata(L, sizeof(proto::Decoder*));
        *p = new proto::Decoder(loader);
        luaL_getmetatable(L, "red.rproto.Decoder");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelDecoder(lua_State* L) {
        printf(".......... del decoder\n");
        proto::Decoder** p = (proto::Decoder**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lLoader_setup(lua_State* L) {
        proto::Loader* loader = *(proto::Loader**)lua_touserdata(L, 1);
        auto path = lua_tostring(L, 2);
        loader->setProtoPath(path);
        loader->loadAllProtos();
        return 0;
    }

    static int lEncoder_encode(lua_State* L) {
        proto::Encoder* encoder = *(proto::Encoder**)lua_touserdata(L, 1);
        proto::ByteArray* bytes = *(proto::ByteArray**)lua_touserdata(L, 2);
        std::string name(lua_tostring(L, 3));
        proto::Map dict;
        tableToMap(L, 4, &dict);
        encoder->encode(bytes, name, dict);
        return 0;
    }

    static int lDecoder_decode(lua_State* L) {
        proto::Decoder* decoder = *(proto::Decoder**)lua_touserdata(L, 1);
        proto::ByteArray* bytes = *(proto::ByteArray**)lua_touserdata(L, 2);
        std::string name(lua_tostring(L, 3));
        proto::Map dict;
        decoder->decode(*bytes, name, &dict);
        mapToTable(L, dict);
        return 1;
    }

    static int lprint_bytes(lua_State* L) {
        proto::ByteArray* bytes = *(proto::ByteArray**)lua_touserdata(L, 1);
        for(size_t i = 0; i < bytes->size(); i++) {
            printf("%d ", *(bytes->first()+i));
        }
        printf("\n");
    }

    int luaopen_rproto(lua_State* L) {
        luaL_checkversion(L);
        luaL_openlibs(L);

        struct luaL_Reg funcs[] = {
            {"newByteArray", lnewByteArray},
            {"newLoader",    lnewLoader},
            {"newEncoder",   lnewEncoder},
            {"newDecoder",   lnewDecoder},
            {"printBytes",   lprint_bytes},
            {NULL,           NULL}
        };

        struct luaL_Reg f_Loader[] = {
            {"setup", lLoader_setup},
            {NULL,  NULL}
        };

        struct luaL_Reg f_Encoder[] = {
            {"encode", lEncoder_encode},
            {NULL,  NULL}
        };

        struct luaL_Reg f_Decoder[] = {
            {"decode", lDecoder_decode},
            {NULL, NULL}
        };

        luaL_newmetatable(L, "red.rproto.ByteArray");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, ldelByteArray);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        luaL_newmetatable(L, "red.rproto.Loader");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, ldelLoader);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);
        luaL_setfuncs(L, f_Loader, 0);

        luaL_newmetatable(L, "red.rproto.Encoder");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, ldelEncoder);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);
        luaL_setfuncs(L, f_Encoder, 0);

        luaL_newmetatable(L, "red.rproto.Decoder");
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, ldelDecoder);
        lua_settable(L, -3);

        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);
        luaL_setfuncs(L, f_Decoder, 0);

        luaL_newlib(L, funcs);
        return 1;
    }
}
