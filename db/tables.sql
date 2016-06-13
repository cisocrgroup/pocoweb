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
)
