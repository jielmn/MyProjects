drop database global_db if exists global_db;
create database  global_db;
use global_db;

create table config ( id int primary key not null, cfg_0 varchar(256), cfg_1 varchar(256), cfg_3 varchar(256) ) DEFAULT CHARSET=utf8;
-- id为1的表示 telemed公司的光猫的公网地址
insert into config values ( 1, null, null, null );
