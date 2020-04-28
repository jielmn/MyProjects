require("bit")

-- 返回值：消耗的字节数，格式字符串，key值（确定唯一块），背景色，文字颜色
function receive(data)
  if ( string.len(data) < 6 )
  then
    return 0,{};
  end
  
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