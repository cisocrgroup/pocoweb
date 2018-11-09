/*drop database if exists pocweb;*/
/*create database if not exists pocoweb*/
/*default character set utf8*/
/*default collate utf8_general_ci;*/
/*use pocoweb;*/

create table if not exists users (
    id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(255) NOT NULL UNIQUE,
	email VARCHAR(255) NOT NULL,
	institute VARCHAR(255) NOT NULL,
	hash BINARY(64),
	salt BINARY(32),
	admin BOOLEAN DEFAULT(false) NOT NULL
);

CREATE TABLE IF NOT EXISTS BOOKS (
	BOOKID INT NOT NULL UNIQUE references projects(projectid),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255),
	uri varchar(255),
	profilerurl varchar(255),
	directory varchar(255) not null,
	lang varchar(50) not null
);

create table if not exists projects (
	id int not null unique primary key auto_increment,
	origin int,
	owner int references users(id),
	pages int
);

create table if not exists project_pages (
	projectid int not null references projects(id),
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

create table if not exists types (
	bookid int references books(bookid),
	typid int,
	string varchar(50),
	primary key (bookid, typid)
);

create table if not exists profiles (
	bookid int references books(bookid),
	timestamp bigint not null,
	primary key (bookid)
);

create table if not exists suggestions (
	suggestionid int not null unique primary key auto_increment,
	bookid int references books(bookid),
	pageid int references pages(pageid),
	lineid int references textlines(lineid),
	tokenid int not null,
	typid int references types(typid),
	suggestiontypid int references types(typid),
	weight double not null,
	distance int not null,
	topsuggestion boolean not null
);

/* (3 * 4)^ + 1^^ + (3 * 4)^ = 25*/
/* ^: max utf length of a pattern with maximal 3 characters */
/* ^^: lenght of separator `:` */
create table if not exists errorpatterns (
	suggestionid int references suggestions(suggestionid),
	bookid int references books(bookid),
	pattern varchar(25),
	ocr boolean not null
);

create table if not exists adaptivetokens (
	bookid int references books(bookid),
	typid int references types(typid),
	primary key (bookid, typid)
);
