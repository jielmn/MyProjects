drop database if exists easytemp;
create database  easytemp;
use easytemp;					   

-- 是否开放注册
create table config ( id int primary key not null auto_increment, value0 int not null ) DEFAULT CHARSET=utf8;
insert into config (value0) values ( 1 );

-- 用户表
-- open_id是微信给的用户id, 28个字节
-- name是获取的微信的用户名
-- avatar_url是微信的头像地址
create table users ( open_id varchar(64) primary key not null, lastdeviceid varchar(20) not null ) DEFAULT CHARSET=utf8;

-- 体温表
create table temperature( id int primary key not null auto_increment, open_id varchar(64) not null, tagid varchar(20) not null, temp int not null, ctime datetime not null ) DEFAULT CHARSET=utf8;

-- 家庭成员
create table family ( memberid int primary key not null auto_increment, open_id varchar(64) not null, membername varchar(20) ) DEFAULT CHARSET=utf8;

-- 体温贴绑定
create table binding ( tagid varchar(20) not null primary key,  memberid int not null ) DEFAULT CHARSET=utf8;