require("bit")

channels = {{1,"RED"},{2,"IR"}}

local r = 1;

-- 返回值：消耗的字节数，{ {通道号1, 数值1}, {通道号2, 数值2}, {通道号3, 数值3} ... } 
--     数值用C语言表示法： ( 字节1 << 24) | 字节2 << 16) | 字节3 << 8) | 字节4 )
function receive(data)
  -- 如果字节数不够5，则返回 0, {}，表示未处理数据
  if ( string.len(data) < 5 )
  then
    return 0,{};
  end
  
  -- 注意下标从1开始,string.byte(data,1)表示第一个字节，和C的从0开始不同。
  local b1  = string.byte(data,2);
  local b2  = string.byte(data,3);
  local b3  = string.byte(data,4);
  local b4  = string.byte(data,5);
  
  local a1 = bit.lshift( b1, 24 );
  local a2 = bit.lshift( b2, 16 );
  local a3 = bit.lshift( b3, 8 );
  
  local c = bit.bor( bit.bor(  bit.bor(a1, a2), a3 ), b4 );
  
  if( r == 1 )
  then
    r = 2;
  else
    r = 1;
  end
  
  return 5, { {r,c} } ;
end