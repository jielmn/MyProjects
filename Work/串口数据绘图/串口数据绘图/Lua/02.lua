require("bit")

channels = {{1,"RED"},{2,"IR"}}

local r = 1;

-- 返回值：消耗的字节数，格式字符串，key值（确定唯一块），背景色，文字颜色
function receive(data)
  if ( string.len(data) < 5 )
  then
    return 0,{};
  end
  
  local b1  = string.byte(data,2);
  local b2  = string.byte(data,3);
  local b3  = string.byte(data,4);
  local b4  = string.byte(data,5);
  
  local a1 = bit.lshift( b1, 24 );
  local a2 = bit.lshift( b1, 16 );
  local a3 = bit.lshift( b1, 8 );
  
  local c = bit.bor( bit.bor(  bit.bor(a1, a2), a3 ), b4 );
  
  if( r == 1 )
  then
    r = 2;
  else
    r = 1;
  end
  
  return 5, { {r,c} } ;
end