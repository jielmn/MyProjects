create database telemedoa;
use telemedoa;
create table proceinfo ( id int not null primary key, procetype int not null, proceman varchar(50) not null, procetime date not null, tagbaseid varchar(4000) not null, proce_batch_id varchar(16) not null );
create table tagsinfo  ( tagid varchar(16) not null primary key, proceinfo_id int not null );
create table staff     ( id int not null primary key, stfid varchar(100) not null unique, stfname varchar(100) not null, stfmobil varchar(100), stfemail varchar(100), ext varchar(100), 
                         ext1 varchar(100), ext2 varchar(100), ext3 varchar(100), ext4 varchar(100) );