#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alive_client.h"

#include <dcgi.h>

#include "ioc_alive.h"

char *Server;
int DB_Port;


void time_string( uint32_t timeval, char *buffer)
{
  unsigned int temp;
  int days, hours, mins, secs;
  char *p;

  temp = timeval;
  secs = temp%60;
  temp /= 60;
  mins = temp%60;
  temp /= 60;
  hours = temp%24;
  temp /= 24;
  days = temp;

  // get end of string
  p = strchr( buffer, '\0');

  if( days)
    sprintf(p, "%d day%s, %d hour%s", days, (days == 1) ? "" : "s", 
            hours, (hours == 1) ? "" : "s" );
  else if( hours)
    sprintf(p, "%d hour%s, %d minute%s", hours, (hours == 1) ? "" : "s" ,
            mins, (mins == 1) ? "" : "s" );
  else if( mins)
    sprintf(p, "%d minute%s, %d second%s", mins, (mins == 1) ? "" : "s" ,
            secs, (secs == 1) ? "" : "s");
  else sprintf(p, "%d second%s", secs, (secs == 1) ? "" : "s" );
}


void print_environment( struct alive_env *env, char *script_name)
{
  int i;

  if( env == NULL)
    {
      printf("<h3>No Environment Variables Recorded</h3>\n" );
    }
  else
    {
      printf("<h3>Environment Variables</h3>\n"
             "<ul>");
      for( i = 0; i < env->number_envvar; i++)
        {
          if( !strcmp("ENGINEER", env->envvar_key[i]) )
            printf( "<li>%s = <a href=%s?eng=%s>%s</a></li>\n", 
                    env->envvar_key[i], script_name, env->envvar_value[i], 
                    env->envvar_value[i]);
          else if( !strcmp("GROUP", env->envvar_key[i]) )
            printf( "<li>%s = <a href=%s?grp=%s>%s</a></li>\n", 
                    env->envvar_key[i], script_name, 
                    env->envvar_value[i], env->envvar_value[i]);
          else
            printf( "<li>%s = %s</li>\n", env->envvar_key[i], 
                    env->envvar_value[i]);
        }
      printf("%s", "</ul>\n");

      switch(env->extra_type)
        {
        case VXWORKS:
          {
            struct alive_iocinfo_extra_vxworks *vw;
          
            vw = env->extra;

            printf("<h3>vxWorks Boot Parameters</h3>\n"
                   "<ul>");
            printf("<li>boot device = %s</li>\n", vw->bootdev);
            printf("<li>unit number = %d</li>\n", vw->unitnum);
            printf("<li>processor number = %d</li>\n", vw->procnum);
            printf("<li>boot host name = %s</li>\n", vw->boothost_name);
            printf("<li>boot file = %s</li>\n", vw->bootfile);
            if( vw->address[0] != '\0')
              printf("<li>IP address = %s</li>\n", vw->address);
            if( vw->backplane_address[0] != '\0')
              printf("<li>backplane IP address = %s</li>\n", 
                     vw->backplane_address );
            if( vw->boothost_address[0] != '\0')
              printf("<li>boot host IP address = %s</li>\n", 
                     vw->boothost_address);
            if( vw->gateway_address[0] != '\0')
              printf("<li>gateway IP address = %s</li>\n", 
                     vw->gateway_address);
            printf("<li>flags = 0x%X</li>\n", vw->flags );
            if( vw->target_name[0] != '\0')
              printf("<li>target name = %s</li>\n", vw->target_name);
            if( vw->startup_script[0] != '\0')
              printf("<li>startup script = %s</li>\n", vw->startup_script);
            if( vw->other[0] != '\0')
              printf("<li>other = %s</li>\n", vw->other);
            printf("%s", "</ul>\n");
          }
          break;
        case LINUX:
          {
            struct alive_iocinfo_extra_linux *lnx;

            lnx = env->extra;

            printf("<h3>Linux Parameters</h3>\n"
                   "<ul><li>User = %s</li>\n"
                   "<li>Group = %s</li>\n"
                   "<li>Hostname = %s</li></ul>\n", 
                   lnx->user, lnx->group, lnx->hostname);
          }
          break;
        case DARWIN:
          {
            struct alive_iocinfo_extra_darwin *dwn;

            dwn = env->extra;

            printf("<h3>Darwin Parameters</h3>\n"
                   "<ul><li>User = %s</li>\n"
                   "<li>Group = %s</li>\n"
                   "<li>Hostname = %s</li></ul>\n", 
                   dwn->user, dwn->group, dwn->hostname);
          }
          break;
        case WINDOWS:
          {
            struct alive_iocinfo_extra_windows *win;

            win = env->extra;

            printf("<h3>Windows Parameters</h3>\n"
                   "<ul><li>User = %s</li>\n"
                   "<li>Machine = %s</li></ul>\n", 
                   win->user, win->machine);
          }
          break;
        }
    }
}


