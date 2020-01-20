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
create table users ( open_id varchar(64) primary key not null ) DEFAULT CHARSET=utf8;

-- 体温表
create table temperature( id int primary key not null auto_increment, open_id varchar(64) not null, tagid varchar(20) not null, temp float(3,1) not null, ctime datetime not null, lat double(9,6) null, lng double(9,6) null ) DEFAULT CHARSET=utf8;

-- 家庭成员
create table family ( memberid int primary key not null auto_increment, open_id varchar(64) not null, membername varchar(20) ) DEFAULT CHARSET=utf8;

-- 体温贴绑定
create table binding ( tagid varchar(20) not null primary key,  memberid int not null ) DEFAULT CHARSET=utf8;

-- 地区上报数目
create table areaupload ( adcode int primary key not null, areaname varchar(20) not null, yesterdayupload int not null default 0, yesterdayadded int not null default 0) DEFAULT CHARSET=utf8;
insert into areaupload(adcode, areaname) values (110101, '东城区' );
insert into areaupload(adcode, areaname) values (110102, '西城区' );
insert into areaupload(adcode, areaname) values (110105, '朝阳区' );
insert into areaupload(adcode, areaname) values (110106, '丰台区' );
insert into areaupload(adcode, areaname) values (110107, '石景山区' );
insert into areaupload(adcode, areaname) values (110108, '海淀区' );
insert into areaupload(adcode, areaname) values (110109, '门头沟区' );
insert into areaupload(adcode, areaname) values (110111, '房山区' );
insert into areaupload(adcode, areaname) values (110112, '通州区' );
insert into areaupload(adcode, areaname) values (110113, '顺义区' );
insert into areaupload(adcode, areaname) values (110114, '昌平区' );
insert into areaupload(adcode, areaname) values (110115, '大兴区' );
insert into areaupload(adcode, areaname) values (110116, '怀柔区' );
insert into areaupload(adcode, areaname) values (110117, '平谷区' );
insert into areaupload(adcode, areaname) values (110118, '密云区' );
insert into areaupload(adcode, areaname) values (110119, '延庆区' );
