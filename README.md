#rproto
- 简单的协议序列化和反序列化工具，作用类似protobuf，但是功能非常简单，针对游戏设计，游戏与常规app不同，rproto完全不考虑兼容旧协议
- 目前是C++编写，目标是绑定到lua，当然也可以绑定到其他语言或者直接用C++调用

## 描述一个协议：
	Student 100 {
		enum {
			Boy 1
			Girl 2
		}
		num int
		name string
		gender enum
		hobby list string
		score list list int
		parent struct Person
	}

## 描述一个RPC：
	RPC Login {
		C2S 120 {
			account string
			password string
		}
		S2C 121 {
			enum {
				Success 1
				Fail 2
			}
			result enum
		} 
	}

- 如果不喜欢现在的描述格式，可以很容易的修改Loader的代码，替换为自己的格式
- 每个字段一行，后置类型，空格隔开。暂时不允许写注释，以后会添加注释语法
- 目前提供的类型有struct，list，enum，int，int32, int16, int8, string。int无符号，string以'\0'表示结束，扩展基本类型非常容易，但是目测这些类型足够了

## lua调用：
	local rproto = require "rproto"

	local loader = rproto.newLoader()
	loader:setup("/home/red/Documents/lua_test/proto");

	-- encode
	local encoder = rproto.newEncoder(loader)
	local bytes = rproto.newByteArray()
	encoder:encode(bytes, "Proto_Test", {
		id = 123,
		name = "Red",
		gender = "Boy",
		test_v = {
			{100, 101},
			{200, 201, 202}
		}
	})

	-- decode
	local decoder = rproto.newDecoder(loader)
	local proto_name, dd = decoder:decode(bytes)
	print(proto_name, dd.id, dd.name, dd.gender)
	for k,v in ipairs(dd.test_v) do
		for k1, v1 in ipairs(v) do
			print(v1)
		end
	end

