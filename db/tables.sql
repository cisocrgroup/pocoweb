-- drop database if exists pocweb;
-- create database if not exists pocoweb
-- default character set utf8
-- default collate utf8_general_ci;
-- use pocoweb;

create database if not exists pocoweb character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists users (
    id integer not null primary key auto_increment,
	name varchar(255) not null,
	email varchar(255) not null unique,
	institute varchar(255) not null,
	hash varchar(128),
	salt varchar(64),
	admin boolean default(false) not null
);
alter table users convert to character set utf8mb4 collate utf8mb4_unicode_ci;
alter table users change name name varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table users change email email varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table users change institute institute varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists sessions (
	auth char(10) not null unique,
	userid integer not null references users(id),
	expires integer not null
);
alter table sessions convert to character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists books (
	bookid int not null unique references projects(id),
	year int,
	title varchar(100) not null,
	author varchar(100) not null,
	description varchar(255) not null,
	uri varchar(255) not null,
	profilerurl varchar(255) not null,
	histpatterns varchar(255) not null,
	directory varchar(255) not null,
	lang varchar(50) not null,
	profiled boolean default(false) not null,
	extendedlexicon boolean default(false) not null,
	postcorrected boolean default(false) not null
);
alter table books convert to character set utf8mb4 collate utf8mb4_unicode_ci;
alter table books change title title varchar(100) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table books change author author varchar(100) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table books change description description varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table books change directory directory varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table books change lang lang varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists projects (
	id int not null unique primary key auto_increment,
	origin int,
	owner int references users(id),
	pages int
);
alter table projects convert to character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists project_pages (
	projectid int not null references projects(id),
	pageid int not null references pages(pageid),
	nextpageid int not null references pages(pageid),
	prevpageid int not null references pages(pageid),
	primary key (projectid, pageid)
);
alter table project_pages convert to character set utf8mb4 collate utf8mb4_unicode_ci;

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
alter table pages convert to character set utf8mb4 collate utf8mb4_unicode_ci;
alter table pages change imagepath imagepath varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;
alter table pages change ocrpath ocrpath varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;

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
alter table textlines convert to character set utf8mb4 collate utf8mb4_unicode_ci;
alter table textlines change imagepath imagepath varchar(255) character set utf8mb4 collate utf8mb4_unicode_ci;

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
alter table contents convert to character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists types (
	id int not null unique primary key auto_increment,
	typ varchar(50) unique not null
);
alter table types convert to character set utf8mb4 collate utf8mb4_unicode_ci;
alter table types change typ typ varchar(50) character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists profiles (
	bookid int references books(bookid),
	timestamp bigint not null,
	primary key (bookid)
);
alter table profiles convert to character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists suggestions (
	id int not null unique primary key auto_increment,
	bookid int references books(bookid),
	tokentypid int references types(id),
	suggestiontypid int references types(id),
	moderntypid int references types(id),
	dict varchar(50) not null,
	histpatterns varchar(50) not null,
	ocrpatterns varchar(50) not null,
	weight double not null,
	distance int not null,
	topsuggestion boolean not null
);
alter table suggestions convert to character set utf8mb4 collate utf8mb4_unicode_ci;

/* (3 * 4)^ + 1^^ + (3 * 4)^ = 25 */
/* ^: max utf length of a pattern with maximal 3 characters */
/* ^^: length of separator `:` */
create table if not exists errorpatterns (
	suggestionid int references suggestions(id),
	bookid int references books(bookid),
	pattern varchar(25),
	ocr boolean not null
);
alter table errorpatterns convert to character set utf8mb4 collate utf8mb4_unicode_ci;
/*alter table errorpatterns change pattern pattern varchar(25) character set utf8mb4 collate utf8mb4_unicode_ci;*/

create table if not exists typcounts (
	   bookid int references books(bookid),
	   typid int references types(id),
	   counts int not null,
	   primary key (bookid, typid)
);

create table if not exists adaptivetokens (
	bookid int references books(bookid),
	typid int references types(id),
	primary key (bookid, typid)
);
alter table adaptivetokens convert to character set utf8mb4 collate utf8mb4_unicode_ci;

create table if not exists autocorrections (
   bookid int references books(bookid),
   pageid int references pages(pageid),
   lineid int references pages(lineid),
   tokenid int not null,
   ocrtypid int references types(id),
   cortypid int references types(id),
   taken boolean not null,
   primary key (bookid, pageid, lineid, tokenid)
);

drop table if exists status;
create table if not exists status (
	id int not null unique primary key,
	text varchar(15) not null
);
alter table status convert to character set utf8mb4 collate utf8mb4_unicode_ci;
insert ignore into status (id,text)
values
	(0,'failed'),
	(1,'running'),
	(2,'done'),
	(3,'empty'),
	(4,'profiled'),
	(5,'post-corrected'),
	(6,'extended-lexicon'),
	(7,'profiled-with-el');

drop table if exists jobs;
create table if not exists jobs (
	   id INT NOT NULL UNIQUE PRIMARY KEY REFERENCES books(bookid),
	   statusid INT NOT NULL REFERENCES status(id),
	   text varchar(50) NOT NULL,
	   timestamp INT(11) NOT NULL
);

/* views */
drop view if exists content_line_view;
create view content_line_view as
	   select c.*,
	   		  l.imagepath,
			  l.lleft,
			  l.lright,
			  l.ltop,
			  l.lbottom
	   from contents c
	   join textlines l on
	   		c.bookid = l.bookid and
			c.pageid = l.pageid and
			c.lineid = l.lineid;

drop view if exists job_status_project_view;
create view job_status_project_view as
	   select j.id,
	   		  j.statusid,
			  j.text as jobname,
			  j.timestamp,
	   		  s.text as statusname,
			  p.id as projectid
	   from jobs j
	   join status s on
	   		j.statusid = s.id
	   join projects p on
	   		j.id = p.origin;
