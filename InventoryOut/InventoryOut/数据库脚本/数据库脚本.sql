create table agencies ( id int not null primary key, str_id varchar(32) not null unique, name varchar(64) not null, province varchar(32) not null );

create table big_out   ( package_id int not null primary key, out_target_type int not null , out_target_id varchar(100) not null, operator varchar(100) not null, operate_time date not null );

create table small_out ( package_id int not null primary key, out_target_type int not null , out_target_id varchar(100) not null, operator varchar(100) not null, operate_time date not null );