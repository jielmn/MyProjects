require("bit")

utf8=true;
description="发送温度数据(浮点型)到串口";
-- 参数列表，格式是：参数名，类型，值，缺省参数值
params = { 
			{"温度",      "edit",  "32.56"},
			{"测试参数1", "combo", {"选项1", "选项2", "选项3"},  1 }
		};
nosend = false;

-- t[1] 是第一个参数: 温度
-- t[2] 是第二个参数: 测试参数1
-- 返回：错误码，错误消息，待发送的数据
--   也可以: 待发送的数据
function send(t)
  -- 参数序号从1开始，不是0
  local temperature = tonumber(t[1]);			
  local p2 = t[2];
  
  -- 如果温度不是数字，则设为温度0
  if ( temperature == nil )
  then
      temperature = 0;
  end
  
  -- 32.56 math.modf 结果为 32, 0.56
  local a,b = math.modf(temperature);
  local a1 = bit.band(a, 0xff);
  local b1 = bit.band(b*100, 0xff);
  
  -- 构成返回字节流
  local send_data = string.char( 0x55, 0x1E, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 
                                 0xE2, 0x8C, 0x8A, 0x8D, 0x8B, 0x5F, 0x5C, 0x6D, 
								 a1,   b1,   p2,   0x00, 0x00, 0x00, 0xE0, 0x04, 
								 0x02, 0x8D, 0x5A, 0x6C, 0x4B, 0x3F, 0x00, 0xFF );
  
  return 0,"OK",send_data;
  -- 或者
  -- return send_data;
end

formattype="text";
maxitemscnt = 20;
itemheight=30;

-- 返回值：消耗的字节数，格式字符串，key值（确定唯一块），背景色，文字颜色
function receive(data)
  if ( string.len(data) < 13 )
  then
    return 0,"0",{},0,0;
  end
  
  local beat = string.byte(data,3);
  local beatV = string.byte(data,4);
  local oxy = string.byte(data,5);
  local oxyV = string.byte(data,6);
  local temp = string.byte(data,7) + string.byte(data,8) / 100;
  local body = string.byte(data,9);
  local freq = string.byte(data,10);
  local machine = string.byte(data,11);
  local s = string.format("心率=%d,心率有效位=%d,血氧饱和度=%d,血氧有效位=%d,体温=%.2f℃,人体姿势=%d,通信频率=%d,设备编号=%d", beat,beatV,oxy,oxyV,temp,body,freq,machine);
  return 13,s,{},0,0;
end