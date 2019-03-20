drop database if exists software_manage;
create database  software_manage;
use software_manage;

-- 是否开放注册
create table items ( 
	id int primary key not null auto_increment, 
	title varchar(32) not null,
	abstract varchar(32) not null,
	content varchar(4000) not null,
	create_time datetime not null,
	software_path varchar(100) not null,
	software_name varchar(32) not null ) DEFAULT CHARSET=utf8;					   
