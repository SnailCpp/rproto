#include <lua.hpp>
#include <assert.h>

#include "Value.h"
#include "ByteArray.h"
#include "Proto.h"
#include "Loader.h"
#include "Encoder.h"
#include "Decoder.h"

bool isMap(lua_State* L);
void tableToMap(lua_State* L, int index, rproto::Map* map);
void tableToVec(lua_State* L, int index, rproto::Vec* vec);
void pushValue(lua_State* L, rproto::Value& val);
void mapToTable(lua_State* L, rproto::Map& map);
void vecToTable(lua_State* L, rproto::Vec& vec);

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

void tableToMap(lua_State* L, int index, rproto::Map* map) {
    lua_pushnil(L);
    while(lua_next(L, index) != 0) {
        auto key = lua_tostring(L, -2);
        if(lua_istable(L, -1)) {
            if(isMap(L)) {
                auto val = new rproto::Map();
                tableToMap(L, lua_gettop(L), val);
                map->insert(rproto::Map::value_type(key, rproto::Value(val)));
            }
            else {
                auto val = new rproto::Vec();
                tableToVec(L, lua_gettop(L), val);
                map->insert(rproto::Map::value_type(key, rproto::Value(val)));
            }
        }
        else if(lua_type(L, -1) == LUA_TSTRING) {
            auto val = new std::string(lua_tostring(L, -1));
            map->insert(rproto::Map::value_type(key, rproto::Value(val)));
        }
        else if(lua_type(L, -1) == LUA_TNUMBER) {
            auto val = lua_tointeger(L, -1);
            map->insert(rproto::Map::value_type(key, rproto::Value(val)));
        }
        else {
            assert("type error!");
        }
        lua_pop(L, 1);
    }
}

void tableToVec(lua_State* L, int index, rproto::Vec* vec) {
    size_t len = lua_rawlen(L, index);
    for(size_t i = 1; i <= len; i++) {
        lua_pushnumber(L, i);
        lua_gettable(L, index);
        if(lua_istable(L, -1)) {
            if(isMap(L)) {
                auto val = new rproto::Map();
                tableToMap(L, lua_gettop(L), val);
                vec->push_back(rproto::Value(val));
            }
            else {
                auto val = new rproto::Vec();
                tableToVec(L, lua_gettop(L), val);
                vec->push_back(rproto::Value(val));
            }
        }
        else if(lua_type(L, -1) == LUA_TSTRING) {
            vec->push_back(rproto::Value(new std::string(lua_tostring(L, -1))));
        }
        else if(lua_type(L, -1) == LUA_TNUMBER) {
            vec->push_back(rproto::Value(lua_tointeger(L, -1)));
        }
        else {
            assert("type error!");
        }
        lua_pop(L, 1);
    }
}

void pushValue(lua_State* L, rproto::Value& val) {
    switch(val.getType()) {
        case rproto::Type::TMap:
            mapToTable(L, val.getMap());
            break;
        case rproto::Type::TVec:
            vecToTable(L, val.getVec());
            break;
        case rproto::Type::TInt:
            lua_pushinteger(L, val.getInt());
            break;
        case rproto::Type::TStr:
            lua_pushstring(L, val.getStr().c_str());
            break;
        default:
            assert("type error!");
            break;
    }
}

void mapToTable(lua_State* L, rproto::Map& map) {
    lua_newtable(L);
    for(auto& it : map) {
        lua_pushstring(L, it.first.c_str());
        pushValue(L, it.second);
        lua_rawset(L, -3);
    }
}

