select * from big_out a inner join big_pkg b on a.PACKAGE_ID = b.id;
select * from big_pkg;
select * from small_pkg;
select * from small_out;
select * from big_out;

-- 查询大包装
select * from big_pkg where package_id like '%ET20180602%';
-- 查询小包装
select a.PACKAGE_ID, b.id, b.PACKAGE_ID as bigid from small_pkg a inner join big_pkg b on a.BIG_PKG_ID = b.id where b.PACKAGE_ID like '%ET20180602%';
-- 查询大包装出库
select * from big_out a inner join big_pkg b on a.PACKAGE_ID = b.id where b.package_id like '%ET20180602%';
-- 查询小包装出库
select c.PACKAGE_ID as smallid, c.BIG_PKG_ID as bigid, d.PACKAGE_ID as bigpkgid from ( select a.PACKAGE_ID, b.BIG_PKG_ID from small_out a inner join small_pkg b on a.PACKAGE_ID = b.id ) c inner join big_pkg d on c.BIG_PKG_ID = d.ID where d.package_id like '%ET20180602%';



--参数查询
-- 查询大包装
select * from big_pkg where package_id like :batchid;
-- 查询小包装
select a.PACKAGE_ID, b.id, b.PACKAGE_ID as bigid from small_pkg a inner join big_pkg b on a.BIG_PKG_ID = b.id where b.PACKAGE_ID like :batchid;
-- 查询大包装出库
select * from big_out a inner join big_pkg b on a.PACKAGE_ID = b.id where b.package_id like :batchid;
-- 查询小包装出库
select c.PACKAGE_ID as smallid, c.BIG_PKG_ID as bigid, d.PACKAGE_ID as bigpkgid from ( select a.PACKAGE_ID, b.BIG_PKG_ID from small_out a inner join small_pkg b on a.PACKAGE_ID = b.id ) c inner join big_pkg d on c.BIG_PKG_ID = d.ID where d.package_id like :batchid;
--



-- 删除小包装出库
delete from small_out where package_id in ( select c.PACKAGE_ID from ( select a.PACKAGE_ID, b.BIG_PKG_ID from small_out a inner join small_pkg b on a.PACKAGE_ID = b.id ) c inner join big_pkg d on c.BIG_PKG_ID = d.ID where d.package_id like :batchid );
-- 删除大包装出库
delete from big_out where package_id in （select a.package_id from big_out a inner join big_pkg b on a.PACKAGE_ID = b.id where b.package_id like :batchid ）;
-- 删除小包装
delete from small_pkg where id in ( select a.id from small_pkg a inner join big_pkg b on a.BIG_PKG_ID = b.id where b.PACKAGE_ID like :batchid );
-- 删除大包装
delete from big_pkg where package_id like :batchid;

--commit;