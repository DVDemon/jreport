create database jreport;
use jreport;
CREATE TABLE IF NOT EXISTS Issue (id VARCHAR(256) NOT NULL,key_field VARCHAR(256) NOT NULL,name VARCHAR(256) NOT NULL,description VARCHAR(4096) NOT NULL,author VARCHAR(256) NOT NULL,assignee VARCHAR(256) NOT NULL,status VARCHAR(256) NOT NULL,PRIMARY KEY (id),KEY(key_field));
