#!/usr/bin/env python3
# -*- coding: utf-8 -*-

## Script Python Requetes SQl 


import mysql.connector
import sys
import os

# Connection dans la base
# Requetes
# Retour dans fichier txt.


# Connection dans la base ragnarok_bdd en localhost/User:root
conn = mysql.connector.connect(host="localhost", user="root", password="root", database="ragnarok_bdd")

# Requetes pour avoir les infos de tous les AP detecté par 1 Rasberry en fonction de son IP.

def RequetesAP(db_conn):
	curs = db_conn.cursor()

	sql0 = "SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.49;"
	sql1 = "SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.50;"
	sql2 = "SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.51;"
	
	curs.execute(sql0, sql1, sql2)

# Requetes pour avoir les infos dites essentielles des AP detecté par 1 Rasberry en fonction de son IP.

def RequetesAPess(db_conn):
	curs = db_conn.cursor()
	
	sql0 = "SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.49;"
	sql1 = "SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.50;"
	sql2 = "SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.51;"

	curs.execute(sql0, sql1, sql2)

# Requetes pour avoir des infos sur les AP detectés par 3 Rasberrys.(WIP)

def RequetesAP3Rasb(db_conn):
	curs = db_conn.cursor()

	sql0 = "SELECT Info_AP.ESSID, Info_AP.Mac, Info_AP.Time, Info_AP.Signal FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.49 AND Device_info.IP = 172.16.220.50 AND Device_info.IP = 172.16.220.51;"

	curs.execute(sql0)
# Requetes pour avoir les infos de 1 Rasberry en fonction de son IP.

def RequetesRasb(db_conn):
	curs = db_conn.cursor()

		sql0 = "SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.49;"
		sql1 = "SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.50;"
		sql2 = "SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.51;"


		curs.execute(sql0, sql1, sql2)

# Requetes pour avoir les diffentes types d encryption des AP detectés.

def RequetesEncryption(db_conn):
	curs = db_conn.cursor()

	sql0 = "SELECT * FROM Encryption;"

	curs.execute(sql0)


if __name__ == '__main__':
	
	RequetesAP(conn)
	conn.commit()
	RequetesEncryption(conn)
	conn.commit()
	RequetesRasb(conn)
	conn.commit()
	RequetesAPess(conn)
	conn.commit()
	RequetesAP3Rasb(conn)
	conn.commit()

