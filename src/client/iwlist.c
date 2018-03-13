/*
 *	Wireless Tools
 *
 *		Jean II - HPLB '99 - HPL 99->07
 *
 * This tool can access various piece of information on the card
 * not part of iwconfig...
 * You need to link this code against "iwlist.c" and "-lm".
 *
 * This file is released under the GPL license.
 *     Copyright (c) 1997-2007 Jean Tourrilhes <jt@hpl.hp.com>
 */

#include <sys/time.h>
#include <iwlib.h>

#include <include/client_tool.h>
#include <include/client.h>
#include <include/common.h>

/****************************** TYPES ******************************/

/*
 * Scan state and meta-information, used to decode events...
 */
typedef struct iwscan_state
{
	/* State */
	int ap_num;		/* Access Point number 1->N */
	int val_index;	/* Value in table 0->(N-1) */
} iwscan_state;

/*
 * Bit to name mapping
 */
typedef struct iwmask_name
{
	unsigned int mask;	/* bit mask for the value */
	const char *name;	/* human readable name for the value */
} iwmask_name;

/*
 * Types of authentication parameters
 */
typedef struct iw_auth_descr
{
	int	value;							/* Type of auth value */
	const char *label;					/* User readable version */
	const struct iwmask_name *names;	/* Names for this value */
	const int num_names;				/* Number of names */
} iw_auth_descr;

/**************************** CONSTANTS ****************************/

#define IW_SCAN_HACK 0x8000

#define IW_EXTKEY_SIZE (sizeof(struct iw_encode_ext) + IW_ENCODING_TOKEN_MAX)

/* ------------------------ WPA CAPA NAMES ------------------------ */
/*
 * This is the user readable name of a bunch of WPA constants in wireless.h
 * Maybe this should go in iwlib.c ?
 */

#ifndef WE_ESSENTIAL
#define IW_ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

#ifndef IW_IE_CIPHER_NONE
/* Cypher values in GENIE (pairwise and group) */
#define IW_IE_CIPHER_NONE	0
#define IW_IE_CIPHER_WEP40	1
#define IW_IE_CIPHER_TKIP	2
#define IW_IE_CIPHER_WRAP	3
#define IW_IE_CIPHER_CCMP	4
#define IW_IE_CIPHER_WEP104	5
/* Key management in GENIE */
#define IW_IE_KEY_MGMT_NONE	0
#define IW_IE_KEY_MGMT_802_1X	1
#define IW_IE_KEY_MGMT_PSK	2
#endif	/* IW_IE_CIPHER_NONE */

#ifdef DEBUG_ADV
/* Values for the IW_IE_CIPHER_* in GENIE */
static const char *	iw_ie_cypher_name[] = {
	"none",
	"WEP-40",
	"TKIP",
	"WRAP",
	"CCMP",
	"WEP-104",
};
#define	IW_IE_CYPHER_NUM	IW_ARRAY_LEN(iw_ie_cypher_name)

/* Values for the IW_IE_KEY_MGMT_* in GENIE */
static const char *	iw_ie_key_mgmt_name[] = {
	"none",
	"802.1x",
	"PSK",
};
#endif
#define	IW_IE_KEY_MGMT_NUM	IW_ARRAY_LEN(iw_ie_key_mgmt_name)

#endif	/* WE_ESSENTIAL */

#ifndef WE_ESSENTIAL

#ifdef DEBUG_ADV
/*
 * Print the name corresponding to a value, with overflow check.
 */
static void
iw_print_value_name(unsigned int value, const char * names[], const unsigned int num_names)
{
	if(value >= num_names)
		send_data(sock, " unknown (%d)", value);
	else
		send_data(sock, " %s", names[value]);
}

#endif
/*
 * Parse, and display the results of a WPA or WPA2 IE.
 *
 */
