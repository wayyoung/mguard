#include <stdlib.h>
#include <stdio.h>              /* for vsprintf */
#include <string.h>
#include "mguard.h"
/*


 * Environment handling routines
 *
 * Copyright 2000 by Gray Watson
 *
 * This file is part of the dmalloc package.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Gray Watson not be used in advertising
 * or publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The author may be contacted via http://dmalloc.com/
 *
 * $Id: env.c,v 1.37 2007/05/14 17:09:20 gray Exp $
 */

/*
 * This file contains short routine(s) to process the environment
 * variable(s) used by the library to get the runtime option(s).
 */

/*
 * Environmental labels.
 *
 * NOTE: the decision has been made _not_ to do the sizeof() hack for
 * portability reasons.
 */
#define ADDRESS_LABEL		"addr"
#define DEBUG_LABEL		"debug"
#define INTERVAL_LABEL		"inter"
#define LOCK_ON_LABEL		"lockon"
#define LOGFILE_LABEL		"log"
#define START_LABEL		"start"
#define LIMIT_LABEL		"limit"
#define RPID_LABEL       "rpid"

#define ASSIGNMENT_CHAR     '='

#define MGUARD_ENV_OPTION      "MGUARD_OPTION"
#define MGUARD_ENV_LOG         "MGUARD_LOG"

#define ASSIGNMENT_CHAR		'='

static	char env_logpath[512]	= { '\0' }; /* storage for env path */
char *mguard_env_logpath=NULL;

static long env_opt_debug=0;
static size_t env_opt_rpid=0;



#define DMALLOC_PNT     void *
#define DMALLOC_FREE_RET    void

extern long hex_to_long(const char *str);

typedef struct {
  char      *at_string;     /* attribute string */
  unsigned long at_value;       /* value for the item */
  char      *at_desc;       /* description string */
} attr_t;

//static  attr_t      attributes[];

/*
 * set pointer macro
 */
#define SET_POINTER(pnt, val) \
    do { \
      if ((pnt) != NULL) { \
        (*(pnt)) = (val); \
          } \
        } while(0)


/*
 * Local ascii to unsigned long function
 */
extern unsigned long   loc_atoul(const char *str);

/****************************** local utilities ******************************/


void env_setup_logpath(void)
{

    const char   *logpath_str;
    size_t len=(sizeof(env_logpath)-1);
    logpath_str = getenv(MGUARD_ENV_LOG);

    if(logpath_str==NULL)
    {
        return;
    }


    len=((strlen(logpath_str)>len)?len:strlen(logpath_str));

    strncpy(env_logpath,logpath_str,len);
    env_logpath[len]='\0';


    //do we need to do some validation??
    mguard_env_logpath=env_logpath;
    printf("env_logpath:%s\n",env_logpath);
}

