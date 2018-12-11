use mysql;

set @cnt = 0;
select count(*) into @cnt from user where user = 'root' and host = '%';
update user set host = '%' where user = 'root' and @cnt = 0;

flush privileges;

drop database if exists surgery;
create database  surgery;
use surgery;

create table bindings
(
	tag_id         varchar(16) not null primary key,
	patient_id     int not null
) DEFAULT CHARSET=utf8;

create table  temperature (
	id            int NOT NULL primary key AUTO_INCREMENT,
	tag_id        varchar(16) not null,
	reader_id     varchar(20) not null,
	collect_time  timestamp not null,
	temperature   int not null,
	tag_name      varchar(16) not null,
	grid_index    int not null
) DEFAULT CHARSET=utf8;





