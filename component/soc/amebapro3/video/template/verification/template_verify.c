#include "ameba_soc.h"

int template_verification(void)
{
	DiagPrintf("Template test start\n\r");
	return 0;
}

/*******************************************/
/*			Add command to table           */
/*******************************************/
u32
CmdTemplateTest(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	DiagPrintf("Template command\n\rargc %d\n\r", argc);
	
	for(int i=0;i<argc;i++){
		DiagPrintf("argv[%d] = %s\n\r", i, argv[i]);
	}
	
	if (_strcmp((const char *)argv[0], "run") == 0 ){
		if(template_verification() == 0)
			DiagPrintf("Pass\n\r");
		else
			DiagPrintf("Fail\n\r");
	}

	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE video_template_test_cmd[] = {
	/* command string is upper case only, and user input argv[0] in console will convert to upper case */
	{
		"TEMPLATE", CmdTemplateTest
	},
	{
		"TEMPLATE2", CmdTemplateTest
	},	
};