static inline void 
iw_print_ie_wpa(unsigned char *	iebuf, int buflen)
{
	int	ielen = iebuf[1] + 2;
	int	offset = 2;	/* Skip the IE id, and the length. */
	unsigned char wpa1_oui[3] = {0x00, 0x50, 0xf2};
	unsigned char wpa2_oui[3] = {0x00, 0x0f, 0xac};
	unsigned char *	wpa_oui;
	uint16_t ver = 0;
	char wpa_stuff[128];
#ifdef DEBUG_ADV
	uint16_t cnt = 0;
	int	i;
#endif

	if(ielen > buflen)
		ielen = buflen;

#ifdef DEBUG_ADV
	/* Debugging code. In theory useless, because it's debugged ;-) */
	send_data(sock, "IE raw value %d [%02X", buflen, iebuf[0]);
	for(i = 1; i < buflen; i++)
		send_data(sock, ":%02X", iebuf[i]);
	send_data(sock, "]\n");
#endif

	switch(iebuf[0])
	{
		case 0x30:	/* WPA2 */
			/* Check if we have enough data */
			if(ielen < 4)
			{
				return;
			}

			wpa_oui = wpa2_oui;
			break;

		case 0xdd:		/* WPA or else */
			wpa_oui = wpa1_oui;

			/* Not all IEs that start with 0xdd are WPA.
			* So check that the OUI is valid. Note : offset==2 */
			if((ielen < 8) || (memcmp(&iebuf[offset], wpa_oui, 3) != 0) || (iebuf[offset + 3] != 0x01))
			{
				return;
			}

			/* Skip the OUI type */
			offset += 4;
			break;

			default:
			return;
	}

	/* Pick version number (little endian) */
	ver = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;

	if(iebuf[0] == 0xdd){
		sprintf(wpa_stuff, "WPA Version %d", ver);
		send_data(sock, "%s\n", wpa_stuff);
	}
	if(iebuf[0] == 0x30){
		sprintf(wpa_stuff, "IEEE 802.11i/WPA2 Version %d", ver);
		send_data(sock, "%s\n", wpa_stuff);
	}

	info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "encryption", NULL);
	xmlNodeAddContent(info_ap_child, BAD_CAST wpa_stuff);
	xmlNewProp(info_ap_child, BAD_CAST "on", BAD_CAST "yes");
	/* From here, everything is technically optional. */
	#ifdef DEBUG_ADV
	/* Check if we are done */
	if(ielen < (offset + 4))
	{
		/* We have a short IE.  So we should assume TKIP/TKIP. */
		send_data(sock, "Group Cipher : TKIP\n");
		send_data(sock, "Pairwise Cipher : TKIP\n");
		return;
	}

	/* Next we have our group cipher. */
	if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
	{
		send_data(sock, "Group Cipher : Proprietary\n");
	}
	else
	{
		send_data(sock, "Group Cipher :");
		iw_print_value_name(iebuf[offset+3], iw_ie_cypher_name, IW_IE_CYPHER_NUM);
		send_data(sock, "\n");
	}
	offset += 4;

	/* Check if we are done */
	if(ielen < (offset + 2))
	{
		/* We don't have a pairwise cipher, or auth method. Assume TKIP. */
		send_data(sock, "Pairwise Ciphers : TKIP\n");
		return;
	}

	/* Otherwise, we have some number of pairwise ciphers. */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;
	send_data(sock, "Pairwise Ciphers (%d) :", cnt);

	if(ielen < (offset + 4*cnt))
		return;

	for(i = 0; i < cnt; i++)
	{
		if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
		{
			send_data(sock, " Proprietary");
		}
		else
		{
			iw_print_value_name(iebuf[offset+3], iw_ie_cypher_name, IW_IE_CYPHER_NUM);
		}
		offset+=4;
	}
	send_data(sock, "\n");

	/* Check if we are done */
	if(ielen < (offset + 2))
		return;

	/* Now, we have authentication suites. */
	cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
	offset += 2;
	send_data(sock, "Authentication Suites (%d) :", cnt);

	if(ielen < (offset + 4*cnt))
		return;

	for(i = 0; i < cnt; i++)
	{
		if(memcmp(&iebuf[offset], wpa_oui, 3) != 0)
		{
			send_data(sock, " Proprietary");
		}
		else
		{
			iw_print_value_name(iebuf[offset+3], iw_ie_key_mgmt_name, IW_IE_KEY_MGMT_NUM);
		}
		offset+=4;
	}
	send_data(sock, "\n");

	/* Check if we are done */
	if(ielen < (offset + 1))
		return;

	/* Otherwise, we have capabilities bytes.
	 * For now, we only care about preauth which is in bit position 1 of the
	 * first byte.  (But, preauth with WPA version 1 isn't supposed to be
	 * allowed.) 8-)
	 */
	if(iebuf[offset] & 0x01)
	{
		send_data(sock, "Preauthentication Supported\n");
	}
	#endif /* DEBUG_ADV */
}

