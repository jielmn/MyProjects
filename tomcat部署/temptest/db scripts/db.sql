-- oracle data base
create table TempTest1 ( id int primary key not null, temp float not null, om float not null, iscorrect int not null, testman varchar2(20) not null, timenow date not null );

-- insert into temptest1 values ( 1, 12.34, 12.34, 1, 'he', to_date ( '2007-12-20 18:31:34' , 'YYYY-MM-DD HH24:MI:SS' ) );