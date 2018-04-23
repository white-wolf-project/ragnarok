SQL

Requetes pour avoir les infos de tous les AP detecté par 1 Rasberry en fonction de son IP.

SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.49; 
SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.50;  
SELECT * FROM Info_AP, Device_info WHERE Info_AP.MAC_Rasb = Device_info.Mac AND Device_info.IP = 172.16.220.51;  

Requetes pour avoir les infos de 1 Rasberry.

SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.49;
SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.50;
SELECT * FROM Device_info WHERE Device_info.IP = 172.16.220.51;

Requetes pour avoir les diffentes types d encryption des AP detectés.

SELECT * FROM Encryption;


