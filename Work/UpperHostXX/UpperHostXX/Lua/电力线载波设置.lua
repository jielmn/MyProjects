require("bit")

utf8=true;
description="根据电力线载波通讯协议，设置相关设备";
-- 参数列表，格式是：参数名，类型，值，缺省参数值
params = { 
			{"模块类型",  "combo", {"带LORA", "不带LORA"},  0 },
			{"设备UID",   "edit",  "" }
		};
nosend = false;

-- t[1] 是第一个参数: 温度
-- t[2] 是第二个参数: 测试参数1
-- 串口发送 55 01 E2 00 00 01 DD AA
-- 55 起始码
-- 01 选择带LORA模块的设备配置UID， 02 选择不带LORA模块的设备配置UID
-- E2 00 00 01  设备唯一性标识UID
-- DD AA  结束码
-- 串口返回数据：如果是发送55 01 E2 00 00 01 DD AA,串口返回 01 4F 4B，表示设置成功
-- 如果是发送55 02 E2 00 00 01 DD AA,串口返回 02 4F 4B，表示设置成功

function send(t)
  -- 参数序号从1开始，不是0
  
  local nLora = t[1];
  local uid = t[2];
  
  
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
  
  return send_data;
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