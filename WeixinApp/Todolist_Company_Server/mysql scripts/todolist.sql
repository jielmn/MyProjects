drop database if exists todo_list_company;
create database  todo_list_company;
use todo_list_company;

-- 是否开放注册
create table config ( id int primary key not null auto_increment, register_enable int not null ) DEFAULT CHARSET=utf8;
insert into config (register_enable) values ( 1 );

-- 用户表
-- open_id是微信给的用户id, 28个字节
-- name是获取的微信的用户名，例如安安爸
-- nickname是我们通过后台给出的真实姓名，例如 安安爸==某某
-- avatar_url是微信的头像地址
create table users ( open_id varchar(64) primary key not null, name varchar(32) not null, nickname varchar(32) not null, avatar_url varchar(128) not null  ) DEFAULT CHARSET=utf8;

-- content 清单的内容
-- start_time是制定清单的时间
-- is_complete指示清单是否完成
-- owner_id是用户的id
create table todolist_items ( item_id int primary key not null auto_increment, content varchar(128) not null, start_time datetime, end_time datetime, is_complete int not null default 0, owner_id varchar(64) not null  ) DEFAULT CHARSET=utf8;
