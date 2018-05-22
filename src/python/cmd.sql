SQL

Requetes pour avoir les infos de tous les AP detecté par 1 Rasberry en fonction de son IP.

SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.49;
SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.50;
SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.51;

Requetes pour avoir les infos dites essentielles des AP detecté par 1 Rasberry en fonction de son IP.

SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.49;
SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.50;
SELECT Info_AP.ESSID, Info_AP.Mac, Encryption.Encryption_name, Info_AP.Signal, Info_AP.Time, Info_AP.Beacon FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.51;

Requetes pour avoir des infos sur les AP detectés par 3 Rasberrys.(WIP)

SELECT Info_AP.ESSID, Info_AP.Mac, Info_AP.Time, Info_AP.Signal FROM Info_AP, Device_info, Encryption WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Info_AP.Id_encryption = Encryption.Id_encryption AND Device_info.IP = 172.16.220.49 AND Device_info.IP = 172.16.220.50 AND Device_info.IP = 172.16.220.51;

Requetes pour avoir les infos de 1 Rasberry en fonction de son IP.

SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.49;
SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.50;
SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.51;

Requetes pour avoir les diffentes types d encryption des AP detectés.

SELECT * FROM Encryption;

