use mysql;
update user set host = '%' where user = 'root';
flush privileges;

drop database if exists luhe_temp;
create database  luhe_temp;
use luhe_temp;

create table PATIENTINFO
(
  patientid     VARCHAR(16) not null primary key,
  patientname   VARCHAR(16) not null,
  gender        CHAR(1) not null,
  bedid         VARCHAR(16),
  age           tinyint not null
);

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
);

create table PATIENTRELTAG
(
  id         int not null primary key,
  patientid  VARCHAR(16) not null,
  tagid      VARCHAR(8) not null,
  reltime    timestamp,
  unique( patientid, tagid )
);

create table NURSEINFO
(
  nurseid    varchar(16) not null primary key,
  name       varchar(16) not null
);





