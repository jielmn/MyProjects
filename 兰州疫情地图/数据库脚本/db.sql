drop database if exists epidemic_demo;
create database epidemic_demo;
use epidemic_demo;

-- id 限制为 1 - 7
create table fever_count ( id int not null primary key, cnt_1 int not null, cnt_2 int not null ) DEFAULT CHARSET=utf8;
insert into fever_count values ( 1,  45, 60 );
insert into fever_count values ( 2,  47, 58 );
insert into fever_count values ( 3,  49, 56 );
insert into fever_count values ( 4,  51, 55 );
insert into fever_count values ( 5,  52, 54 );
insert into fever_count values ( 6,  53, 53 );
insert into fever_count values ( 7,  52, 54 );
insert into fever_count values ( 8,  51, 53 );
insert into fever_count values ( 9,  53, 52 );
insert into fever_count values ( 10, 54, 51 );
insert into fever_count values ( 11, 55, 50 );
insert into fever_count values ( 12, 55, 50 );
insert into fever_count values ( 13, 57, 49 );
insert into fever_count values ( 14, 59, 48 );
insert into fever_count values ( 15, 62, 47 );
insert into fever_count values ( 16, 62, 47 );
insert into fever_count values ( 17, 61, 47 );
insert into fever_count values ( 18, 60, 45 );
insert into fever_count values ( 19, 59, 44 );
insert into fever_count values ( 20, 58, 43 );
insert into fever_count values ( 21, 57, 44 );
insert into fever_count values ( 22, 55, 45 );
insert into fever_count values ( 23, 52, 46 );
insert into fever_count values ( 24, 49, 48 );
insert into fever_count values ( 25, 50, 48 );
insert into fever_count values ( 26, 51, 49 );
insert into fever_count values ( 27, 49, 50 );
insert into fever_count values ( 28, 47, 50 );
insert into fever_count values ( 29, 45, 49 );
insert into fever_count values ( 30, 44, 48 );

create table instant_msg ( id int not null primary key, content varchar(200) not null ) DEFAULT CHARSET=utf8; 
insert into instant_msg values ( 1, "" );
insert into instant_msg values ( 2, "" );
insert into instant_msg values ( 3, "" );
insert into instant_msg values ( 4, "" );
insert into instant_msg values ( 5, "" );
insert into instant_msg values ( 6, "" );
insert into instant_msg values ( 7, "2018-07-06 11:36:43 城关区医院新上报3案例" );
insert into instant_msg values ( 8, "2018-07-06 13:05:17 七里河区医院新上报5案例" );

-- severity 严重程度：1, normal 2, warning, 3 serious
create table hospital_dot ( id int not null primary key, lng double not null, lat double not null, severity int not null, value int not null, info varchar(200) not null ) DEFAULT CHARSET=utf8;
insert into hospital_dot values ( 1, 103.778127,36.082669, 1, 10, '兰州市第一人名医院' );
insert into hospital_dot values ( 2, 103.829062,36.073657, 2, 18, '兰州市第二人名医院' );
insert into hospital_dot values ( 3, 103.746119,36.051477, 3, 18, '兰州市第三人名医院' );
insert into hospital_dot values ( 4, 103.265398,36.740625, 1, 10, '永登县中医医院' );
insert into hospital_dot values ( 5, 103.955433,36.334388, 1, 10, '皋兰县人民医院' );
insert into hospital_dot values ( 6, 104.120215,35.853646, 2, 18, '榆中县第一人民医院' );
insert into hospital_dot values ( 7, 102.860378,36.35192,  1, 10, '红古区人民医院' );
insert into hospital_dot values ( 8, 103.645842,36.09319,  3, 18, '西固区区人民医院' );




-- severity 严重程度：1, normal 2, warning, 3 serious
create table vaccinum_dot ( id int not null primary key, lng double not null, lat double not null, severity int not null, value int not null, info varchar(200) not null ) DEFAULT CHARSET=utf8;
insert into vaccinum_dot values ( 1, 103.315398,36.790625, 1, 18, '疫苗站1:<br>疫苗充足' );
insert into vaccinum_dot values ( 2, 104.005433,36.384388, 2, 18, '疫苗站2:<br>疫苗缺货95份' );
insert into vaccinum_dot values ( 3, 103.779062,36.183657, 3, 18, '疫苗站3:<br>疫苗缺货207份' );
insert into vaccinum_dot values ( 4, 102.910378,36.33192,  1, 18, '疫苗站4:<br>疫苗充足' );
insert into vaccinum_dot values ( 5, 104.170215,35.803646, 2, 18, '疫苗站5:<br>疫苗缺货68份' );


-- 手足口的点
create table epidemic_dot ( id int not null primary key, lng double not null, lat double not null );
insert into epidemic_dot values ( 1, 103.778127,36.082669 );
insert into epidemic_dot values ( 2, 103.829062,36.073657 );

-- 手足口的散点图
create table epidemic_show ( dot_id int not null, day int not null, type int not null, value int not null );
insert into epidemic_show values ( 1, 1, 1, 10 );
insert into epidemic_show values ( 2, 1, 1, 10 );
insert into epidemic_show values ( 1, 2, 1, 10 );

-- 手足口住院人数
create table  epidemic_hos ( day int not null primary key, count int not null );
insert into  epidemic_hos values ( 1, 20 );
insert into  epidemic_hos values ( 2, 20 );
insert into  epidemic_hos values ( 3, 40 );

