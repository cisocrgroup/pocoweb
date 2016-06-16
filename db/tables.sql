create database if not exists pocoweb;
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
	id int not null primary key auto_increment,
	owner int references users(id),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255),
	uri varchar(255)
);

drop table if exists pages;
create table pages (
	bookid int references books(id),
	pagenumber int,
	nlines int,
	imagepath varchar(255) not null,
	x0 int,
	x1 int,
	y0 int,
	y1 int,
	primary key (bookid, pagenumber)
);

drop table if exists linesx;
create table linesx (
	bookid int references books(id),
	pagenumber int references pages(pagenumber),
	linenumber int,
	string varchar(255),
	cuts varchar(255),
	x0 int,
	x1 int,
	y0 int,
	y1 int,
	primary key (bookid, pagenumber, linenumber)
);
	
drop table if exists packages;
create table packages (
	id int not null primary key auto_increment,
	bookid int references books(id),
	firstpage int,
	len int
);

drop table if exists bookperms;
create table bookperms (
	packageid int references packages(id),
	userid int references users(id),
	primary key (packageid, userid)
);


