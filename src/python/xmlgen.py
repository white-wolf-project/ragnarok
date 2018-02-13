#!/usr/bin/env python
#-*- coding: utf-8 -*-

#XML generator

from lxml import etree

ragnarok = etree.Element("ragnarok")

appareil = etree.SubElement(ragnarok, "appareil")
appareil.set("id", "")
ESSID = etree.SubElement(appareil, "ESSID")
ESSID.text = ""
MAC = etree.SubElement(appareil, "MAC")
MAC.text = ""
timestamp = etree.SubElement(appareil, "timestamp")
timestamp.text = ""

info_AP = etree.SubElement(ragnarok, "info_AP")
MAC = etree.SubElement(info_AP, "MAC")
MAC.text = ""
id_encryption = etree.SubElement(info_AP, "id_encryption")
id_encryption.text = ""
channel = etree.SubElement(info_AP, "channel")
channel.text = ""
beacon = etree.SubElement(info_AP, "beacon")
beacon.text = ""
id_quality = etree.SubElement(info_AP, "id_quality")
id_quality.text = ""

encryption = etree.SubElement(ragnarok, "encryption")
encryption_name = etree.SubElement(encryption, "encryption_name")
encryption_name.text = ""
id_encryption = etree.SubElement(encryption, "id_encryption")
id_encryption.text = ""

timeStat = etree.SubElement(ragnarok, "time")
time = etree.SubElement(timeStat,"time")
time.text = ""
timestamp = etree.SubElement(timeStat, "timestamp")
timestamp.text = ""

quality = etree.SubElement(ragnarok, "quality")
id_quality = etree.SubElement(quality, "id_quality")
id_quality.text = ""
Qual_rpi0 = etree.SubElement(quality,"Qual_rpi0")
Qual_rpi0.text = ""
Qual_rpi1 = etree.SubElement(quality,"Qual_rpi1")
Qual_rpi1.text = ""
Qual_rpi2 = etree.SubElement(quality,"Qual_rpi2")
Qual_rpi2.text = ""


print(etree.tostring(ragnarok, pretty_print=True))
#with open("log.xml", "r") as fichier:
#	fichier.read