require("bit")

utf8=true;
description="小型可穿戴设备演示";
-- 参数列表，格式是：参数名，缺省参数值
params = {  };
nosend = true;

formattype="grids";
itemheight=200;
itemwidth=200;

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
  local s = string.format("心率     :%d<n>心率有效位:%d<n>血氧饱和度:%d<n>血氧有效位:%d<n>体温     :%.2f<n>人体姿势  :%d<n>通信频率  :%d<n>设备编号  :%d", beat,beatV,oxy,oxyV,temp,body,freq,machine);
  return 13,s,{freq,machine},0,0;
end