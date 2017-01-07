/**************************************************************************
 * Moesoft SMS Pager v1.0 (C)2003 Michael Irving (moe@moenuts.com)        *
 * (C)2003 All Rights Reserved, Protected Under Canadian Copyright Laws   *
 **************************************************************************
 * Un-Authorized Use Of This Code Strictly Prohibited.  Written Concent   *
 * MUST be obtained for legal use of this code in whole or in part.       *
 **************************************************************************/

#define SMS_GATEWAY_ID "mytalker@myhost.com"  /* What is the "from" address */
#define SMS_SEND_CMD   "/usr/sbin/sendmail %s < %s"  /* Sytstem Mailer    */
#define SMS_LOGIN_MAX  		5	/* SMS Maximum Messages Per Login */
#define SMS_MSG_COST   		100	/* SMS Message Cost               */
#define SMS_OWNER_OK   		1   	/* SMS Owner Can Send Free SMS    */
#define SMS_CODE_PREF  		"SMS69" /* SMS Verification Code Prefix   */
#define SMS_DEFAULT_PROVIDER	0    /* Set a Default Provider (0 = None) */
#define SMS_MAX_PAGER_LEN	20   /* Maximum Phone Number Length       */
#define SMS_VERIFY_CODE_LEN	8    /* Maximum Verify Code Length        */
#define SMS_MAX_MESSAGE_LEN     130  /* Most SMS Gateways = 160 - 255     */
#define SMS_MAX_PROVIDERS	7    /* How Many Providers Are Available  */

/* Remember, with the maximum message length, there is a overhead for "from" address so we'll
   keep it smaller, Fido I know is 160 Chars Max, Some providers like Telus is 255 chars */


char *providers[SMS_MAX_PROVIDERS]={
	"None",
	"Fido (Canada)",
	"Rogers AT&T (Canada)",
	"Cellular One (USA)",
	"AT&T Wireless (USA)",
	"Sprint PCS",
	"Other" 
	};

char *sms_suffix[SMS_MAX_PROVIDERS]={
	"none",
	"fido.ca",
	"pcs.rogers.com",
	"utext.com",
	"attwireless.com",
	"pcs.sprint.com",
	"crazysms.com" 
	};