void create_statusstring( char *statusstring, int status, 
                          time_t ioc_time_value, 
                          time_t current_time, time_t daemon_start_time)
{
  switch( status)
    {
    case STATUS_UNKNOWN:
      strcpy( statusstring, 
              "<span class=\"status_uncertain\">Uncertain</span>");
      break;
    case STATUS_DOWN_UNKNOWN:
      strcpy( statusstring, 
              "<span class=\"status_down\">Down</span>&nbsp;&darr; (>&nbsp;");
      time_string( current_time - daemon_start_time, statusstring);
      strcat( statusstring, ")");
      break;
    case STATUS_DOWN:
      strcpy( statusstring, 
              "<span class=\"status_down\">Down</span>&nbsp;&darr; (");
      time_string( current_time - ioc_time_value, statusstring);
      strcat( statusstring, ")");
      break;
    case STATUS_UP:
      strcpy( statusstring, 
              "<span class=\"status_up\">Up</span>&nbsp;&uarr; (");
      time_string( current_time - ioc_time_value, statusstring);
      strcat( statusstring, ")");
      break;
    case STATUS_CONFLICT:
      strcpy( statusstring, 
              "<span class=\"status_conflict\">Conflict</span>&nbsp;&hArr; (");
      time_string( current_time - ioc_time_value, statusstring);
      strcat( statusstring, ")");
      break;
    }
}



void print_ioc( char *ioc_name, char *script_name)
{
  struct alive_db *db;
  struct alive_ioc *ioc;

  time_t now;
  char time_buffer[256];

  char statusstring[256];

  db = alive_get_ioc( Server, DB_Port, ioc_name);
  if( db == NULL)
    {  
      printf("Content-Type: text/html\n\n");
      printf( error_string, "Unknown IOC.");
      return;
    }
  if( !db->number_ioc)
    {
      printf("Content-Type: text/html\n\n");
      printf( error_string, "No IOC(s) found.");
      return;
    }

  ioc = db->ioc;


  create_statusstring( statusstring, ioc->status, ioc->time_value,
                       db->current_time, db->start_time);


  now = time(NULL);
  time_buffer[255] = '\0';
  strftime( time_buffer, 255, "%H:%M:%S %Y-%m-%d", localtime( &now));


  printf("Content-Type: text/html\n\n"
         "<html><head>\n"
         "<title>%s Information</title>\n"
         "<style>\n"
         "body { bgcolor: white }\n"
         "span.status_up { color: green }\n"
         "span.status_down { color: red }\n"
         "span.status_conflict { color: cyan }\n"
         "span.status_unknown { color: orange }\n"
         "span.msg_zero { color: green }\n"
         "span.msg_nonzero { color: red }\n"
         //         "a { text-decoration: none; }\n"
         "</style>\n"
         "</head><body>\n", ioc_name);
 
  printf("<h2>%s</h2>\n", ioc->ioc_name );
  printf("Information retrieved at %s\n", time_buffer);


  if( ioc->status == STATUS_CONFLICT)
    printf( "<h3><a href=%s?conflicts=%s>Conflict Information</a></h3>\n", 
            script_name, ioc_name);


  printf("<h3>General Information</h3>\n<ul>\n"
         "<li>Status: %s</li>\n", statusstring);
  printf("<li>IP address = <a href=%s?net=%d.%d.%d>%d.%d.%d</a>.%d</li>\n",
         script_name, ioc->ip_address[0], ioc->ip_address[1], 
         ioc->ip_address[2], ioc->ip_address[0], ioc->ip_address[1],
         ioc->ip_address[2], ioc->ip_address[3]);
  if( ioc->user_msg == 0)
    printf("<li>User Message = <span class=\"msg_zero\">%d</span></li>\n"
           "</ul>\n", ioc->user_msg );
  else
    printf("<li>User Message = <span class=\"msg_nonzero\">%d</span></li>\n"
           "</ul>\n", ioc->user_msg );

  print_environment(ioc->environment, script_name);

  printf( "<h3><a href=%s?events=%s>Event History</a></h3>\n", 
          script_name, ioc_name);

  printf( "<h3><a href=%s?debug=%s>Tracked Recent Boot Instances</a> (for debugging)</h3>\n", 
          script_name, ioc_name);

  printf("%s", "</body></html>\n");

}