static void    _dmalloc_environ_process(const char *env_str,
//                 DMALLOC_PNT *addr_p,
//                 unsigned long *addr_count_p,

                 long *debug_p,
                 size_t *pid_p
//                 unsigned long *interval_p, int *lock_on_p,
//                 char **logpath_p, char **start_file_p,
//                 int *start_line_p,
//                 unsigned long *start_iter_p,
//                 unsigned long *start_size_p,
//                 unsigned long *limit_p
                 )
{
  char      *env_p, *this_p;
  char      buf[1024];
  int       len, done_b = 0;
//  unsigned int  flags = 0;
//  attr_t    *attr_p;

//  SET_POINTER(addr_p, NULL);
//  SET_POINTER(addr_count_p, 0);
  SET_POINTER(debug_p, 0);
//  SET_POINTER(interval_p, 0);
//  SET_POINTER(lock_on_p, 0);
//  SET_POINTER(logpath_p, NULL);
//  SET_POINTER(start_file_p, NULL);
//  SET_POINTER(start_line_p, 0);
//  SET_POINTER(start_iter_p, 0);
//  SET_POINTER(start_size_p, 0);
//  SET_POINTER(limit_p, 0);

  /* make a copy */
  (void)strncpy(buf, env_str, sizeof(buf));
  buf[sizeof(buf) - 1] = '\0';

  /* handle each of tokens, in turn */
  for (env_p = buf, this_p = buf; ! done_b; env_p++, this_p = env_p) {

    /* find the comma of end */
    for (;; env_p++) {
      if (*env_p == '\0') {
    done_b = 1;
    break;
      }
      if (*env_p == ',' && (env_p == buf || *(env_p - 1) != '\\')) {
    break;
      }
    }

    /* should we strip ' ' or '\t' here? */

    if (this_p == env_p) {
      continue;
    }

    *env_p = '\0';

//    len = strlen(ADDRESS_LABEL);
//    if (strncmp(this_p, ADDRESS_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      _dmalloc_address_break(this_p, addr_p, addr_count_p);
//      continue;
//    }

    len = strlen(DEBUG_LABEL);
    if (strncmp(this_p, DEBUG_LABEL, len) == 0
    && *(this_p + len) == ASSIGNMENT_CHAR) {
      this_p += len + 1;
      SET_POINTER(debug_p, hex_to_long(this_p));
      continue;
    }
    len = strlen(RPID_LABEL);
    if (strncmp(this_p, RPID_LABEL, len) == 0
    && *(this_p + len) == ASSIGNMENT_CHAR) {
      this_p += len + 1;
      SET_POINTER(pid_p, loc_atoul(this_p));
      continue;
    }

//    len = strlen(INTERVAL_LABEL);
//    if (strncmp(this_p, INTERVAL_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      SET_POINTER(interval_p, loc_atoul(this_p));
//      continue;
//    }
//
//    len = strlen(LOCK_ON_LABEL);
//    if (strncmp(this_p, LOCK_ON_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      SET_POINTER(lock_on_p, atoi(this_p));
//      continue;
//    }
//
//    /* get the dmalloc logfile name into a holding variable */
//    len = strlen(LOGFILE_LABEL);
//    if (strncmp(this_p, LOGFILE_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      (void)strncpy(log_path, this_p, sizeof(log_path));
//      log_path[sizeof(log_path) - 1] = '\0';
//      SET_POINTER(logpath_p, log_path);
//      continue;
//    }

//    /*
//     * start checking the heap after X iterations OR
//     * start at a file:line combination
//     */
//    len = strlen(START_LABEL);
//    if (strncmp(this_p, START_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      _dmalloc_start_break(this_p, start_file_p, start_line_p, start_iter_p,
//               start_size_p);
//      continue;
//    }
//
//    /* set the memory limit to the library */
//    len = strlen(LIMIT_LABEL);
//    if (strncmp(this_p, LIMIT_LABEL, len) == 0
//    && *(this_p + len) == ASSIGNMENT_CHAR) {
//      this_p += len + 1;
//      SET_POINTER(limit_p, loc_atoul(this_p));
//      continue;
//    }
//
//    /* need to check the short/long debug options */
//    for (attr_p = attributes; attr_p->at_string != NULL; attr_p++) {
//      if (strcmp(this_p, attr_p->at_string) == 0) {
//    flags |= attr_p->at_value;
//    break;
//      }
//    }
//    if (attr_p->at_string != NULL) {
//      continue;
//    }
  }

//  /* append the token settings to the debug setting */
//  if (debug_p != NULL) {
//    if (*debug_p == 0) {
//      *debug_p = flags;
//    }
//    else {
//      *debug_p |= flags;
//    }
//  }
}

void env_load_option(void)
{
    const char   *option_str;
    static char   options[1024];

    option_str = getenv(MGUARD_ENV_OPTION);
    /* process the options flag */
    if (option_str == NULL) {
      options[0] = '\0';
    }
    else {
      strncpy(options, option_str, sizeof(options));
      options[sizeof(options) - 1] = '\0';
    }

    if(option_str==NULL)
    {
        printf("No MGUARD_OPTION is set!!\n");

    }

    _dmalloc_environ_process(options,&env_opt_debug,&env_opt_rpid);



}

//long env_get_option(void)
//{
//    return mguard_env_option;
//}