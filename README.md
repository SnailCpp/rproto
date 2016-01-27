#rproto
- 简单的协议序列化和反序列化工具，作用类似protobuf，但是功能非常简单，针对游戏设计，游戏与常规app不同，rproto完全不考虑兼容旧协议
- 目前是C++编写，目标是绑定到lua，当然也可以绑定到其他语言或者直接用C++调用

## 描述一个协议类似这样：
	{
		"name": "Student",
		"enum": {
			"Boy": 1
			"Girl": 2
		},
		"field": [
			"num int",
			"name string",
			"gender enum",
			"hobby list string",
			"score list list int",
			"parent struct Person"
		]
	}

- 目前是使用json格式描述协议，在Loader中解析后会释放json。如果不想使用json，可以很容易的修改Loader的代码，替换为自己的格式
- 字段使用字符串，后置类型，空格隔开，空格必须1个，另外json不允许写注释。我提供了ProtoMin工具用来格式化代码和去掉注释
- 目前提供的类型有struct，list，enum，int，string。int是32位无符号，string以'\0'表示结束，扩展基本类型非常容易，但是目测这些类型足够了

## lua调用类似这样：
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
	local dd = decoder:decode(bytes, "Proto_Test")
	print(dd.id, dd.name, dd.gender)
	for k,v in ipairs(dd.test_v) do
		for k1, v1 in ipairs(v) do
			print(v1)
		end
	end


*目前只是alpha版，使用起来可能不够方便，只供学习研究使用，一定不要用于生产环境！*