/*
 * Process a generic IE and display the info in human readable form
 * for some of the most interesting ones.
 * For now, we only decode the WPA IEs.
 */
static inline void
iw_print_gen_ie(unsigned char *	buffer,
	int		buflen)
{
	int offset = 0;

	/* Loop on each IE, each IE is minimum 2 bytes */
	while(offset <= (buflen - 2))
	{
	/* Check IE type */
		switch(buffer[offset])
		{
			case 0xdd:	/* WPA1 (and other) */
			case 0x30:	/* WPA2 */
				iw_print_ie_wpa(buffer + offset, buflen);
				break;
			default:
				debug("");
		}
		/* Skip over this IE to the next one in the list. */
		offset += buffer[offset+1] + 2;
	}
}
#endif	/* WE_ESSENTIAL */

/***************************** SCANNING *****************************/
/*
 * This one behave quite differently from the others
 *
 * Note that we don't use the scanning capability of iwlib (functions
 * iw_process_scan() and iw_scan()). The main reason is that
 * iw_process_scan() return only a subset of the scan data to the caller,
 * for example custom elements and bitrates are ommited. Here, we
 * do the complete job...
 */

/*
 * Print one element from the scanning results
 */

static inline void
print_scanning_token(struct stream_descr *stream, struct iw_event *event, struct iwscan_state *state, struct iw_range *iw_range, int has_range)
{
	char info_ap_id[12];
	char buffer[128];
	char *channel_out, *frequency_out, *quality_out, *signal_out, *lastbeacon_out;
	/* Now, let's decode the event */
	switch(event->cmd)
	{
		case SIOCGIWAP:
			send_data(sock, "===========================");
			send_data(sock, "========================================================================\n");
			char *get_time = get_date_and_time();
			debug("%s\n", get_time);

			send_data(sock, "AP  %02d\n", state->ap_num);
			info_ap = xmlNewChild(root, NULL, BAD_CAST "info_AP", NULL);

			send_data(sock, "MAC : %s\n", iw_saether_ntop(&event->u.ap_addr, buffer));
			info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "mac", NULL);
			xmlNodeAddContent(info_ap_child, BAD_CAST iw_saether_ntop(&event->u.ap_addr, buffer));

			state->ap_num++;
			break;
		case SIOCGIWNWID:
			if(event->u.nwid.disabled)
				send_data(sock, "NWID: off/any\n");
			else
				send_data(sock, "NWID: %X\n", event->u.nwid.value);
			break;
		case SIOCGIWFREQ:
		{
			double freq;			/* Frequency/channel */
			int	channel = -1;		/* Converted to channel */
			freq = iw_freq2float(&(event->u.freq));
			/* Convert to channel if possible */
			if(has_range)
				channel = iw_freq_to_channel(freq, iw_range);
			iw_print_freq(buffer, sizeof(buffer), freq, channel, event->u.freq.flags);

			/* Assume it is channel */
			if (buffer[0] == 'C')
			{
				/* crappy code*/
				char new_buf[12];
				sprintf(new_buf, "%sa", buffer);
				channel_out = get_txt(new_buf, "Channel:", "a");
				send_data(sock, "channel : %s\n", channel_out);
				/*end of crappy code */

				info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "channel", NULL);
				xmlNodeAddContent(info_ap_child, BAD_CAST channel_out);

			} /* Assume it is Frequency*/
			else if (buffer[0] == 'F'){
				frequency_out = get_txt(buffer, ":", " ");
				send_data(sock, "frequency : %s\n", frequency_out);
				info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "frequency", NULL);
				xmlNodeAddContent(info_ap_child, BAD_CAST frequency_out);
			}
			else {
				break;
			}
		}
			break;
		case SIOCGIWMODE:
			/* Note : event->u.mode is unsigned, no need to check <= 0 */
			if(event->u.mode >= IW_NUM_OPER_MODE)
				event->u.mode = IW_NUM_OPER_MODE;
			#ifdef DEBUG_ADV
			debug("Mode: %s\n", iw_operation_mode[event->u.mode]);
			#endif
			break;
		case SIOCGIWNAME:
			send_data(sock, "Protocol: %-1.16s\n", event->u.name);
			break;
		case SIOCGIWESSID:
		{
			char essid[IW_ESSID_MAX_SIZE+1];
			memset(essid, '\0', sizeof(essid));
			if((event->u.essid.pointer) && (event->u.essid.length))
				memcpy(essid, event->u.essid.pointer, event->u.essid.length);
			if(event->u.essid.flags)
			{
				/* Does it have an ESSID index ? */
				if((event->u.essid.flags & IW_ENCODE_INDEX) > 1)
					send_data(sock, "ESSID: \"%s\" [%d]\n", essid, (event->u.essid.flags & IW_ENCODE_INDEX));
				else
					send_data(sock, "ESSID: \"%s\"\n", essid);
			}
			else
				send_data(sock, "ESSID: off/any/hidden\n");
			info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "essid", NULL);
			xmlNodeAddContent(info_ap_child, BAD_CAST essid);
		}
		break;
		case SIOCGIWENCODE:
		{
			unsigned char key[IW_ENCODING_TOKEN_MAX];
			if(event->u.data.pointer)
				memcpy(key, event->u.data.pointer, event->u.data.length);
			else
				event->u.data.flags |= IW_ENCODE_NOKEY;
			send_data(sock, "Encryption key: ");
			if(event->u.data.flags & IW_ENCODE_DISABLED) {
				send_data(sock, "off\n");
				info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "encryption", NULL);
				xmlNodeAddContent(info_ap_child, BAD_CAST "");
				xmlNewProp(info_ap_child, BAD_CAST "on", BAD_CAST "no");
			}
			else
			{
				/* Display the key */
				iw_print_key(buffer, sizeof(buffer), key, event->u.data.length, event->u.data.flags);
				send_data(sock, " %s", buffer); // should output "on"
				/* Other info... */
				if((event->u.data.flags & IW_ENCODE_INDEX) > 1)
					send_data(sock, " [%d]", event->u.data.flags & IW_ENCODE_INDEX);
				if(event->u.data.flags & IW_ENCODE_RESTRICTED)
					send_data(sock, "Security mode: restricted");
				if(event->u.data.flags & IW_ENCODE_OPEN)
					send_data(sock, "Security mode: open");
				send_data(sock, "\n");
			}
		}
		break;
		case SIOCGIWMODUL:
		{
			unsigned int modul = event->u.param.value;
			int i;
			int n = 0;
			send_data(sock, "Modulations : ");
			for(i = 0; i < IW_SIZE_MODUL_LIST; i++)
			{
				if((modul & iw_modul_list[i].mask) == iw_modul_list[i].mask)
				{
					if((n++ % 8) == 7)
						send_data(sock, "\n");
					else
						send_data(sock, " ; ");
					send_data(sock, "%s", iw_modul_list[i].cmd);
				}
			}
			//send_data(sock, "\n");
		}
			break;
		case IWEVQUAL:
			iw_print_stats(buffer, sizeof(buffer), &event->u.qual, iw_range, has_range);

			quality_out = get_txt(buffer, "Quality=", "  S");
			signal_out = get_txt(buffer, "level=", " dBm");

			send_data(sock, "Quality : %s\n", quality_out);
			info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "quality", NULL);
			xmlNodeAddContent(info_ap_child, BAD_CAST quality_out);

			send_data(sock, "Signal : %s dBm\n", signal_out);
			info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "signal", NULL);
			xmlNodeAddContent(info_ap_child, BAD_CAST signal_out);

			break;
