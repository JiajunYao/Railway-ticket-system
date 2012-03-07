
set storage_engine = InnoDB;

drop database if exists railway_ticket_system;
create database if not exists railway_ticket_system;

use railway_ticket_system;

drop table if exists client;
create table if not exists client(
	id bigint(20) primary key auto_increment,
	name varchar(255) not null,
	password varchar(512) not null,
	register_time timestamp default current_timestamp,
	unique(name)
);


drop table if exists train;
create table if not exists train(
	id bigint(20) primary key auto_increment,
	name varchar(255) not null, -- the name of the train such as G7095
	departure_time timestamp not null, -- the most recent departure time from the initiaing station
	cycle_time integer(10) not null, -- the interval between two adjacent departures
	unique(name)
);

drop table if exists station;
create table if not exists station(
	id bigint(20) primary key auto_increment,
	name varchar(255) not null, -- the name of the station such as Beijing
	unique(name)
);

drop table if exists schedule;
create table if not exists schedule(
	id bigint(20) primary key auto_increment,
	train_id bigint(20) not null,
	station_id bigint(20) not null,
	cost_time integer(10) not null, -- the minutes it costs for "train_id" to travel from initiaing station to "station_id"
	cost_money integer(10) not null, -- the yuans it costs for "train_id" to travel from initiaing station to "station_id"
	foreign key (train_id) references train(id),
	foreign key (station_id) references station(id)
);

drop table if exists seat;
create table if not exists seat(
	id bigint(20) primary key auto_increment,
	name varchar(255) not null, -- the name of the seat such as A21
	train_id bigint(20) not null
);

drop table if exists ticket;
create table if not exists ticket(
	id bigint(20) primary key auto_increment,
	client_id bigint(20) not null,
	train_id bigint(20) not null,
	seat_id bigint(20) not null,
	buy_time timestamp default current_timestamp,
	start_station_id bigint(20) not null,
	end_station_id bigint(20) not null,
	start_time timestamp not null,
	end_time timestamp not null,
	price integer(10) not null,
	foreign key (client_id) references client(id),
	foreign key (train_id) references train(id),
	foreign key (seat_id) references seat(id),
	foreign key (start_station_id) references station(id),
	foreign key (end_station_id) references station(id)
);

-- create user if not exist
grant all on railway_ticket_system.* to 'manager'@'localhost' identified by 'manager';
