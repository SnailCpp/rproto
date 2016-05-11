#ifndef __PROTO_LUABINDING_H__
#define __PROTO_LUABINGING_H__

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int luaopen_rproto(lua_State* L);
}

#endif
