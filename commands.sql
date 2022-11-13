create database jreport;
use jreport;
CREATE TABLE IF NOT EXISTS Issue (id VARCHAR(256) NOT NULL,key_field VARCHAR(256) NOT NULL,name VARCHAR(256) NOT NULL,description VARCHAR(4096) NOT NULL,author VARCHAR(256) NOT NULL,assignee VARCHAR(256) NOT NULL,status VARCHAR(256) NOT NULL,PRIMARY KEY (id),KEY(key_field));
CREATE TABLE IF NOT EXISTS Initiatives_Issue(initative_name VARCHAR(256) NOT NULL,issue_key VARCHAR(256) NOT NULL);
CREATE TABLE IF NOT EXISTS Cluster_Initiative_Issue(issue varchar(256) NOT NULL,initiative_issue VARCHAR(256) NOT NULL,initiative VARCHAR(256) NOT NULL,cluster VARCHAR(256) NOT NULL);
CREATE TABLE IF NOT EXISTS Product_Initiative_Issue(product varchar(256) NOT NULL,cluster_issue VARCHAR(256) NOT NULL,product_issue VARCHAR(256) NOT NULL);

INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1395');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1396');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Cloud Native','KA-1397');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('ArchOPS','KA-1435');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('TechRadar','KA-1479');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1140');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1141');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Target Architecture Guidelines','KA-1142');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Near Realtime','KA-1480');
INSERT INTO Initiatives_Issue(initative_name,issue_key) VALUES ('Near Realtime','KA-1504');
