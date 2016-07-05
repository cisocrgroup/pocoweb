/*drop database if exists pocweb;*/
create database if not exists pocoweb
  default character set utf8
  default collate utf8_general_ci;
use pocoweb;

drop table if exists users;
create table users (
       userid int not null primary key auto_increment,
       name varchar(255) not null,
       email varchar(255) not null unique,
       institute varchar(255) not null,
       passwd varchar(255) not null,
       active boolean not null default true
);

drop table if exists bookdata;
create table bookdata (
	bookdataid int not null primary key auto_increment,
	owner int references users(userid),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255),
	uri varchar(255),
	directory varchar(255)
);

drop table if exists books;
create table books (
	bookid int primary key auto_increment,
	bookdataid int references bokdata(bookdataid),
	firstpage int,
	lastpage int 
);
	

drop table if exists pages;
create table pages (
	bookid int references books(bookid),
	pageid int,
	nlines int,
	imagepath varchar(255) not null,
	pleft int,
	ptop int,
	pright int,
	pbottom int,
	primary key (bookid, pageid)
);

drop table if exists linesx;
create table linesx (
	bookid int references books(bookid),
	pageid int references pages(pageid),
	lineid int,
	lstr varchar(255),
	cuts varchar(1024),
	lleft int,
	ltop int,
	lright int,
	lbottom int,
	primary key (bookid, pageid, lineid)
);
	
drop table if exists bookpermissions;
create table bookpermissions (
	bookid int references books(bookid),
	userid int references users(userid),
	primary key (bookid, userid)
);


