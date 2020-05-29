-- oracle data base
create table temptest_1 ( id int primary key not null, temp float not null, om float not null, iscorrect int not null, testman varchar2(20) not null, timenow date not null );

-- insert into temptest1 values ( 1, 12.34, 12.34, 1, 'he', to_date ( '2007-12-20 18:31:34' , 'YYYY-MM-DD HH24:MI:SS' ) );
-- create table temptest_1 (id int not null primary key, temp float not null, om float not null, iscorrect int not null, testman varchar2(12) not null, timenow timestamp not null);
-- insert into temptest_1 values ( 2, 10.09,  10.123, 1, 'she', SYSDATE );
-- select TO_CHAR(timenow,'YYYY-MM-DD hh24:mi:ss') from temptest_1;