void print_ioc_events( char *ioc_name, char *script_name)
{
  struct alive_ioc_event_db *events;
  struct alive_ioc_event_item *item;

  time_t current_time;
  struct tm *ct;

  char *event_strings[] = {"NONE", "FAIL", "BOOT", "RECOVER", "MESSAGE", 
                           "CONFLICT_START", "CONFLICT_STOP"};

  time_t now;
  char time_buffer[256];
  char statusstring[256];

  int i;

  events = alive_get_ioc_event_db( Server, DB_Port, ioc_name);
  if( events == NULL)
    {
      printf("Content-Type: text/html\n\n");
      printf( error_string, "Unknown IOC.");
      return;
    }
  if( !events->number)
    {
      printf("Content-Type: text/html\n\n");
      printf( error_string, "No events found.");
      return;
    }


  now = time(NULL);
  time_buffer[255] = '\0';
  strftime( time_buffer, 255, "%H:%M:%S %Y-%m-%d", localtime( &now));
      
  printf("Content-Type: text/html\n\n"
         "<html><head>\n"
         "<style>\n"
         "<title>%s Events</title>\n"
         "body { bgcolor: white }\n"
         "</style>\n"
         "</head><body>\n", ioc_name);
 
  printf("<h2>%s</h2>\n", ioc_name );
  printf("Information retrieved at %s\n", time_buffer);

  printf("<h3>IOC Events (most recent on top)</h3>\n"
         "<ul>\n");

  item = &(events->instances[events->number - 1]);
  for( i = 0; i < events->number; i++)
    {
      current_time = (time_t) item->time;
      ct = localtime( &current_time);
      strftime( statusstring, 255, "%Y-%m-%d %H:%M:%S", ct);

      printf("<li>%s (%d.%d.%d.%d) [%d] %s - ",
             statusstring, item->ip_address[0], item->ip_address[1],
             item->ip_address[2], item->ip_address[3],
             item->user_msg, event_strings[item->event]);

      statusstring[0] = '\0';
      time_string( events->current_time - item->time, statusstring);
      printf("%s ago</li>\n", statusstring);

      item--;
    }
  printf("%s", "</ul>\n");
  printf("%s", "</body></html>\n");
}


