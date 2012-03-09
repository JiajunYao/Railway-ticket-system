
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
	departure_time time not null, -- the every day departure time from the initiaing station
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
	departure_time timestamp not null,
	arrival_time timestamp not null,
	price integer(10) not null,
	foreign key (client_id) references client(id),
	foreign key (train_id) references train(id),
	foreign key (seat_id) references seat(id),
	foreign key (start_station_id) references station(id),
	foreign key (end_station_id) references station(id)
);

-- create user if not exist
grant all on railway_ticket_system.* to 'manager'@'localhost' identified by 'manager';

-- add some data
insert into client (name, password) values ('client', 'client');
insert into client (name, password) values ('manager', 'manager');

insert into train (name, departure_time ) values ('G7059', '09:00:00'); -- id should be 1
insert into train (name, departure_time ) values ('G730', '07:00:00'); -- id should be 2

insert into station (name) values ('Nanjing'); -- id should be 1
insert into station (name) values ('Changzhou');
insert into station (name) values ('Wuxi');
insert into station (name) values ('Suzhou');
insert into station (name) values ('Shanghai');

insert into station (name) values ('Xuzhou');
insert into station (name) values ('Jinan');
insert into station (name) values ('Tianjin');
insert into station (name) values ('Beijing');

-- Nanjing -> Changzhou -> Wuxi -> Suzhou -> Shanghai
insert into schedule (train_id, station_id, cost_time, cost_money) values (1, 1, 0, 0);
insert into schedule (train_id, station_id, cost_time, cost_money) values (1, 2, 60, 60);
insert into schedule (train_id, station_id, cost_time, cost_money) values (1, 3, 90, 90);
insert into schedule (train_id, station_id, cost_time, cost_money) values (1, 4, 120, 120);
insert into schedule (train_id, station_id, cost_time, cost_money) values (1, 5, 150, 150);

-- Beijing -> Tianjin -> Jinan -> Xuzhou -> Nanjing -> Wuxi -> Suzhou -> Shanghai
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 9, 0, 0);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 8, 30, 30);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 7, 60, 60);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 6, 90, 90);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 1, 120, 120);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 3, 150, 150);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 4, 180, 180);
insert into schedule (train_id, station_id, cost_time, cost_money) values (2, 5, 210, 210);

insert into seat (name, train_id) values ('G1', 1);
insert into seat (name, train_id) values ('G2', 1);
insert into seat (name, train_id) values ('G3', 1);
insert into seat (name, train_id) values ('G4', 1);
insert into seat (name, train_id) values ('G5', 1);
insert into seat (name, train_id) values ('G6', 1);
insert into seat (name, train_id) values ('G7', 1);
insert into seat (name, train_id) values ('G8', 1);
insert into seat (name, train_id) values ('G9', 1);
insert into seat (name, train_id) values ('G10', 1);

insert into seat (name, train_id) values ('T1', 2);
insert into seat (name, train_id) values ('T2', 2);
insert into seat (name, train_id) values ('T3', 2);
insert into seat (name, train_id) values ('T4', 2);
insert into seat (name, train_id) values ('T5', 2);
insert into seat (name, train_id) values ('T6', 2);
insert into seat (name, train_id) values ('T7', 2);
insert into seat (name, train_id) values ('T8', 2);
insert into seat (name, train_id) values ('T9', 2);
insert into seat (name, train_id) values ('T10', 2);