void vecToTable(lua_State* L, rproto::Vec& vec) {
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
        rproto::ByteArray** p = (rproto::ByteArray**)lua_newuserdata(L, sizeof(rproto::ByteArray*));
        *p = new rproto::ByteArray();
        luaL_getmetatable(L, "red.rproto.ByteArray");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelByteArray(lua_State* L) {
        rproto::ByteArray** p = (rproto::ByteArray**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewLoader(lua_State* L) {
        rproto::Loader** p = (rproto::Loader**)lua_newuserdata(L, sizeof(rproto::Loader*));
        *p = new rproto::Loader();
        luaL_getmetatable(L, "red.rproto.Loader");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelLoader(lua_State* L) {
        rproto::Loader** p = (rproto::Loader**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewEncoder(lua_State* L) {
        rproto::Loader* loader = *(rproto::Loader**)lua_touserdata(L, 1);
        rproto::Encoder** p = (rproto::Encoder**)lua_newuserdata(L, sizeof(rproto::Encoder*));
        *p = new rproto::Encoder(loader);
        luaL_getmetatable(L, "red.rproto.Encoder");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelEncoder(lua_State* L) {
        rproto::Encoder** p = (rproto::Encoder**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lnewDecoder(lua_State* L) {
        rproto::Loader* loader = *(rproto::Loader**)lua_touserdata(L, 1);
        rproto::Decoder** p = (rproto::Decoder**)lua_newuserdata(L, sizeof(rproto::Decoder*));
        *p = new rproto::Decoder(loader);
        luaL_getmetatable(L, "red.rproto.Decoder");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int ldelDecoder(lua_State* L) {
        rproto::Decoder** p = (rproto::Decoder**)lua_touserdata(L, 1);
        delete *p;
        *p = nullptr;
        return 0;
    }

    static int lByteArray_wLStr(lua_State* L) {
        rproto::ByteArray* bytes = *(rproto::ByteArray**)lua_touserdata(L, 1);
        size_t sz;
        auto ppack = luaL_checklstring(L, 2, &sz);
        bytes->wBytes((rproto::byte*)ppack, sz);
        return 0;
    }

    static int lByteArray_toLStr(lua_State* L) {
        rproto::ByteArray* bytes = *(rproto::ByteArray**)lua_touserdata(L, 1);
        lua_pushlstring(L, (char*)(bytes->first()), bytes->size());
        return 1;
    }

    static int lLoader_setup(lua_State* L) {
        rproto::Loader* loader = *(rproto::Loader**)lua_touserdata(L, 1);
        auto path = lua_tostring(L, 2);
        loader->setProtoPath(path);
        loader->loadAllProtos();
        return 0;
    }

    static int lEncoder_encode(lua_State* L) {
        rproto::Encoder* encoder = *(rproto::Encoder**)lua_touserdata(L, 1);
        rproto::ByteArray* bytes = *(rproto::ByteArray**)lua_touserdata(L, 2);
        std::string name(lua_tostring(L, 3));
        rproto::Map dict;
        tableToMap(L, 4, &dict);
        int result = encoder->encode(bytes, name, dict);
        lua_pushinteger(L, result);
        return 1;
    }

    static int lDecoder_decode(lua_State* L) {
        rproto::Decoder* decoder = *(rproto::Decoder**)lua_touserdata(L, 1);
        rproto::ByteArray* bytes = *(rproto::ByteArray**)lua_touserdata(L, 2);
        std::string name;
        rproto::Map dict;
        int result = decoder->decode(*bytes, name, &dict);
        lua_pushinteger(L, result);
        lua_pushstring(L, name.c_str());
        mapToTable(L, dict);
        return 3;
    }

    static int lprint_bytes(lua_State* L) {
        rproto::ByteArray* bytes = *(rproto::ByteArray**)lua_touserdata(L, 1);
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

        struct luaL_Reg f_ByteArray[] = {
            {"wLStr", lByteArray_wLStr},
            {"toLStr", lByteArray_toLStr},
            {NULL, NULL}
        };

        struct luaL_Reg f_Loader[] = {
            {"setup", lLoader_setup},
            {NULL, NULL}
        };

        struct luaL_Reg f_Encoder[] = {
            {"encode", lEncoder_encode},
            {NULL, NULL}
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
        luaL_setfuncs(L, f_ByteArray, 0);

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
