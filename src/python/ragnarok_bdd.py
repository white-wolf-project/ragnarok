#!/usr/bin/env python3
##
# @file ragnarok_bdd.py
# @brief Create database for ragnarok
#

import mysql.connector
from mysql.connector import errorcode

DB_NAME = "ragnarok_bdd"

TABLES = {}

TABLES['Info_AP'] = (
	"CREATE TABLE `Info_AP` ("
	"  `Mac` varchar(25) NOT NULL,"
	"  `ESSID` varchar(50) NULL,"
	"  `Time` varchar(25)  NOT NULL,"
	"  `Id_encryption` int(25) NOT NULL ,"
	"  `Channel` varchar(25) NOT NULL,"
	"  `Beacon` varchar(25) NOT NULL,"
	"  `Signal` varchar(25) NOT NULL,"
	"  `Frequency` varchar(25) NOT NULL,"
   	"  `Id_quality` varchar(25) NOT NULL,"
	"  `MAC_Rasb` varchar(25) NOT NULL,"
	"  PRIMARY KEY (`Mac`)"
	") ENGINE=InnoDB")

TABLES['Encryption'] = (
	"CREATE TABLE `Encryption` ("
	"  `Id_encryption` int NOT NULL AUTO_INCREMENT,"
	"  `Encryption_name` varchar(50) NOT NULL,"
	"  PRIMARY KEY (`Id_encryption`)"
	") ENGINE=InnoDB")

TABLES['Device_info'] = (
	"CREATE TABLE `Device_info` ("
	"  `Time` varchar(25) NOT NULL,"
	"  `Mac` varchar(17) NOT NULL,"
	"  `IP` varchar(15) NOT NULL,"
	"  PRIMARY KEY (`Mac`)"
	") ENGINE=InnoDB")

TABLES['Quality'] = (
	"CREATE TABLE `Quality` ("
	"  `Id_quality` int NOT NULL AUTO_INCREMENT,"
	"  `Qual_Rpi1` int NOT NULL,"
	"  `Qual_Rpi2` int NOT NULL,"
	"  `Qual_Rpi3` int NOT NULL,"
	"  PRIMARY KEY (`Id_quality`)"
	") ENGINE=InnoDB")

TABLES['Blank'] = (
	"CREATE TABLE `Blank` ("
	"  `X1` varchar(100) NOT NULL,"
	"  `X2` varchar(100) NOT NULL,"
	"  `X3` varchar(100) NOT NULL"
 	") ENGINE=InnoDB")

## Function : create_database(cursor)
#
# Function used to create database
# @param cursor
def create_database(cursor):
	try:
		cursor.execute("CREATE DATABASE {} DEFAULT CHARACTER SET 'utf8'".format(DB_NAME))
	except mysql.connector.Error as err:
		print("Failed creating database: {}".format(err))
		exit(1)

## Function : create_tables(cursor)
#
# Function used to create tables for the database
# @param cursor
def create_tables(cursor):
	cnx = cnx = mysql.connector.connect(user='root', password='root', host='localhost')
	for name, ddl in TABLES.items():
		try:
			print("Creating table {}: ".format(name))
			cursor.execute(ddl)
		except mysql.connector.Error as err:
			if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
				print("already exists.")
			else:
				print(err.msg)
		else:
			print("OK")

	cursor.close()
	cnx.close()

## Function : init_bdd()
#
# Function used init ragnarok database, to prepare for setting data in tables
# @param cursor
def init_bdd():
	cnx = mysql.connector.connect(user='root', password='root', host='localhost')
	cursor = cnx.cursor()

	create_database(cursor)

	try:
		cnx.database = DB_NAME  
	except mysql.connector.Error as err:
		if err.errno == errorcode.ER_BAD_DB_ERROR:
			create_database(cursor)
			cnx.database = DB_NAME
		else:
			print(err)
			exit(1)
	try:
		cnx = mysql.connector.connect(user='root', password='root', host='localhost', database='ragnarok')

	except mysql.connector.Error as err:
		if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
			print("Incorrect username or password")
		else:
			print(err)

	create_tables(cursor)
	cnx.close()

if __name__ == '__main__':
	init_bdd()
