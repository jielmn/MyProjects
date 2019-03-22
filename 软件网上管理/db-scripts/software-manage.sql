drop database if exists software_manage;
create database  software_manage;
use software_manage;

-- 是否开放注册
create table items ( 
	id int primary key not null auto_increment, 
	title varchar(100) not null,
	abstract varchar(200) not null,
	content varchar(10000) not null,
	create_time datetime not null,
	software_path varchar(100) not null,
	cover varchar(100) not null ) DEFAULT CHARSET=utf8;					   