void print_ioc_details( char *ioc_name, char *script_name, int mode)
{
  struct alive_detailed_ioc *dioc;
  struct alive_instance *inst;

  time_t now, t;
  struct tm *ct;
  char time_buffer[256];
  char statusstring[256];

  int i;

  if( mode == 0)
    dioc = alive_get_debug( Server, DB_Port, ioc_name);
  else
    dioc = alive_get_conflicts( Server, DB_Port, ioc_name);
  if( dioc == NULL)
    {  
      printf("Content-Type: text/html\n\n");
      printf( error_string, "Unknown IOC.");
      return;
    }

  now = time(NULL);
  time_buffer[255] = '\0';
  strftime( time_buffer, 255, "%H:%M:%S %Y-%m-%d", localtime( &now));


  printf("Content-Type: text/html\n\n"
         "<html><head>\n"
         "<title>%s Conflict Information</title>\n"
         "<style>\n"
         "body { bgcolor: white }\n"
         "</style>\n"
         "</head><body>\n", ioc_name);
 
  printf("<h2>%s</h2>\n", ioc_name );
  printf("Information retrieved at %s\n", time_buffer);

  if( mode && (dioc->overall_status != STATUS_CONFLICT) )
    {
      printf("<h3>There's no conflict at this time.</h3>\n"
             "</body></html>\n");
      return;
    }
  
  if( !mode)
    {
      create_statusstring( statusstring, dioc->overall_status, 
                           dioc->overall_time_value,
                           dioc->current_time, dioc->start_time);
      printf("<h3>Overall Status: %s</h3>\n", statusstring);
    }

  inst = dioc->instances;
  for( i = 0; i < dioc->number_instances; i++)
    {
      switch( inst->status)
        {
        case INSTANCE_STATUS_UP:
          strcpy( statusstring, "UP");
          break;
        case INSTANCE_STATUS_DOWN:
          strcpy( statusstring, "DOWN");
          break;
        case INSTANCE_STATUS_UNTIMED_DOWN:
          strcpy( statusstring, "UNTIMED_DOWN");
          break;
        case INSTANCE_STATUS_MAYBE_UP:
          strcpy( statusstring, "MAYBE_UP");
          break;
        case INSTANCE_STATUS_MAYBE_DOWN:
          strcpy( statusstring, "MAYBE_DOWN");
          break;
        }

      printf("<h2>%s #%d</h2>\n"
             "<ul>\n"
             "<li>Status: %s</li>\n"
             "<li>IP address: %d.%d.%d.%d</li>\n"
             "<li>Origin Port: %d</li>\n"
             "<li>Incarnation: %d</li>\n"
             "<li>Period: %d</li>\n"
             "<li>Heartbeat: %d</li>\n",
             mode ? "Conflict" : "Instance", i+1, statusstring, 
             inst->ip_address[0], inst->ip_address[1], 
             inst->ip_address[2], inst->ip_address[3], inst->origin_port,
             inst->incarnation, inst->period, inst->heartbeat);
      
      t = (time_t) inst->boottime;
      ct = localtime( &t);
      strftime( time_buffer, 255, "%Y-%m-%d %H:%M:%S", ct);
      printf("<li>Boot Time: %s</li>\n", time_buffer);

      t = (time_t) inst->timestamp;
      ct = localtime( &t);
      strftime( time_buffer, 255, "%Y-%m-%d %H:%M:%S", ct);
      printf("<li>Ping Time Stamp: %s</li>\n", time_buffer);

      printf("<li>Reply Port: %d</li>\n"
             "<li>User Message: %d</li>\n",
             inst->reply_port, inst->user_msg);

      print_environment(inst->environment, script_name);

      printf("</ul>\n");

      inst++;
    }

  printf("%s", "</body></html>\n");
}