#ifndef WE_ESSENTIAL
		case IWEVGENIE:
			/* Informations Elements are complex, let's do only some of them */
			iw_print_gen_ie(event->u.data.pointer, event->u.data.length);
			break;
#endif	/* WE_ESSENTIAL */
		case IWEVCUSTOM:
		{
			char custom[IW_CUSTOM_MAX+1];
			if((event->u.data.pointer) && (event->u.data.length))
				memcpy(custom, event->u.data.pointer, event->u.data.length);
			custom[event->u.data.length] = '\0';

			/* don't print TSF stuff, useless for what we want */
			if (custom[0] != 't'){
				lastbeacon_out = get_txt(custom, ": ", "ms");
				send_data(sock, "last beacon: %s ms\n", lastbeacon_out);
				info_ap_child = xmlNewChild(info_ap, NULL, BAD_CAST "last_beacon", NULL);
				xmlNodeAddContent(info_ap_child, BAD_CAST lastbeacon_out);
			}
		}
		break;

		default:
			break;
	}
}

/*------------------------------------------------------------------*/
/*
 * Perform a scanning on one device
 */
static int print_scanning_info(int skfd, char *	ifname, char *	args[], int	count)
{
	struct iwreq wrq;
	struct iw_scan_req scanopt;
	int	scanflags = 0;
	unsigned char *	buffer = NULL;
	int	buflen = IW_SCAN_MAX_DATA;	/* Min for compat WE<17 */
	struct iw_range	range;
	int has_range;
	struct timeval tv;	/* Select timeout */
	int timeout = 15000000;	/* 15s */

	/* Avoid "Unused parameter" warning */
	args = args; count = count;

	/* Debugging stuff */
	if((IW_EV_LCP_PK2_LEN != IW_EV_LCP_PK_LEN) || (IW_EV_POINT_PK2_LEN != IW_EV_POINT_PK_LEN))
	{
		send_data(sock, "*** Please report to jt@hpl.hp.com your platform details or open an issue on Ragnarok repo because code has been modified\n");
		send_data(sock, "*** and the following line :\n");
		send_data(sock, "*** IW_EV_LCP_PK2_LEN = %zu ; IW_EV_POINT_PK2_LEN = %zu\n\n", IW_EV_LCP_PK2_LEN, IW_EV_POINT_PK2_LEN);
	}

	/* Get range stuff */
	has_range = (iw_get_range_info(skfd, ifname, &range) >= 0);

	/* Check if the interface could support scanning. */
	if((!has_range) || (range.we_version_compiled < 14))
	{
		send_data(sock, "%-8.16s  Interface doesn't support scanning.\n\n", ifname);
		return(-1);
	}

	/* Init timeout value -> 250ms between set and first get */
	tv.tv_sec = 0;
	tv.tv_usec = 250000;

	/* Clean up set args */
	memset(&scanopt, 0, sizeof(scanopt));

	/* Parse command line arguments and extract options.
	 * Note : when we have enough options, we should use the parser
	 * from iwconfig... */
	while(count > 0) {
		/* One arg is consumed (the option name) */
		count--;

		/*
		* Check for Active Scan (scan with specific essid)
		*/
		if(!strncmp(args[0], "essid", 5))
		{
			if(count < 1)
			{
				send_data(sock, "Too few arguments for scanning option [%s]\n", args[0]);
				return(-1);
			}
			args++;
			count--;

			/* Store the ESSID in the scan options */
			scanopt.essid_len = strlen(args[0]);
			memcpy(scanopt.essid, args[0], scanopt.essid_len);
			/* Initialise BSSID as needed */
			if(scanopt.bssid.sa_family == 0)
			{
				scanopt.bssid.sa_family = ARPHRD_ETHER;
				memset(scanopt.bssid.sa_data, 0xff, ETH_ALEN);
			}
			/* Scan only this ESSID */
			scanflags |= IW_SCAN_THIS_ESSID;
		}
		else
		{
			/* Check for last scan result (do not trigger scan) */
			if(!strncmp(args[0], "last", 4))
			{
				/* Hack */
				scanflags |= IW_SCAN_HACK;
			}
			else
			{
				send_data(sock, "Invalid scanning option [%s]\n", args[0]);
				return(-1);
			}
			/* Next arg */
			args++;
		}
	}

	/* Check if we have scan options */
	if(scanflags)
	{
		wrq.u.data.pointer = (caddr_t) &scanopt;
		wrq.u.data.length = sizeof(scanopt);
		wrq.u.data.flags = scanflags;
	}
	else
	{
		wrq.u.data.pointer = NULL;
		wrq.u.data.flags = 0;
		wrq.u.data.length = 0;
	}

	/* If only 'last' was specified on command line, don't trigger a scan */
	if(scanflags == IW_SCAN_HACK) {
		/* Skip waiting */
		tv.tv_usec = 0;
	}
	else
	{
		/* Initiate Scanning */
		if(iw_set_ext(skfd, ifname, SIOCSIWSCAN, &wrq) < 0)
		{
			if((errno != EPERM) || (scanflags != 0))
			{
				send_data(sock, "%-8.16s  Interface doesn't support scanning : %s\n", ifname, strerror(errno));
				return(-1);
			}
			/* If we don't have the permission to initiate the scan, we may
			* still have permission to read left-over results.
			* But, don't wait !!! */
#if 0
			/* Not cool, it display for non wireless interfaces... */
			send_data(sock, "%-8.16s  (Could not trigger scanning, just reading left-over results)\n", ifname);
#endif
			tv.tv_usec = 0;
		}
	}
	timeout -= tv.tv_usec;

	/* Forever */
	while(1)
	{
		fd_set rfds;		/* File descriptors for select */
		int	last_fd;		/* Last fd */
		int	ret;

		/* Guess what ? We must re-generate rfds each time */
		FD_ZERO(&rfds);
		last_fd = -1;

		/* In here, add the rtnetlink fd in the list */

		/* Wait until something happens */
		ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

		/* Check if there was an error */
		if(ret < 0)
		{
			if(errno == EAGAIN || errno == EINTR)
				continue;
			send_data(sock, "Unhandled signal - exiting...\n");
			return(-1);
		}

		/* Check if there was a timeout */
		if(ret == 0)
		{
			unsigned char *	newbuf;

			realloc:
		/* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
			newbuf = realloc(buffer, buflen);
			if(newbuf == NULL)
			{
				if(buffer)
					free(buffer);
				send_data(sock, "%s: Allocation failed\n", __FUNCTION__);
				return(-1);
			}
			buffer = newbuf;

			/* Try to read the results */
			wrq.u.data.pointer = buffer;
			wrq.u.data.flags = 0;
			wrq.u.data.length = buflen;
			if(iw_get_ext(skfd, ifname, SIOCGIWSCAN, &wrq) < 0)
			{
				/* Check if buffer was too small (WE-17 only) */
				if((errno == E2BIG) && (range.we_version_compiled > 16))
				{
				/* Some driver may return very large scan results, either
				   * because there are many cells, or because they have many
				   * large elements in cells (like IWEVCUSTOM). Most will
				   * only need the regular sized buffer. We now use a dynamic
				   * allocation of the buffer to satisfy everybody. Of course,
				   * as we don't know in advance the size of the array, we try
				   * various increasing sizes. Jean II
				*/

					/* Check if the driver gave us any hints. */
					if(wrq.u.data.length > buflen)
						buflen = wrq.u.data.length;
					else
						buflen *= 2;

					/* Try again */
					goto realloc;
				}

					/* Check if results not available yet */
				if(errno == EAGAIN)
				{
					/* Restart timer for only 100ms*/
					tv.tv_sec = 0;
					tv.tv_usec = 100000;
					timeout -= tv.tv_usec;
					if(timeout > 0)
						continue;	/* Try again later */
				}

				/* Bad error */
				free(buffer);
				send_data(sock, "%-8.16s  Failed to read scan data : %s\n\n", ifname, strerror(errno));
				return(-2);
			}
			else {
				/* We have the results, go to process them */
				break;
			}
		}

		/* In here, check if event and event type
		*  if scan event, read results. All errors bad & no reset timeout */
	}

	if(wrq.u.data.length)
	{
		struct iw_event		iwe;
		struct stream_descr	stream;
		struct iwscan_state	state = { .ap_num = 1, .val_index = 0 };
		int			ret;

#ifdef DEBUG_ADV
		/* Debugging code. In theory useless, because it's debugged ;-) */
		int	i;
		send_data(sock, "Scan result %d [%02X", wrq.u.data.length, buffer[0]);
		for(i = 1; i < wrq.u.data.length; i++)
			send_data(sock, ":%02X", buffer[i]);
		send_data(sock, "]\n");
#endif
		send_data(sock, "%-8.16s  Scan completed :\n", ifname);
		iw_init_event_stream(&stream, (char *) buffer, wrq.u.data.length);
		do
		{
			/* Extract an event and print it */
			ret = iw_extract_event_stream(&stream, &iwe,
				range.we_version_compiled);
			if(ret > 0)
				print_scanning_token(&stream, &iwe, &state, &range, has_range);
		} while(ret > 0);
	}
	else {
		send_data(sock, "%-8.16s  No scan results\n\n", ifname);
		free(buffer);
		return(0);
	}
	return 0;
}

int run_iwlist(char const * interface)
{
	/* generic raw socket desc. */
	int skfd;
	if((skfd = iw_sockets_open()) < 0)
	{
		perror("socket");
		return -1;
	}

	print_scanning_info(skfd, (char *)interface, NULL, 0);
	/* Close the socket. */
	iw_sockets_close(skfd);
	return 0;
}
