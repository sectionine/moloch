#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "moloch.h"

/******************************************************************************/
void bt_classify(MolochSession_t *session, const unsigned char *UNUSED(data), int UNUSED(len))
{
    moloch_nids_add_tag(session, "protocol:bittorrent");
    moloch_nids_add_protocol(session, "bittorrent");
}
/******************************************************************************/
void rdp_classify(MolochSession_t *session, const unsigned char *data, int len)
{
    if (len > 5 && data[3] <= len && data[4] == (data[3] - 5) && data[5] == 0xe0) {
        moloch_nids_add_tag(session, "protocol:rdp");
        moloch_nids_add_protocol(session, "rdp");
    }
}
/******************************************************************************/
void imap_classify(MolochSession_t *session, const unsigned char *data, int len)
{
    if (moloch_memstr((const char *)data+5, len-5, "IMAP", 4)) {
        moloch_nids_add_tag(session, "protocol:imap");
        moloch_nids_add_protocol(session, "imap");
    }
}
/******************************************************************************/
void pop3_classify(MolochSession_t *session, const unsigned char *UNUSED(data), int UNUSED(len))
{
    moloch_nids_add_tag(session, "protocol:pop3");
    moloch_nids_add_protocol(session, "pop3");
}
/******************************************************************************/
void gh0st_classify(MolochSession_t *session, const unsigned char *data, int len)
{
    if (data[13] == 0x78 &&  
        (((data[8] == 0) && (data[7] == 0) && (((data[6]&0xff) << (uint32_t)8 | (data[5]&0xff)) == len)) ||  // Windows
         ((data[5] == 0) && (data[6] == 0) && (((data[7]&0xff) << (uint32_t)8 | (data[8]&0xff)) == len)))) { // Mac
        moloch_nids_add_tag(session, "protocol:gh0st");
        moloch_nids_add_protocol(session, "gh0st");
    }

    if (data[7] == 0 && data[8] == 0 && data[11] == 0 && data[12] == 0 && data[13] == 0x78 && data[14] == 0x9c) {
        moloch_nids_add_tag(session, "protocol:gh0st-improved");
        moloch_nids_add_protocol(session, "gh0st");
    }
}
/******************************************************************************/
void other220_classify(MolochSession_t *session, const unsigned char *data, int len)
{
    if (g_strstr_len((char *)data, len, "LMTP") != 0) {
        moloch_nids_add_tag(session, "protocol:lmtp");
        moloch_nids_add_protocol(session, "lmtp");
    }
    else if (g_strstr_len((char *)data, len, "SMTP") == 0) {
        moloch_nids_add_tag(session, "protocol:ftp");
        moloch_nids_add_protocol(session, "ftp");
    }
}
/******************************************************************************/
void moloch_parser_init()
{
    moloch_parsers_classifier_register_tcp("bt", 0, (unsigned char*)"\x13" "BitTorrent protocol", 20, bt_classify);
    moloch_parsers_classifier_register_tcp("rdp", 0, (unsigned char*)"\x03\x00", 2, rdp_classify);
    moloch_parsers_classifier_register_tcp("imap", 0, (unsigned char*)"* OK ", 5, imap_classify);
    moloch_parsers_classifier_register_tcp("pop3", 0, (unsigned char*)"+OK POP3 ", 9, pop3_classify);
    moloch_parsers_classifier_register_tcp("gh0st", 14, 0, 0, gh0st_classify);
    moloch_parsers_classifier_register_tcp("other220", 0, (unsigned char*)"220 ", 5, other220_classify);

    moloch_parsers_classifier_register_udp("bt", 0, (unsigned char*)"d1:a", 4, bt_classify);
    moloch_parsers_classifier_register_udp("bt", 0, (unsigned char*)"d1:r", 4, bt_classify);
    moloch_parsers_classifier_register_udp("bt", 0, (unsigned char*)"d1:q", 4, bt_classify);
}

