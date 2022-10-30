create database jreport;
use jreport;
CREATE TABLE IF NOT EXISTS Issue (id VARCHAR(256) NOT NULL,key_field VARCHAR(256) NOT NULL,name VARCHAR(256) NOT NULL,description VARCHAR(4096) NOT NULL,author VARCHAR(256) NOT NULL,assignee VARCHAR(256) NOT NULL,status VARCHAR(256) NOT NULL,PRIMARY KEY (id),KEY(key_field));
CREATE TABLE IF NOT EXISTS Initiatives_Issue(initative_name VARCHAR(256) NOT NULL,issue_key VARCHAR(256) NOT NULL);
CREATE TABLE IF NOT EXISTS Product_Issue(product_name VARCHAR(256) NOT NULL,initative_name VARCHAR(256) NOT NULL,issue_key VARCHAR(256) NOT NULL);

INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1395');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1396');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1397');

INSERT INTO Product_Issue(product_name,initative_name,issue_key) VALUES ('МТС Твой Бизнес','Cloud Native','CLMRKT-83');


