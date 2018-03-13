# DB.py

import mysql.connector
from mysql.connector import errorcode
import sys


DB_NAME = 'ragnarok_bdd'

TABLES = {}

TABLES['Appareil'] = (
    "CREATE TABLE `Appareil` ("
    "  `ESSID` varchar(25) NOT NULL,"
    "  `MAC` varchar(25) NOT NULL,"
    "  `Timestamp` int NOT NULL,"
    "  `Id` int NOT NULL,AUTO_INCREMENT,"
    "  PRIMARY KEY (`Id`)"
    ") ENGINE=InnoDB")

TABLES['Event'] = (
    "CREATE TABLE `Appareil` ("
    "  `Timestamp` int NOT NULL,"
    "  `Id` int NOT NULL,"
    ") ENGINE=InnoDB")

TABLES['Info_AP'] = (
    "CREATE TABLE `Info_AP` ("
    "  `MAC` int NOT NULL,"
    "  `Id_encryption` int NOT NULL,AUTO_INCREMENT,"
    "  `Channel` varchar(25) NOT NULL,"
    "  `Beacon` int NOT NULL,"
   	"  `Id_quality` int NOT NULL,"
    "  PRIMARY KEY (`MAC`)"
    ") ENGINE=InnoDB")

TABLES['Encryption'] = (
    "CREATE TABLE `Encryption` ("
    "  `Id_encryption` int NOT NULL,AUTO_INCREMENT,"
    "  `Encryption_name` varchar(50) NOT NULL,"
    "  PRIMARY KEY (`Id_encryption`)"
    ") ENGINE=InnoDB")

TABLES['Time'] = (
    "CREATE TABLE `Time` ("
    "  `Timestamp` int NOT NULL,"
    "  `Time` varchar(25) NOT NULL,"
    "  PRIMARY KEY (`Timestamp`)"
    ") ENGINE=InnoDB")

TABLES['Quality'] = (
	"CREATE TABLE `Quality` ("
	"  `Id_quality` int NOT NULL,AUTO_INCREMENT,"
	"  `Qual_Rpi1` int NOT NULL,"
	"  `Qual_Rpi2` int NOT NULL,"
	"  `Qual_Rpi3` int NOT NULL,"
	"  PRIMARY KEY (`Id_quality`)"
	") ENGINE=InnoDB")


def create_database(cursor):
    try:
        cursor.execute(
            "CREATE DATABASE {} DEFAULT CHARACTER SET 'utf8'".format(DB_NAME))
    except mysql.connector.Error as err:
        print("Failed creating database: {}".format(err))
        exit(1)

def create_tables(cursor):

	for name, ddl in TABLES.iteritems():
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

def init_bdd():
	
	cnx = mysql.connector.connect(user='root', password='root',
	                              host='127.0.0.1')
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
		cnx = mysql.connector.connect(user='root', password='root',
	                              host='127.0.0.1',
	                              database='ragnarok')

	except mysql.connector.Error as err:

	  	if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
			print("Something is wrong with your user name or password")

	  	else:
			print(err)

	create_tables(cursor)
	cnx.close()

if __name__ == '__main__':

	init_bdd()

