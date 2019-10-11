require("bit")

utf8=true;
description="发送温度数据(浮点型)到串口";
-- 参数列表，格式是：参数名，缺省参数值
params = { {"温度", "32.56"} };

-- t[1] 是第一个参数: 温度
-- t[2] 是第二个参数: 测试参数1
function send(t)
  -- 参数序号从1开始，不是0
  local temperature = tonumber(t[1]);								
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
								 a1,   b1,   0x00, 0x00, 0x00, 0x00, 0xE0, 0x04, 
								 0x02, 0x8D, 0x5A, 0x6C, 0x4B, 0x3F, 0x00, 0xFF );
  
  return send_data;
end