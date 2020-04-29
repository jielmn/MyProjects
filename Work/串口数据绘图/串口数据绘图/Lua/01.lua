require("bit")

channels = {{1,"s1"},{2,"s2"}}


-- 返回值：消耗的字节数，{ {通道号1, 数值1}, {通道号2, 数值2}, {通道号3, 数值3} ... } 
--     数值用C语言表示法： (( 字节1 & 0x03 ) << 16) | (字节2 << 8) | 字节3
--                         (( 字节4 & 0x03 ) << 16) | (字节5 << 8) | 字节6

function receive(data)
  -- 如果字节数不够6，则返回 0, {}，表示未处理数据
  if ( string.len(data) < 6 )
  then
    return 0,{};
  end
  
  -- 注意下标从1开始,string.byte(data,1)表示第一个字节，和C的从0开始不同。
  local b1  = string.byte(data,1);
  local b2  = string.byte(data,2);
  local b3  = string.byte(data,3);
  local b4  = string.byte(data,4);
  local b5  = string.byte(data,5);
  local b6  = string.byte(data,6);
  
  local a1 = bit.lshift( bit.band(b1, 0x03), 16 );
  local a2 = bit.lshift( b2, 8 );
  local a3 = b3;
  
  local a4 = bit.lshift( bit.band(b4, 0x03), 16 );
  local a5 = bit.lshift( b5, 8 );
  local a6 = b6;
  
  local c1 = bit.bor(  bit.bor(a1, a2), a3 );
  local c2 = bit.bor(  bit.bor(a4, a5), a6 );
  
  return 6, { {1,c1}, {2,c2} } ;
end