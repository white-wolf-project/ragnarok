#!/usr/bin/env python3
# -*- coding: utf-8 -*-
##
# @file xmlparser.py
# @brief Parse XML file to put info in the ragnarok database
#

import mysql.connector
import xml.etree.ElementTree 
import sys

ap_element_data = ["mac", "essid", "channel", "last_beacon", "quality", "signal", "frequency", "encryption"]
device_element_data = ["time", "rpi_mac", "ip"]

## Function : insert_info_ap
# Function used to insert Wifi access point information in the database
# @param db_conn
# @param ap_data
def insert_info_ap(db_conn, ap_data):
	# TODO : find a simpler way
	mac = ap_data[0]
	essid = ap_data[1]
	channel = ap_data[2]
	beacon = ap_data[3]
	quality = ap_data[4]
	signal = ap_data[5]
	frequency = ap_data[6]
	encryption = ap_data[7]
	time = ap_data[8]
	MAC_Rasb = ap_data[9]

	curs = db_conn.cursor() 
	sql = "INSERT INTO Info_AP VALUES (NULL,'%s', '%s', '%s', (SELECT Id_encryption FROM Encryption WHERE Encryption.Encryption_name = '%s'), \
	'%s', '%s', '%s', '%s', '%s', '%s');" % \
	(mac, essid, time, encryption, channel, beacon, signal, frequency, quality, MAC_Rasb)
	curs.execute(sql)

## Function : insert_device_info
# Function used to insert client information in the database
# @param db_conn
# @param time
# @param mac
# @param ip
def insert_device_info(db_conn, time, mac, ip):
	curs = db_conn.cursor()
	sql = "INSERT INTO Device_info VALUES ('%s', '%s', '%s');" % (time, mac, ip)
	curs.execute(sql)

## Function : insert_encryption
# Function used to insert encryption information in the database
# @param db_conn
# @param Encryption_name
def insert_encryption(db_conn, Encryption_name):
	curs = db_conn.cursor()
	sql1 = "INSERT INTO Encryption VALUES (NULL,'%s');" % (Encryption_name)
	sql2 = "SELECT COUNT(Encryption_name) FROM Encryption WHERE Encryption_name='%s';" % (Encryption_name)
	curs.execute(sql2)
	rows = curs.fetchone()
	line_nb = rows[0]
	if line_nb == 0:
		curs.execute(sql1)
	else:
		pass

## Function : ap_data_from_element
# Parse data in the XML file
# @param info_AP
# @param info_rasb
def ap_data_from_element(info_AP, info_rasb):
	element_data = list()
	data = list()
	for i in range(0, len(ap_element_data)):
		data.append(info_AP.find(ap_element_data[i]))
		if data[i] == None:
			element_data.append("None")
		else :
			element_data.append(data[i].text)

	data.clear()
	for i in range(0, len(device_element_data)):
		data.append(info_rasb.find(device_element_data[i]))
		if data[i] == None:
			element_data.append("None")
		else :
			element_data.append(data[i].text)

	return element_data

def parse_xml(xmlfile):
	ragnarok = xml.etree.ElementTree.parse(xmlfile)
	APs = ragnarok.findall("info_AP")
	RASBs = ragnarok.find("rpi_info")

	return APs, RASBs

## Function : usage
# usage function
def usage():
	print("usage : %s <file>" % sys.argv[0])

if __name__ == '__main__':
	if len(sys.argv) != 4 :
		usage()
		sys.exit(1)
	else :
		xmlfile1 = sys.argv[1]
		xmlfile2 = sys.argv[2]
		xmlfile3 = sys.argv[3]

	ap_data = list()

	# open connection to our database
	print("[i] opening connection to ragnarok_bdd")
	conn = mysql.connector.connect(host="localhost", user="root", password="root", database="ragnarok_bdd")

	# for all xml files, we parse them then grab all the info we need
	for i in range(1, len(sys.argv)):
		APs, RASBs = parse_xml(sys.argv[i])

		for AP in APs:
			ap_data = ap_data_from_element(AP, RASBs)

			# get encryption value then commit it to DB
			for i in range(0, len(ap_element_data)) :
				if ap_element_data[i] == "encryption":
					encryption = ap_data[i]

			print("[+] inserting encryption data")
			insert_encryption(conn, encryption)
			conn.commit()

			print("[+] inserting Wifi Access Point data")
			insert_info_ap(conn, ap_data)
			conn.commit()

		# in this loop we grab time, MAC addr of RPi and IP addr to put it in the Device_Info table
		for i in range(0, len(device_element_data)) :
			if device_element_data[i] == "time":
				time = ap_data[i + len(ap_element_data)]

			elif device_element_data[i] == "rpi_mac":
				mac_rasb = ap_data[i + len(ap_element_data)]

			elif device_element_data[i] == "ip":
				addr_ip = ap_data[i + len(ap_element_data)]

			else :
				pass

		print("[+] inserting Raspberry Pi info data")
		insert_device_info(conn, time, mac_rasb, 0)
		conn.commit()

	print("[i] closing connection to ragnarok_bdd")
	conn.close()

	print("done")
