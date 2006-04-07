/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd is copyright (C) 2002-2006 ChatSpike-Dev.
 *                       E-mail:
 *                <brain@chatspike.net>
 *                <Craig@chatspike.net>
 *
 * Written by Craig Edwards, Craig McLure, and others.
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

using namespace std;

#include "inspircd_config.h"
#include "inspircd.h"
#include "inspircd_io.h"
#include <time.h>
#include <string>
#include "users.h"
#include "ctables.h"
#include "globals.h"
#include "wildcard.h"
#include "message.h"
#include "commands.h"
#include "inspstring.h"
#include "helperfuncs.h"
#include "hashcomp.h"
#include "typedefs.h"
#include "cmd_die.h"

extern ServerConfig* Config;
extern std::vector<userrec*> all_opers;

void cmd_die::Handle (char **parameters, int pcnt, userrec *user)
{
	if (!strcmp(parameters[0],Config->diepass))
	{
		log(SPARSE, "/DIE command from %s!%s@%s, terminating in %d seconds...", user->nick, user->ident, user->host, Config->DieDelay);
		
		/* This would just be WriteOpers(), but as we just sleep() and then die then the write buffers never get flushed.
		 * so we iterate the oper list, writing the message and immediately trying to flush their write buffer.
		 */
		
		for (std::vector<userrec*>::iterator i = all_opers.begin(); i != all_opers.end(); i++)
		{
			userrec* a = *i;
			
			if (IS_LOCAL(a) && (a->modebits & UM_SERVERNOTICE))
			{
				WriteServ(a->fd, "NOTICE %s :*** DIE command from %s!%s@%s, terminating...", a->nick, a->nick, a->ident, a->host);
				a->FlushWriteBuf();
			}
		}
		
		sleep(Config->DieDelay);
		Exit(ERROR);
	}
	else
	{
		log(SPARSE, "Failed /DIE command from %s!%s@%s", user->nick, user->ident, user->host);
		WriteOpers("*** Failed DIE Command from %s!%s@%s.",user->nick,user->ident,user->host);
	}
}
