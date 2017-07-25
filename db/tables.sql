/*drop database if exists pocweb;*/
create database if not exists pocoweb
default character set utf8
default collate utf8_general_ci;
use pocoweb;

create table if not exists users (
	userid int not null unique primary key auto_increment,
	name varchar(50) not null unique,
	email varchar(50) not null,
	institute varchar(50) not null,
	passwd varchar(100) not null,
	admin boolean not null
);

create table if not exists books (
	bookid int not null unique references projects(projectid),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255),
	uri varchar(255),
	directory varchar(255) not null,
	lang varchar(50) not null
);

create table if not exists projects (
	projectid int not null unique primary key auto_increment,
	origin int,
	owner int references users(userid),
	pages int
);

create table if not exists project_pages (
	projectid int not null references projects(projectid),
	pageid int not null references pages(pageid),
	primary key (projectid, pageid)
);

create table if not exists pages (
	bookid int references books(bookid),
	pageid int,
	imagepath varchar(255) not null,
	ocrpath varchar(255) not null,
	filetype int,
	pleft int,
	ptop int,
	pright int,
	pbottom int,
	primary key (bookid, pageid)
);

create table if not exists textlines (
	bookid int references books(bookid),
	pageid int references pages(pageid),
	lineid int,
	imagepath varchar(255),
	lleft int,
	ltop int,
	lright int,
	lbottom int,
	primary key (bookid, pageid, lineid)
);

create table if not exists contents (
	bookid int references books(bookid),
	pageid int references pages(pageid),
	lineid int references textlines(lineid),
	seq int not null,
	ocr int not null,
	cor int not null,
	cut int not null,
	conf double not null,
	primary key (bookid, pageid, lineid, seq)
);


create table if not exists profiles (
	bookid int references books(bookid),
	timestamp bigint not null,
	primary key (bookid)
);

create table if not exists errortokens (
	bookid int references profiles(bookid),
	errortokenid int not null,
	errortoken varchar(50) not null,
	primary key (bookid, errortokenid)
);

create table if not exists suggestions (
	bookid int references errortokens(bookid),
	errortokenid int references errortokens(errortokenid),
	suggestion varchar(50) not null,
	weight double not null,
	distance int not null
);
