

#!/usr/bin/env python
# -*- coding: utf-8 -*-

import mysql.connector
import xml.etree.ElementTree 
import os

path = os.getcwd
os.chdir("/home/eehp/Desktop")

conn = mysql.connector.connect(host="localhost",user="root",password="root", database="ragnarok_bdd")


def insert_info_ap(db_conn, mac, essid, time, id_encryption, channel, beacon, signal, frequency, id_quality, MAC_Rasb): 
	curs = db_conn.cursor() 
	curs.execute("INSERT INTO Info_AP (Mac, ESSID, Time, Id_encryption, Channel, Beacon, Signal, Frequency, Id_quality, MAC_Rasb) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s", 
				(mac, essid, time, id_encryption, channel, beacon, signal, frequency, id_quality, MAC_Rasb)) 
	

def insert_encryption(db_conn, Id_encryption, Encryption_name):
	curs = db_conn.cursor() 
	curs.execute("""INSERT INTO Encryption (Id_encryption, Encryption_name) values (?, ?)""",
				(Id_encryption, Encryption_name))


def insert_device_info(db_conn, time, mac, ip):
	curs = db_conn.cursor() 
	curs.execute("""INSERT INTO  Devide_info (Time, Mac, Ip) values (?, ?, ?)""",
				(time, mac, ip))


def insert_quality(db_conn, Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3):
	curs = db_conn.cursor() 
	curs.execute("""INSERT INTO  Quality (Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3) values (?, ?, ?, ?)""",
				(Id_quality, Qual_Rpi1, Qual_Rpi2, Qual_Rpi3))
	

# def person_data_from_element(element): 
#     first = element.find("firstName").text 
#     last = element.find("lastName").text 
#     profession = element.find("profession").text 
#     return first, last, profession 

# def xmlparser():
	
# 	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
# 	APs = ragnarok.findall("info_AP")
# 	for info_AP in APs:

def ap_data_from_element(info_AP):
	mac = info_AP.find("mac").text
	channel = info_AP.find("channel").text
	frequency = info_AP.find("frequency").text
	quality = info_AP.find("quality").text
	signal = info_AP.find("signal").text
	essid = info_AP.find("essid").text
	beacon = info_AP.find("last_beacon").text
	encryption = info_AP.find("encryption").text
	return mac, channel, frequency, quality, signal, essid, beacon, encryption


if __name__ == '__main__':
	conn = mysql.connector.connect(host="localhost",user="root",password="root", database="ragnarok_bdd")
	
	ragnarok = xml.etree.ElementTree.parse("ragnarok.xml")
	APs = ragnarok.findall("info_AP")
	for element in APs:
		mac, channel, frequency, quality, signal, essid, beacon, encryption = ap_data_from_element(element)
		insert_info_ap(conn, mac, essid, "10:00", 1, channel, beacon, signal, frequency, 1, "AA:AA:AA:AA:AA:AA")


