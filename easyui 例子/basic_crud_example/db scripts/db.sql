drop database if exists test;
create database test;
use test;


create table users ( id int primary key not null auto_increment, firstname varchar(16) not null, lastname varchar(16) not null, phone varchar(16) not null, email varchar(16) not null ) DEFAULT CHARSET=utf8;