int main(int argc, char *argv[])
{
  struct alive_db *db;
  struct alive_ioc *iocptr;

  int i, j;
  int cnt;

  char *state_strings[5] = {"unknown","down_unknown", "down", "up", "conflict"};

  time_t now;
  char time_buffer[256];

  char *key, *value;
  char *arch, *top, *epics_base, *support, *engineer, *location, *group;
  char *nullstr = "";

  struct cgi_info cinfo;
  int cgi_mode = 1;

  char *good_list;

  char *grp = NULL;
  char *eng = NULL;
  char *ioc = NULL;
  char *events = NULL;
  char *conflicts = NULL;
  char *debug = NULL;
  char *subnet_str = NULL;
  unsigned char subnet[3] = {0,0,0};

  
  // this contains a dummy unless overwritten
  char *script = "ioc_alive.cgi";

  // means we are at command line
  if( getenv("GATEWAY_INTERFACE") == NULL)
    cgi_mode = 0;

  if( cgi_mode)
    {
      script = getenv("SCRIPT_NAME");

      cnt = cgi_parse_input(&cinfo);
      if( cnt < 0)
        /*   /\* serious CGI error *\/ */
        {
          printf("Content-Type: text/html\n\n");
          printf( error_string, cgi_get_error_string(cnt));
          return 1;
        }

      if( cnt)
        {
          if( cnt == 1) 
            {
              if( (ioc = cgi_get_value(&cinfo, "ioc")) != NULL)
                {
                }
              else if( (events = cgi_get_value(&cinfo, "events")) != NULL)
                {
                }
              else if( (conflicts = cgi_get_value(&cinfo, "conflicts")) != NULL)
                {
                }
              else if( (debug = cgi_get_value(&cinfo, "debug")) != NULL)
                {
                }
              else if( (grp = cgi_get_value(&cinfo, "grp")) != NULL)
                {
                }
              else if( (eng = cgi_get_value(&cinfo, "eng")) != NULL)
                {
                }
              else if( (subnet_str = cgi_get_value(&cinfo, "net")) != NULL)
                {
                }
              else 
                {
                  /* the input was either not there or wrong */
                  printf("Content-Type: text/html\n\n");
                  printf( error_string, "Incorrect CGI arguments.");
                  return 1;
                }
            }
          else
            {
              /* the input was either not there or wrong */
              printf("Content-Type: text/html\n\n");
              printf( error_string, "Incorrect CGI arguments.");
              return 1;
            }
        }
    }


#ifdef SERVER 
  Server = strdup(SERVER);
#else
  Server = alive_default_database_host();
#endif

#ifdef DB_PORT
  DB_Port = DB_PORT;
#else
  DB_Port = alive_default_database_port();
#endif

  if( ioc != NULL)
    {
      print_ioc( ioc, script);
      return 0;
    }
  if( events != NULL)
    {
      print_ioc_events( events, script);
      return 0;
    }
  if( debug != NULL)
    {
      print_ioc_details( debug, script, 0);
      return 0;
    }
  if( conflicts != NULL)
    {
      print_ioc_details( conflicts, script, 1);
      return 0;
    }
  if( subnet_str != NULL)
    {
      int val[3];
      int err_flag;

      err_flag = 0;
      cnt = sscanf( subnet_str, "%d.%d.%d", &val[0], &val[1], &val[2]);
      if( cnt == 3)
        {
          for( i = 0; i < 3; i++)
            {
              if( (val[i] >= 0) && (val[i] < 256) )
                subnet[i] = val[i];
              else
                err_flag = 1;
            }
        }
      else
        err_flag = 1;

      if( err_flag)
        {
          /* the input was either not there or wrong */
          printf("Content-Type: text/html\n\n");
          printf( error_string, "Incorrect subnet argument.");
          return 1;
        }
    }

  db = alive_get_db( Server, DB_Port);
  if( db == NULL)
    {
      printf("Content-Type: text/html\n\n");
      printf( error_string, "Server error!\n");
      return 1;
    }


  now = time(NULL);
  time_buffer[255] = '\0';
  strftime( time_buffer, 255, "%H:%M:%S %Y-%m-%d", localtime( &now));

  
  printf("Content-Type: text/html\n\n"
	 "<html><head>\n"
	 "<title>IOC Information</title>\n"
         "<style>\n"
         "body { bgcolor: white }\n"
         "span.status_up { color: green }\n"
         "span.status_down { color: red }\n"
         "span.status_conflict { color: cyan }\n"
         "span.status_unknown { color: orange }\n"
         "span.msg_zero { color: green }\n"
         "span.msg_nonzero { color: red }\n"
         "</style>\n"
	 "</head><body>\n");

  printf("<h1>APS IOC Information</h1>\n");
  printf("Information retrieved at %s\n", time_buffer);

  printf("<h2>General Information</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', iocs, 1)\">IOC</button>\n"
         "<button onclick=\"time_sort(iocs, 2)\">Time</button>\n"
         "<button onclick=\"address_sort(iocs, 3)\">Address</button>\n"
         "<button onclick=\"generic_sort(\'message\', iocs, 4)\">Message</button>\n"
         "<button onclick=\"generic_sort(\'type\', iocs, 5)\">Type</button>\n"
);
  printf("<p id=\"tbl\"></p>\n");

  printf("<h2>Standard Environment Variables</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', envvars, 11)\">IOC</button>\n"
         "<button onclick=\"generic_sort(\'group\', envvars, 12)\">GROUP</button>\n"
         "<button onclick=\"generic_sort(\'engineer\', envvars, 13)\">ENGINEER</button>\n"
         "<button onclick=\"generic_sort(\'arch\', envvars, 14)\">ARCH</button>\n"
         "<button onclick=\"generic_sort(\'support\', envvars, 15)\">SUPPORT</button>\n"
         );
  printf("<p id=\"tbl_envvar\"></p>\n");

  printf("<div id=\"vxworks_table\"><h2>VXWorks Boot Information</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', vxworks, 21)\">IOC</button>\n"
         "<button onclick=\"generic_sort(\'bootdev\', vxworks, 22)\">Bootdev</button>\n"
         "<button onclick=\"generic_sort(\'hostname\', vxworks, 23)\">Boot host</button>\n"
         "<button onclick=\"generic_sort(\'bootfile\', vxworks, 24)\">Boot File</button>\n");
  printf("<p id=\"tbl_vxworks\"></p></div>\n");

  printf("<div id=\"linux_table\"><h2>Linux Boot Information</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', linux, 31)\">IOC</button>\n"
         "<button onclick=\"generic_sort(\'user\', linux, 32)\">User</button>\n");
  printf("<p id=\"tbl_linux\"></p></div>\n");

  printf("<div id=\"darwin_table\"><h2>Darwin Boot Information</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', darwin, 41)\">IOC</button>\n"
         "<button onclick=\"generic_sort(\'user\', darwin, 42)\">User</button>\n");
  printf("<p id=\"tbl_darwin\"></p></div>\n");

  printf("<div id=\"windows_table\"><h2>Windows Boot Information</h2>\n");
  printf("Sorts: \n"
         "<button onclick=\"generic_sort(\'name\', windows, 51)\">IOC</button>\n"
         "<button onclick=\"generic_sort(\'user\', windows, 52)\">User</button>\n");
  printf("<p id=\"tbl_windows\"></p></div>\n");


  printf("<script type='text/javascript'>\n"
         "var sort=0;\n"
         "var sortdir=1;\n"
         "var current_time=%d;\n"
         "var start_time=%d;\n", 
         (int) db->current_time, (int) db->start_time );

  good_list = malloc( sizeof(char) * db->number_ioc);
  for( i = 0; i < db->number_ioc; i++)
    good_list[i] = 1;
  for( i = 0; i < db->number_ioc; i++)
    {
      iocptr = &db->ioc[i];
      
      if( subnet_str != NULL)
        {
          if( (iocptr->ip_address[0] != subnet[0]) ||
              (iocptr->ip_address[1] != subnet[1]) ||
              (iocptr->ip_address[2] != subnet[2]) )
            {
              good_list[i] = 0;
              continue;
            }
        }

      engineer = group = nullstr;

      if( iocptr->environment != NULL)
        for( j = 0; j < iocptr->environment->number_envvar; j++)
          {
            key = iocptr->environment->envvar_key[j];
            value = iocptr->environment->envvar_value[j];
            
            if( !strcmp( key, "ENGINEER") )
              engineer = value;
            if( !strcmp( key, "GROUP") )
              group = value;
          }

      if( (eng != NULL) && strcmp(eng, engineer) )
        {
          good_list[i] = 0;
          continue;
        }
      if( (grp != NULL) && strcmp(grp, group) )
        {
          good_list[i] = 0;
          continue;
        }
    }      


  //  cnt = 0;
  printf("var iocs = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      
      printf(" {name:\"%s\",status:\"%s\",", 
             iocptr->ioc_name, state_strings[iocptr->status]);
      printf("type:");
      if( iocptr->environment == NULL)
        printf("\"unknown\",");
      else
        {
          switch( iocptr->environment->extra_type)
            {
            case VXWORKS:
              printf("\"vxWorks\",");
              break;
            case LINUX:
              printf("\"Linux\",");
              break;
            case DARWIN:
              printf("\"Darwin\",");
              break;
            case WINDOWS:
              printf("\"Windows\",");
              break;
            default:
              printf("\"generic\",");
              break;
            }
        }
      printf("time_value:\"%d\",address:[\"%d\",\"%d\",\"%d\",\"%d\"],message:\"%d\"},\n", 
             (int) iocptr->time_value, 
             iocptr->ip_address[0], iocptr->ip_address[1], 
             iocptr->ip_address[2], iocptr->ip_address[3], 
             iocptr->user_msg);
    }
  printf("];\n");

  printf("var envvars = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      if( iocptr->environment == NULL)
        continue;
      
      arch = top = epics_base = support = engineer = location = group = nullstr;

      for( j = 0; j < iocptr->environment->number_envvar; j++)
        {
          key = iocptr->environment->envvar_key[j];
          value = iocptr->environment->envvar_value[j];

          if( !strcmp( key, "ARCH") )
            arch = value;
          else if( !strcmp( key, "TOP") )
            top = value;
          else if( !strcmp( key, "EPICS_BASE") )
            epics_base = value;
          else if( !strcmp( key, "SUPPORT") )
            support = value;
          else if( !strcmp( key, "ENGINEER") )
            engineer = value;
          else if( !strcmp( key, "LOCATION") )
            location = value;
          else if( !strcmp( key, "GROUP") )
            group = value;
        }

      printf(" {name:\"%s\",status:\"%s\",", 
             iocptr->ioc_name, state_strings[iocptr->status]);
      printf("arch:\"%s\",top:\"%s\",base:\"%s\",support:\"%s\","
             "engineer:\"%s\",location:\"%s\",group:\"%s\"},\n", 
             arch, top, epics_base, support, 
             engineer, location, group);

    }
  printf("];\n");


  printf("var linux = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      struct alive_iocinfo_extra_linux *lnx;

      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      if( (iocptr->environment == NULL) || 
          (iocptr->environment->extra_type != LINUX) )
        continue;
          
      lnx = iocptr->environment->extra;
      printf(" {name:\"%s\",status:\"%s\","
             "user:\"%s\",group:\"%s\",hostname:\"%s\"},\n",
             iocptr->ioc_name, state_strings[iocptr->status], 
             lnx->user, lnx->group, lnx->hostname);
    }
  printf("];\n");


  printf("var darwin = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      struct alive_iocinfo_extra_darwin *dwn;
      
      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      if( (iocptr->environment == NULL) || 
          (iocptr->environment->extra_type != DARWIN) )
        continue;
      
      dwn = iocptr->environment->extra;
      printf(" {name:\"%s\",status:\"%s\","
             "user:\"%s\",group:\"%s\",hostname:\"%s\"},\n",
             iocptr->ioc_name, state_strings[iocptr->status], 
             dwn->user, dwn->group, dwn->hostname);
    }
  printf("];\n");


  printf("var windows = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      struct alive_iocinfo_extra_windows *win;

      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      if( (iocptr->environment == NULL) ||
          (iocptr->environment->extra_type != WINDOWS) )
        continue;
      
      win = iocptr->environment->extra;
      printf(" {name:\"%s\",status:\"%s\","
             "user:\"%s\",machine:\"%s\"},\n",
             iocptr->ioc_name, state_strings[iocptr->status], 
             win->user, win->machine);
    }
  printf("];\n");


  printf("var vxworks = [\n");
  for( i = 0; i < db->number_ioc; i++)
    {
      struct alive_iocinfo_extra_vxworks *vw;

      if( !good_list[i])
        continue;
      iocptr = &db->ioc[i];
      if( (iocptr->environment == NULL) ||
          (iocptr->environment->extra_type != VXWORKS) )
        continue;
      
      vw = iocptr->environment->extra;
      printf(" {name:\"%s\",status:\"%s\","
             "bootdev:\"%s(%d,%d)\",host_name:\"%s\",boot_file:\"%s\","
             "vx_ip:\"%s\",backplane_ip:\"%s\",host_ip:\"%s\","
             "gateway_ip:\"%s\",flags:\"0x%X\",vx_name:\"%s\","
             "script:\"%s\"},\n", 
             iocptr->ioc_name, state_strings[iocptr->status], 
             vw->bootdev, vw->unitnum, vw->procnum, vw->boothost_name, 
             vw->bootfile, vw->address, vw->backplane_address, 
             vw->boothost_address, vw->gateway_address, vw->flags, 
             vw->target_name, vw->startup_script);
    }
  printf("];\n");

  free( good_list);

  printf(javascript_functions, script, script, script, 
	 script, script, script, script, script, script);

  printf("\n</script>\n</body></html>\n");

  return 0;
}

