require("bit")

utf8=true;
description="根据电力线载波通讯协议，设置相关设备";
-- 参数列表，格式是：参数名，类型，值，缺省参数值
params = { 
			{"模块类型",  "combo", {"带LORA", "不带LORA"},  0 },
			{"设备UID",   "edit",  "" }
		};
nosend = false;

-- t[1] 是第一个参数:
-- t[2] 是第二个参数:

-- 协议定义如下
-- ---------------------------------------------------------------------------------
-- 串口发送 55 01 E2 00 00 01 DD AA
-- 55 起始码
-- 01 选择带LORA模块的设备配置UID， 02 选择不带LORA模块的设备配置UID
-- E2 00 00 01  设备唯一性标识UID
-- DD AA  结束码
-- 串口返回数据：如果是发送55 01 E2 00 00 01 DD AA,串口返回 01 4F 4B，表示设置成功
-- 如果是发送55 02 E2 00 00 01 DD AA,串口返回 02 4F 4B，表示设置成功
-- ----------------------------------------------------------------------------------
function send(t)
  -- 参数序号从1开始，不是0
  
  local nLora = t[1] + 1;   -- combo 对应的是整型，第几项被选中
  local uid   = string.lower(t[2]);   -- edit 对应的是字符串
  
  -- 检测uid格式
  local len = string.len(uid);
  if ( len ~= 8 )
  then
    return 1,"uid的长度不是8","";
  end
  
  for i=1,len do
	local ch = string.byte(uid,i)
	if ( not ((ch >= 97 and ch <= 102) or (ch >=48 and ch<= 57)  ) )
    then
      return 1,"uid格式为: xxxxxxxx(x为0~9,a~f)","";
    end
  end
  

  -- 构成返回字节流
  local s1 = string.char( 0x55, nLora );
  local s2 = string.char( 0xdd, 0xaa );
  
  return s1..str2hex(uid)..s2;
end

function str2hex(str)
	--判断输入类型	
	if (type(str)~="string") then
	    return nil,"str2hex invalid input type"
	end
	--滤掉分隔符
	str=str:gsub("[%s%p]",""):upper()
	--检查内容是否合法
	if(str:find("[^0-9A-Fa-f]")~=nil) then
	    return nil,"str2hex invalid input content"
	end
	--检查字符串长度
	if(str:len()%2~=0) then
	    return nil,"str2hex invalid input lenth"
	end
	--拼接字符串
	local index=1
	local ret=""
	for index=1,str:len(),2 do
	    ret=ret..string.char(tonumber(str:sub(index,index+1),16))
	end
 
	return ret
end

formattype="text";
maxitemscnt = 10;
itemheight=30;

-- 返回值：消耗的字节数，格式字符串，key值（确定唯一块），背景色，文字颜色, 是否忽略(当消耗数大于0时)
function receive(data)
  local len = string.len(data);
  if ( len < 3 )
  then
    return 0,"0",{},0,0,false;
  end
  
  local nLora = string.byte(data,1);
  if ( nLora == 1 or nLora == 2 ) then
    if ( string.byte(data,2) == 79 and string.byte(data,3) == 75 )
    then
	  return 3, nLora.." OK",{},0,0,false;
    else
      return 3,"ERROR",{},0,0,false;
    end
  elseif (nLora == 0x66) then
    if ( len >= 8 ) then
		return 8, "温度数据",{},0,0,true;
	else
		return 0, "0",{},0,0,false;
	end
  end
  
end