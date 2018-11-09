use mysql;
update user set host = '%' where user = 'root';
flush privileges;

drop database if exists luhe_temp_1;
create database  luhe_temp_1;
use luhe_temp_1;

/*
create table PATIENTINFO
(
  patientid     VARCHAR(16) not null primary key,
  patientname   VARCHAR(16) not null,
  gender        CHAR(1) not null,
  bedid         VARCHAR(16),
  age           tinyint not null
) DEFAULT CHARSET=utf8;

create table TEMPERMONITOR
(
  id         int not null primary key,
  readerid   VARCHAR(16) not null,
  tagid      VARCHAR(8) not null,
  temper     int not null,
  mtime      timestamp not null,
  nurseid    VARCHAR(16),
  bodyindex  tinyint not null default 0,
  islast     tinyint not null default 0,
  isbind     tinyint not null default 0
)DEFAULT CHARSET=utf8;

create table PATIENTRELTAG
(
  id         int not null primary key,
  patientid  VARCHAR(16) not null,
  tagid      VARCHAR(8) not null,
  reltime    timestamp,
  unique( patientid, tagid )
)DEFAULT CHARSET=utf8;

create table NURSEINFO
(
  nurseid    varchar(16) not null primary key,
  name       varchar(16) not null
)DEFAULT CHARSET=utf8;

create table STATION
(
	ip       varchar(16) not null primary key,
	name     varchar(32) not null,
	atime    timestamp not null
)DEFAULT CHARSET=utf8;
*/





