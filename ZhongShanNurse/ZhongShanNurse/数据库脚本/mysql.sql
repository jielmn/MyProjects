drop database if exists telemed_temp;
create database  telemed_temp;
use telemed_temp;

-- 病人信息
create table patients ( id int not null primary key AUTO_INCREMENT, str_id varchar(32) not null unique, name varchar(32) not null, bed_no varchar(32) not null, gender int not null default 0, in_hos int not null default 0 ) DEFAULT CHARSET=utf8;
-- 护士信息
create table nurses ( id int not null primary key AUTO_INCREMENT, str_id varchar(32) not null unique, name varchar(32) not null, card_no varchar(32) null unique ) DEFAULT CHARSET=utf8;
-- tag
create table tags ( id varchar(32) not null primary key, patient_id int not null ) DEFAULT CHARSET=utf8;
-- 温度
create table temperature_data ( id int not null primary key AUTO_INCREMENT, patient_id varchar(32) not null, nurse_id varchar(32) not null, reader_id varchar(32) not null, gen_time datetime not null, temp_data int not null ) DEFAULT CHARSET=utf8;


