/*drop database if exists pocweb;*/
create database if not exists pocoweb
  default character set utf8
  default collate utf8_general_ci;
use pocoweb;

drop table if exists users;
create table users (
       id int not null primary key auto_increment,
       name varchar(255) not null,
       email varchar(255) not null unique,
       institute varchar(255) not null,
       passwd varchar(255) not null,
       active boolean not null default true
);

drop table if exists books;
create table books (
	bookid int not null primary key auto_increment,
	owner int references users(id),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255),
	uri varchar(255),
	npages int
);

drop table if exists pages;
create table pages (
	bookid int references books(bookid),
	pageid int,
	nlines int,
	imagepath varchar(255) not null,
	x0 int,
	x1 int,
	y0 int,
	y1 int,
	primary key (bookid, pageid)
);

drop table if exists linesx;
create table linesx (
	bookid int references books(bookid),
	pageid int references pages(pageid),
	lineid int,
	line varchar(255),
	cuts varchar(1024),
	x0 int,
	x1 int,
	y0 int,
	y1 int,
	primary key (bookid, pageid, lineid)
);
	
drop table if exists packages;
create table packages (
	packageid int not null primary key auto_increment,
	bookid int references books(bookid),
	firstpage int,
	lastpage int
);

drop table if exists bookperms;
create table bookperms (
	packageid int references packages(packageid),
	userid int references users(id),
	primary key (packageid, userid)
);


