/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

/*
 * MT safe
 */

/**
 * SECTION:warnings
 * @Title: Message Output and Debugging Functions
 * @Short_description: functions to output messages and help debug applications
 *
 * These functions provide support for outputting messages.
 *
 * The g_return family of macros (g_return_if_fail(),
 * g_return_val_if_fail(), g_return_if_reached(),
 * g_return_val_if_reached()) should only be used for programming
 * errors, a typical use case is checking for invalid parameters at
 * the beginning of a public function. They should not be used if
 * you just mean "if (error) return", they should only be used if
 * you mean "if (bug in program) return". The program behavior is
 * generally considered undefined after one of these checks fails.
 * They are not intended for normal control flow, only to give a
 * perhaps-helpful warning before giving up.
 *
 * Structured logging output is supported using g_log_structured(). This differs
 * from the traditional g_log() API in that log messages are handled as a
 * collection of key每value pairs representing individual pieces of information,
 * rather than as a single string containing all the information in an arbitrary
 * format.
 *
 * The support for structured logging was motivated by the following needs (some
 * of which were supported previously; others weren＊t):
 *  * Support for multiple logging levels.
 *  * Structured log support with the ability to add `MESSAGE_ID`s (see
 *    g_log_structured()).
 *  * Moving the responsibility for filtering log messages from the program to
 *    the log viewer 〞 instead of libraries and programs installing log handlers
 *    (with g_log_set_handler()) which filter messages before output, all log
 *    messages are outputted, and the log viewer program (such as `journalctl`)
 *    must filter them. This is based on the idea that bugs are sometimes hard
 *    to reproduce, so it is better to log everything possible and then use
 *    tools to analyse the logs than it is to not be able to reproduce a bug to
 *    get additional log data. Code which uses logging in performance-critical
 *    sections should compile out the g_log_structured() calls in
 *    release builds, and compile them in in debugging builds.
 *  * A single writer function which handles all log messages in a process, from
 *    all libraries and program code; rather than multiple log handlers with
 *    poorly defined interactions between them. This allows a program to easily
 *    change its logging policy by changing the writer function, for example to
 *    log to an additional location or to change what logging output fallbacks
 *    are used. The log writer functions provided by GLib are exposed publicly
 *    so they can be used from programs＊ log writers. This allows log writer
 *    policy and implementation to be kept separate.
 *  * If a library wants to add standard information to all of its log messages
 *    (such as library state) or to redact private data (such as passwords or
 *    network credentials), it should use a wrapper function around its
 *    g_log_structured() calls or implement that in the single log writer
 *    function.
 *  * If a program wants to pass context data from a g_log_structured() call to
 *    its log writer function so that, for example, it can use the correct
 *    server connection to submit logs to, that user data can be passed as a
 *    zero-length #GLogField to g_log_structured_array().
 *  * Color output needed to be supported on the terminal, to make reading
 *    through logs easier.
 */

#include "config.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <locale.h>
#include <errno.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include "glib-init.h"
#include "galloca.h"
#include "gbacktrace.h"
#include "gcharset.h"
#include "gconvert.h"
#include "genviron.h"
#include "gmem.h"
#include "gprintfint.h"
#include "gtestutils.h"
#include "gthread.h"
#include "gstrfuncs.h"
#include "gstring.h"
#include "gpattern.h"

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

#ifdef G_OS_WIN32
#include <process.h>		/* For getpid() */
#include <io.h>
#  define _WIN32_WINDOWS 0x0401 /* to get IsDebuggerPresent */
#  include <windows.h>
#endif

#ifdef HAVE_LIBSYSTEMD
#define SD_JOURNAL_SUPPRESS_LOCATION 1
#include <sys/uio.h>
#include <systemd/sd-journal.h>
#endif


/**
 * SECTION:messages
 * @title: Message Logging
 * @short_description: versatile support for logging messages
 *     with different levels of importance
 *
 * These functions provide support for logging error messages
 * or messages used for debugging.
 *
 * There are several built-in levels of messages, defined in
 * #GLogLevelFlags. These can be extended with user-defined levels.
 */

/**
 * G_LOG_DOMAIN:
 *
 * Defines the log domain.
 *
 * For applications, this is typically left as the default %NULL
 * (or "") domain. Libraries should define this so that any messages
 * which they log can be differentiated from messages from other
 * libraries and application code. But be careful not to define
 * it in any public header files.
 *
 * For example, GTK+ uses this in its Makefile.am:
 * |[
 * AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Gtk\"
 * ]|
 */

/**
 * G_LOG_FATAL_MASK:
 *
 * GLib log levels that are considered fatal by default.
 */

/**
 * GLogFunc:
 * @log_domain: the log domain of the message
 * @log_level: the log level of the message (including the
 *     fatal and recursion flags)
 * @message: the message to process
 * @user_data: user data, set in g_log_set_handler()
 *
 * Specifies the prototype of log handler functions.
 *
 * The default log handler, g_log_default_handler(), automatically appends a
 * new-line character to @message when printing it. It is advised that any
 * custom log handler functions behave similarly, so that logging calls in user
 * code do not need modifying to add a new-line character to the message if the
 * log handler is changed.
 */

/**
 * GLogLevelFlags:
 * @G_LOG_FLAG_RECURSION: internal flag
 * @G_LOG_FLAG_FATAL: internal flag
 * @G_LOG_LEVEL_ERROR: log level for errors, see g_error().
 *     This level is also used for messages produced by g_assert().
 * @G_LOG_LEVEL_CRITICAL: log level for critical warning messages, see
 *     g_critical().
 *     This level is also used for messages produced by g_return_if_fail()
 *     and g_return_val_if_fail().
 * @G_LOG_LEVEL_WARNING: log level for warnings, see g_warning()
 * @G_LOG_LEVEL_MESSAGE: log level for messages, see g_message()
 * @G_LOG_LEVEL_INFO: log level for informational messages, see g_info()
 * @G_LOG_LEVEL_DEBUG: log level for debug messages, see g_debug()
 * @G_LOG_LEVEL_MASK: a mask including all log levels
 *
 * Flags specifying the level of log messages.
 *
 * It is possible to change how GLib treats messages of the various
 * levels using g_log_set_handler() and g_log_set_fatal_mask().
 */

/**
 * G_LOG_LEVEL_USER_SHIFT:
 *
 * Log levels below 1<<G_LOG_LEVEL_USER_SHIFT are used by GLib.
 * Higher bits can be used for user-defined log levels.
 */

/**
 * g_message:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * A convenience function/macro to log a normal message.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 */

/**
 * g_warning:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * A convenience function/macro to log a warning message.
 *
 * This is not intended for end user error reporting. Use of #GError is
 * preferred for that instead, as it allows calling functions to perform actions
 * conditional on the type of error.
 *
 * You can make warnings fatal at runtime by setting the `G_DEBUG`
 * environment variable (see
 * [Running GLib Applications](glib-running.html)).
 *
 * If g_log_default_handler() is used as the log handler function,
 * a newline character will automatically be appended to @..., and
 * need not be entered manually.
 */

/**
 * g_critical:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * Logs a "critical warning" (#G_LOG_LEVEL_CRITICAL).
 * It's more or less application-defined what constitutes
 * a critical vs. a regular warning. You could call
 * g_log_set_always_fatal() to make critical warnings exit
 * the program, then use g_critical() for fatal errors, for
 * example.
 *
 * You can also make critical warnings fatal at runtime by
 * setting the `G_DEBUG` environment variable (see
 * [Running GLib Applications](glib-running.html)).
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 */

/**
 * g_error:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * A convenience function/macro to log an error message.
 *
 * This is not intended for end user error reporting. Use of #GError is
 * preferred for that instead, as it allows calling functions to perform actions
 * conditional on the type of error.
 *
 * Error messages are always fatal, resulting in a call to
 * abort() to terminate the application. This function will
 * result in a core dump; don't use it for errors you expect.
 * Using this function indicates a bug in your program, i.e.
 * an assertion failure.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 *
 */

/**
 * g_info:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * A convenience function/macro to log an informational message. Seldom used.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 *
 * Such messages are suppressed by the g_log_default_handler() unless
 * the G_MESSAGES_DEBUG environment variable is set appropriately.
 *
 * Since: 2.40
 */

/**
 * g_debug:
 * @...: format string, followed by parameters to insert
 *     into the format string (as with printf())
 *
 * A convenience function/macro to log a debug message.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 *
 * Such messages are suppressed by the g_log_default_handler() unless
 * the G_MESSAGES_DEBUG environment variable is set appropriately.
 *
 * Since: 2.6
 */

/* --- structures --- */
typedef struct _GLogDomain	GLogDomain;
typedef struct _GLogHandler	GLogHandler;
struct _GLogDomain
{
  gchar		*log_domain;
  GLogLevelFlags fatal_mask;
  GLogHandler	*handlers;
  GLogDomain	*next;
};
struct _GLogHandler
{
  guint		 id;
  GLogLevelFlags log_level;
  GLogFunc	 log_func;
  gpointer	 data;
  GDestroyNotify destroy;
  GLogHandler	*next;
};


/* --- variables --- */
static GMutex         g_messages_lock;
static GLogDomain    *g_log_domains = NULL;
static GPrintFunc     glib_print_func = NULL;
static GPrintFunc     glib_printerr_func = NULL;
static GPrivate       g_log_depth;
static GPrivate       g_log_structured_depth;
static GLogFunc       default_log_func = g_log_default_handler;
static gpointer       default_log_data = NULL;
static GTestLogFatalFunc fatal_log_func = NULL;
static gpointer          fatal_log_data;
static GLogWriterFunc log_writer_func = g_log_writer_default;
static gpointer       log_writer_user_data = NULL;
static GDestroyNotify log_writer_user_data_free = NULL;

/* --- functions --- */

static void _g_log_abort (gboolean breakpoint);

static void
_g_log_abort (gboolean breakpoint)
{
  gboolean debugger_present;

  if (g_test_subprocess ())
    {
      /* If this is a test case subprocess then it probably caused
       * this error message on purpose, so just exit() rather than
       * abort()ing, to avoid triggering any system crash-reporting
       * daemon.
       */
      _exit (1);
    }

#ifdef G_OS_WIN32
  debugger_present = IsDebuggerPresent ();
#else
  /* Assume GDB is attached. */
  debugger_present = TRUE;
#endif /* !G_OS_WIN32 */

  if (debugger_present && breakpoint)
    G_BREAKPOINT ();
  else
    g_abort ();
}

#ifdef G_OS_WIN32
#  include <windows.h>
static gboolean win32_keep_fatal_message = FALSE;

/* This default message will usually be overwritten. */
/* Yes, a fixed size buffer is bad. So sue me. But g_error() is never
 * called with huge strings, is it?
 */
static gchar  fatal_msg_buf[1000] = "Unspecified fatal error encountered, aborting.";
static gchar *fatal_msg_ptr = fatal_msg_buf;

#undef write
static inline int
dowrite (int          fd,
	 const void  *buf,
	 unsigned int len)
{
  if (win32_keep_fatal_message)
    {
      memcpy (fatal_msg_ptr, buf, len);
      fatal_msg_ptr += len;
      *fatal_msg_ptr = 0;
      return len;
    }

  write (fd, buf, len);

  return len;
}
#define write(fd, buf, len) dowrite(fd, buf, len)

#endif

static void
write_string (FILE        *stream,
	      const gchar *string)
{
  fputs (string, stream);
}

static void
write_string_sized (FILE        *stream,
                    const gchar *string,
                    gssize       length)
{
  /* Is it nul-terminated? */
  if (length < 0)
    write_string (stream, string);
  else
    fwrite (string, 1, length, stream);
}

static GLogDomain*
g_log_find_domain_L (const gchar *log_domain)
{
  GLogDomain *domain;
  
  domain = g_log_domains;
  while (domain)
    {
      if (strcmp (domain->log_domain, log_domain) == 0)
	return domain;
      domain = domain->next;
    }
  return NULL;
}

static GLogDomain*
g_log_domain_new_L (const gchar *log_domain)
{
  GLogDomain *domain;

  domain = g_new (GLogDomain, 1);
  domain->log_domain = g_strdup (log_domain);
  domain->fatal_mask = G_LOG_FATAL_MASK;
  domain->handlers = NULL;
  
  domain->next = g_log_domains;
  g_log_domains = domain;
  
  return domain;
}

static void
g_log_domain_check_free_L (GLogDomain *domain)
{
  if (domain->fatal_mask == G_LOG_FATAL_MASK &&
      domain->handlers == NULL)
    {
      GLogDomain *last, *work;
      
      last = NULL;  

      work = g_log_domains;
      while (work)
	{
	  if (work == domain)
	    {
	      if (last)
		last->next = domain->next;
	      else
		g_log_domains = domain->next;
	      g_free (domain->log_domain);
	      g_free (domain);
	      break;
	    }
	  last = work;
	  work = last->next;
	}  
    }
}

static GLogFunc
g_log_domain_get_handler_L (GLogDomain	*domain,
			    GLogLevelFlags log_level,
			    gpointer	*data)
{
  if (domain && log_level)
    {
      GLogHandler *handler;
      
      handler = domain->handlers;
      while (handler)
	{
	  if ((handler->log_level & log_level) == log_level)
	    {
	      *data = handler->data;
	      return handler->log_func;
	    }
	  handler = handler->next;
	}
    }

  *data = default_log_data;
  return default_log_func;
}

/**
 * g_log_set_always_fatal:
 * @fatal_mask: the mask containing bits set for each level
 *     of error which is to be fatal
 *
 * Sets the message levels which are always fatal, in any log domain.
 * When a message with any of these levels is logged the program terminates.
 * You can only set the levels defined by GLib to be fatal.
 * %G_LOG_LEVEL_ERROR is always fatal.
 *
 * You can also make some message levels fatal at runtime by setting
 * the `G_DEBUG` environment variable (see
 * [Running GLib Applications](glib-running.html)).
 *
 * Libraries should not call this function, as it affects all messages logged
 * by a process, including those from other libraries.
 *
 * Structured log messages (using g_log_structured() and
 * g_log_structured_array()) are fatal only if the default log writer is used;
 * otherwise it is up to the writer function to determine which log messages
 * are fatal.
 *
 * Returns: the old fatal mask
 */
GLogLevelFlags
g_log_set_always_fatal (GLogLevelFlags fatal_mask)
{
  GLogLevelFlags old_mask;

  /* restrict the global mask to levels that are known to glib
   * since this setting applies to all domains
   */
  fatal_mask &= (1 << G_LOG_LEVEL_USER_SHIFT) - 1;
  /* force errors to be fatal */
  fatal_mask |= G_LOG_LEVEL_ERROR;
  /* remove bogus flag */
  fatal_mask &= ~G_LOG_FLAG_FATAL;

  g_mutex_lock (&g_messages_lock);
  old_mask = g_log_always_fatal;
  g_log_always_fatal = fatal_mask;
  g_mutex_unlock (&g_messages_lock);

  return old_mask;
}

/**
 * g_log_set_fatal_mask:
 * @log_domain: the log domain
 * @fatal_mask: the new fatal mask
 *
 * Sets the log levels which are fatal in the given domain.
 * %G_LOG_LEVEL_ERROR is always fatal.
 *
 * This has no effect on structured log messages (using g_log_structured() or
 * g_log_structured_array()). To change the fatal behaviour for specific log
 * messages, programs must install a custom log writer function using
 * g_log_set_writer_func().
 *
 * Returns: the old fatal mask for the log domain
 */
GLogLevelFlags
g_log_set_fatal_mask (const gchar   *log_domain,
		      GLogLevelFlags fatal_mask)
{
  GLogLevelFlags old_flags;
  GLogDomain *domain;
  
  if (!log_domain)
    log_domain = "";

  /* force errors to be fatal */
  fatal_mask |= G_LOG_LEVEL_ERROR;
  /* remove bogus flag */
  fatal_mask &= ~G_LOG_FLAG_FATAL;
  
  g_mutex_lock (&g_messages_lock);

  domain = g_log_find_domain_L (log_domain);
  if (!domain)
    domain = g_log_domain_new_L (log_domain);
  old_flags = domain->fatal_mask;
  
  domain->fatal_mask = fatal_mask;
  g_log_domain_check_free_L (domain);

  g_mutex_unlock (&g_messages_lock);

  return old_flags;
}

/**
 * g_log_set_handler:
 * @log_domain: (allow-none): the log domain, or %NULL for the default ""
 *     application domain
 * @log_levels: the log levels to apply the log handler for.
 *     To handle fatal and recursive messages as well, combine
 *     the log levels with the #G_LOG_FLAG_FATAL and
 *     #G_LOG_FLAG_RECURSION bit flags.
 * @log_func: the log handler function
 * @user_data: data passed to the log handler
 *
 * Sets the log handler for a domain and a set of log levels.
 * To handle fatal and recursive messages the @log_levels parameter
 * must be combined with the #G_LOG_FLAG_FATAL and #G_LOG_FLAG_RECURSION
 * bit flags.
 *
 * Note that since the #G_LOG_LEVEL_ERROR log level is always fatal, if
 * you want to set a handler for this log level you must combine it with
 * #G_LOG_FLAG_FATAL.
 *
 * Here is an example for adding a log handler for all warning messages
 * in the default domain:
 * |[<!-- language="C" --> 
 * g_log_set_handler (NULL, G_LOG_LEVEL_WARNING | G_LOG_FLAG_FATAL
 *                    | G_LOG_FLAG_RECURSION, my_log_handler, NULL);
 * ]|
 *
 * This example adds a log handler for all critical messages from GTK+:
 * |[<!-- language="C" --> 
 * g_log_set_handler ("Gtk", G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL
 *                    | G_LOG_FLAG_RECURSION, my_log_handler, NULL);
 * ]|
 *
 * This example adds a log handler for all messages from GLib:
 * |[<!-- language="C" --> 
 * g_log_set_handler ("GLib", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
 *                    | G_LOG_FLAG_RECURSION, my_log_handler, NULL);
 * ]|
 *
 * Returns: the id of the new handler
 */
guint
g_log_set_handler (const gchar	 *log_domain,
                   GLogLevelFlags log_levels,
                   GLogFunc       log_func,
                   gpointer       user_data)
{
  return g_log_set_handler_full (log_domain, log_levels, log_func, user_data, NULL);
}

/**
 * g_log_set_handler_full: (rename-to g_log_set_handler)
 * @log_domain: (allow-none): the log domain, or %NULL for the default ""
 *     application domain
 * @log_levels: the log levels to apply the log handler for.
 *     To handle fatal and recursive messages as well, combine
 *     the log levels with the #G_LOG_FLAG_FATAL and
 *     #G_LOG_FLAG_RECURSION bit flags.
 * @log_func: the log handler function
 * @user_data: data passed to the log handler
 * @destroy: destroy notify for @user_data, or %NULL
 *
 * Like g_log_sets_handler(), but takes a destroy notify for the @user_data.
 *
 * Returns: the id of the new handler
 *
 * Since: 2.46
 */
guint
g_log_set_handler_full (const gchar    *log_domain,
                        GLogLevelFlags  log_levels,
                        GLogFunc        log_func,
                        gpointer        user_data,
                        GDestroyNotify  destroy)
{
  static guint handler_id = 0;
  GLogDomain *domain;
  GLogHandler *handler;
  
  g_return_val_if_fail ((log_levels & G_LOG_LEVEL_MASK) != 0, 0);
  g_return_val_if_fail (log_func != NULL, 0);
  
  if (!log_domain)
    log_domain = "";

  handler = g_new (GLogHandler, 1);

  g_mutex_lock (&g_messages_lock);

  domain = g_log_find_domain_L (log_domain);
  if (!domain)
    domain = g_log_domain_new_L (log_domain);
  
  handler->id = ++handler_id;
  handler->log_level = log_levels;
  handler->log_func = log_func;
  handler->data = user_data;
  handler->destroy = destroy;
  handler->next = domain->handlers;
  domain->handlers = handler;

  g_mutex_unlock (&g_messages_lock);
  
  return handler_id;
}

/**
 * g_log_set_default_handler:
 * @log_func: the log handler function
 * @user_data: data passed to the log handler
 *
 * Installs a default log handler which is used if no
 * log handler has been set for the particular log domain
 * and log level combination. By default, GLib uses
 * g_log_default_handler() as default log handler.
 *
 * Returns: the previous default log handler
 *
 * Since: 2.6
 */
GLogFunc
g_log_set_default_handler (GLogFunc log_func,
			   gpointer user_data)
{
  GLogFunc old_log_func;
  
  g_mutex_lock (&g_messages_lock);
  old_log_func = default_log_func;
  default_log_func = log_func;
  default_log_data = user_data;
  g_mutex_unlock (&g_messages_lock);
  
  return old_log_func;
}

/**
 * g_test_log_set_fatal_handler:
 * @log_func: the log handler function.
 * @user_data: data passed to the log handler.
 *
 * Installs a non-error fatal log handler which can be
 * used to decide whether log messages which are counted
 * as fatal abort the program.
 *
 * The use case here is that you are running a test case
 * that depends on particular libraries or circumstances
 * and cannot prevent certain known critical or warning
 * messages. So you install a handler that compares the
 * domain and message to precisely not abort in such a case.
 *
 * Note that the handler is reset at the beginning of
 * any test case, so you have to set it inside each test
 * function which needs the special behavior.
 *
 * This handler has no effect on g_error messages.
 *
 * This handler also has no effect on structured log messages (using
 * g_log_structured() or g_log_structured_array()). To change the fatal
 * behaviour for specific log messages, programs must install a custom log
 * writer function using g_log_set_writer_func().
 *
 * Since: 2.22
 **/
void
g_test_log_set_fatal_handler (GTestLogFatalFunc log_func,
                              gpointer          user_data)
{
  g_mutex_lock (&g_messages_lock);
  fatal_log_func = log_func;
  fatal_log_data = user_data;
  g_mutex_unlock (&g_messages_lock);
}

/**
 * g_log_remove_handler:
 * @log_domain: the log domain
 * @handler_id: the id of the handler, which was returned
 *     in g_log_set_handler()
 *
 * Removes the log handler.
 */
void
g_log_remove_handler (const gchar *log_domain,
		      guint	   handler_id)
{
  GLogDomain *domain;
  
  g_return_if_fail (handler_id > 0);
  
  if (!log_domain)
    log_domain = "";
  
  g_mutex_lock (&g_messages_lock);
  domain = g_log_find_domain_L (log_domain);
  if (domain)
    {
      GLogHandler *work, *last;
      
      last = NULL;
      work = domain->handlers;
      while (work)
	{
	  if (work->id == handler_id)
	    {
	      if (last)
		last->next = work->next;
	      else
		domain->handlers = work->next;
	      g_log_domain_check_free_L (domain); 
	      g_mutex_unlock (&g_messages_lock);
              if (work->destroy)
                work->destroy (work->data);
	      g_free (work);
	      return;
	    }
	  last = work;
	  work = last->next;
	}
    } 
  g_mutex_unlock (&g_messages_lock);
  g_warning ("%s: could not find handler with id '%d' for domain \"%s\"",
	     G_STRLOC, handler_id, log_domain);
}

#define CHAR_IS_SAFE(wc) (!((wc < 0x20 && wc != '\t' && wc != '\n' && wc != '\r') || \
			    (wc == 0x7f) || \
			    (wc >= 0x80 && wc < 0xa0)))
     
static gchar*
strdup_convert (const gchar *string,
		const gchar *charset)
{
  if (!g_utf8_validate (string, -1, NULL))
    {
      GString *gstring = g_string_new ("[Invalid UTF-8] ");
      guchar *p;

      for (p = (guchar *)string; *p; p++)
	{
	  if (CHAR_IS_SAFE(*p) &&
	      !(*p == '\r' && *(p + 1) != '\n') &&
	      *p < 0x80)
	    g_string_append_c (gstring, *p);
	  else
	    g_string_append_printf (gstring, "\\x%02x", (guint)(guchar)*p);
	}
      
      return g_string_free (gstring, FALSE);
    }
  else
    {
      GError *err = NULL;
      
      gchar *result = g_convert_with_fallback (string, -1, charset, "UTF-8", "?", NULL, NULL, &err);
      if (result)
	return result;
      else
	{
	  /* Not thread-safe, but doesn't matter if we print the warning twice
	   */
	  static gboolean warned = FALSE; 
	  if (!warned)
	    {
	      warned = TRUE;
	      _g_fprintf (stderr, "GLib: Cannot convert message: %s\n", err->message);
	    }
	  g_error_free (err);
	  
	  return g_strdup (string);
	}
    }
}

/* For a radix of 8 we need at most 3 output bytes for 1 input
 * byte. Additionally we might need up to 2 output bytes for the
 * readix prefix and 1 byte for the trailing NULL.
 */
#define FORMAT_UNSIGNED_BUFSIZE ((GLIB_SIZEOF_LONG * 3) + 3)

static void
format_unsigned (gchar  *buf,
		 gulong  num,
		 guint   radix)
{
  gulong tmp;
  gchar c;
  gint i, n;

  /* we may not call _any_ GLib functions here (or macros like g_return_if_fail()) */

  if (radix != 8 && radix != 10 && radix != 16)
    {
      *buf = '\000';
      return;
    }
  
  if (!num)
    {
      *buf++ = '0';
      *buf = '\000';
      return;
    } 
  
  if (radix == 16)
    {
      *buf++ = '0';
      *buf++ = 'x';
    }
  else if (radix == 8)
    {
      *buf++ = '0';
    }
	
  n = 0;
  tmp = num;
  while (tmp)
    {
      tmp /= radix;
      n++;
    }

  i = n;

  /* Again we can't use g_assert; actually this check should _never_ fail. */
  if (n > FORMAT_UNSIGNED_BUFSIZE - 3)
    {
      *buf = '\000';
      return;
    }

  while (num)
    {
      i--;
      c = (num % radix);
      if (c < 10)
	buf[i] = c + '0';
      else
	buf[i] = c + 'a' - 10;
      num /= radix;
    }
  
  buf[n] = '\000';
}

/* string size big enough to hold level prefix */
#define	STRING_BUFFER_SIZE	(FORMAT_UNSIGNED_BUFSIZE + 32)

#define	ALERT_LEVELS		(G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING)

/* these are emitted by the default log handler */
#define DEFAULT_LEVELS (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE)
/* these are filtered by G_MESSAGES_DEBUG by the default log handler */
#define INFO_LEVELS (G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG)

static const gchar *log_level_to_color (GLogLevelFlags log_level,
                                        gboolean       use_color);
static const gchar *color_reset        (gboolean       use_color);

static FILE *
mklevel_prefix (gchar          level_prefix[STRING_BUFFER_SIZE],
                GLogLevelFlags log_level,
                gboolean       use_color)
{
  gboolean to_stdout = TRUE;

  /* we may not call _any_ GLib functions here */

  strcpy (level_prefix, log_level_to_color (log_level, use_color));

  switch (log_level & G_LOG_LEVEL_MASK)
    {
    case G_LOG_LEVEL_ERROR:
      strcat (level_prefix, "ERROR");
      to_stdout = FALSE;
      break;
    case G_LOG_LEVEL_CRITICAL:
      strcat (level_prefix, "CRITICAL");
      to_stdout = FALSE;
      break;
    case G_LOG_LEVEL_WARNING:
      strcat (level_prefix, "WARNING");
      to_stdout = FALSE;
      break;
    case G_LOG_LEVEL_MESSAGE:
      strcat (level_prefix, "Message");
      to_stdout = FALSE;
      break;
    case G_LOG_LEVEL_INFO:
      strcat (level_prefix, "INFO");
      break;
    case G_LOG_LEVEL_DEBUG:
      strcat (level_prefix, "DEBUG");
      break;
    default:
      if (log_level)
	{
	  strcat (level_prefix, "LOG-");
	  format_unsigned (level_prefix + 4, log_level & G_LOG_LEVEL_MASK, 16);
	}
      else
	strcat (level_prefix, "LOG");
      break;
    }

  strcat (level_prefix, color_reset (use_color));

  if (log_level & G_LOG_FLAG_RECURSION)
    strcat (level_prefix, " (recursed)");
  if (log_level & ALERT_LEVELS)
    strcat (level_prefix, " **");

#ifdef G_OS_WIN32
  if ((log_level & G_LOG_FLAG_FATAL) != 0 && !g_test_initialized ())
    win32_keep_fatal_message = TRUE;
#endif
  return to_stdout ? stdout : stderr;
}

typedef struct {
  gchar          *log_domain;
  GLogLevelFlags  log_level;
  gchar          *pattern;
} GTestExpectedMessage;

static GSList *expected_messages = NULL;

/**
 * g_logv:
 * @log_domain: (nullable): the log domain, or %NULL for the default ""
 * application domain
 * @log_level: the log level
 * @format: the message format. See the printf() documentation
 * @args: the parameters to insert into the format string
 *
 * Logs an error or debugging message.
 *
 * If the log level has been set as fatal, the abort()
 * function is called to terminate the program.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 */
void
g_logv (const gchar   *log_domain,
	GLogLevelFlags log_level,
	const gchar   *format,
	va_list	       args)
{
  gboolean was_fatal = (log_level & G_LOG_FLAG_FATAL) != 0;
  gboolean was_recursion = (log_level & G_LOG_FLAG_RECURSION) != 0;
  gchar buffer[1025], *msg, *msg_alloc = NULL;
  gint i;

  log_level &= G_LOG_LEVEL_MASK;
  if (!log_level)
    return;

  if (log_level & G_LOG_FLAG_RECURSION)
    {
      /* we use a stack buffer of fixed size, since we're likely
       * in an out-of-memory situation
       */
      gsize size G_GNUC_UNUSED;

      size = _g_vsnprintf (buffer, 1024, format, args);
      msg = buffer;
    }
  else
    msg = msg_alloc = g_strdup_vprintf (format, args);

  if (expected_messages)
    {
      GTestExpectedMessage *expected = expected_messages->data;

      if (g_strcmp0 (expected->log_domain, log_domain) == 0 &&
          ((log_level & expected->log_level) == expected->log_level) &&
          g_pattern_match_simple (expected->pattern, msg))
        {
          expected_messages = g_slist_delete_link (expected_messages,
                                                   expected_messages);
          g_free (expected->log_domain);
          g_free (expected->pattern);
          g_free (expected);
          g_free (msg_alloc);
          return;
        }
      else if ((log_level & G_LOG_LEVEL_DEBUG) != G_LOG_LEVEL_DEBUG)
        {
          gchar level_prefix[STRING_BUFFER_SIZE];
          gchar *expected_message;

          mklevel_prefix (level_prefix, expected->log_level, FALSE);
          expected_message = g_strdup_printf ("Did not see expected message %s-%s: %s",
                                              expected->log_domain ? expected->log_domain : "**",
                                              level_prefix, expected->pattern);
          g_log_default_handler (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, expected_message, NULL);
          g_free (expected_message);

          log_level |= G_LOG_FLAG_FATAL;
        }
    }

  for (i = g_bit_nth_msf (log_level, -1); i >= 0; i = g_bit_nth_msf (log_level, i))
    {
      GLogLevelFlags test_level;

      test_level = 1 << i;
      if (log_level & test_level)
	{
	  GLogDomain *domain;
	  GLogFunc log_func;
	  GLogLevelFlags domain_fatal_mask;
	  gpointer data = NULL;
          gboolean masquerade_fatal = FALSE;
          guint depth;

	  if (was_fatal)
	    test_level |= G_LOG_FLAG_FATAL;
	  if (was_recursion)
	    test_level |= G_LOG_FLAG_RECURSION;

	  /* check recursion and lookup handler */
	  g_mutex_lock (&g_messages_lock);
          depth = GPOINTER_TO_UINT (g_private_get (&g_log_depth));
	  domain = g_log_find_domain_L (log_domain ? log_domain : "");
	  if (depth)
	    test_level |= G_LOG_FLAG_RECURSION;
	  depth++;
	  domain_fatal_mask = domain ? domain->fatal_mask : G_LOG_FATAL_MASK;
	  if ((domain_fatal_mask | g_log_always_fatal) & test_level)
	    test_level |= G_LOG_FLAG_FATAL;
	  if (test_level & G_LOG_FLAG_RECURSION)
	    log_func = _g_log_fallback_handler;
	  else
	    log_func = g_log_domain_get_handler_L (domain, test_level, &data);
	  domain = NULL;
	  g_mutex_unlock (&g_messages_lock);

	  g_private_set (&g_log_depth, GUINT_TO_POINTER (depth));

          log_func (log_domain, test_level, msg, data);

          if ((test_level & G_LOG_FLAG_FATAL)
              && !(test_level & G_LOG_LEVEL_ERROR))
            {
              masquerade_fatal = fatal_log_func
                && !fatal_log_func (log_domain, test_level, msg, fatal_log_data);
            }

          if ((test_level & G_LOG_FLAG_FATAL) && !masquerade_fatal)
            {
#ifdef G_OS_WIN32
              if (win32_keep_fatal_message)
                {
                  gchar *locale_msg = g_locale_from_utf8 (fatal_msg_buf, -1, NULL, NULL, NULL);

                  MessageBox (NULL, locale_msg, NULL,
                              MB_ICONERROR|MB_SETFOREGROUND);
                }
#endif /* !G_OS_WIN32 */

              _g_log_abort (!(test_level & G_LOG_FLAG_RECURSION));
	    }
	  
	  depth--;
	  g_private_set (&g_log_depth, GUINT_TO_POINTER (depth));
	}
    }

  g_free (msg_alloc);
}

/**
 * g_log:
 * @log_domain: (nullable): the log domain, usually #G_LOG_DOMAIN, or %NULL
 * for the default
 * @log_level: the log level, either from #GLogLevelFlags
 *     or a user-defined level
 * @format: the message format. See the printf() documentation
 * @...: the parameters to insert into the format string
 *
 * Logs an error or debugging message.
 *
 * If the log level has been set as fatal, the abort()
 * function is called to terminate the program.
 *
 * If g_log_default_handler() is used as the log handler function, a new-line
 * character will automatically be appended to @..., and need not be entered
 * manually.
 */
void
g_log (const gchar   *log_domain,
       GLogLevelFlags log_level,
       const gchar   *format,
       ...)
{
  va_list args;
  
  va_start (args, format);
  g_logv (log_domain, log_level, format, args);
  va_end (args);
}

/* Return value must be 1 byte long (plus nul byte).
 * Reference: http://man7.org/linux/man-pages/man3/syslog.3.html#DESCRIPTION
 */
static const gchar *
log_level_to_priority (GLogLevelFlags log_level)
{
  if (log_level & G_LOG_LEVEL_ERROR)
    return "3";
  else if (log_level & G_LOG_LEVEL_CRITICAL)
    return "4";
  else if (log_level & G_LOG_LEVEL_WARNING)
    return "4";
  else if (log_level & G_LOG_LEVEL_MESSAGE)
    return "5";
  else if (log_level & G_LOG_LEVEL_INFO)
    return "6";
  else if (log_level & G_LOG_LEVEL_DEBUG)
    return "7";

  /* Default to LOG_NOTICE for custom log levels. */
  return "5";
}

static FILE *
log_level_to_file (GLogLevelFlags log_level)
{
  if (log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL |
                   G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE))
    return stderr;
  else
    return stdout;
}

static const gchar *
log_level_to_color (GLogLevelFlags log_level,
                    gboolean       use_color)
{
  /* we may not call _any_ GLib functions here */

  if (!use_color)
    return "";

  if (log_level & G_LOG_LEVEL_ERROR)
    return "\033[1;31m";
  else if (log_level & G_LOG_LEVEL_CRITICAL)
    return "\033[1;35m";
  else if (log_level & G_LOG_LEVEL_WARNING)
    return "\033[1;33m";
  else if (log_level & G_LOG_LEVEL_MESSAGE)
    return "\033[1;32m";
  else if (log_level & G_LOG_LEVEL_INFO)
    return "\033[1;32m";
  else if (log_level & G_LOG_LEVEL_DEBUG)
    return "\033[1;32m";

  /* No color for custom log levels. */
  return "";
}

static const gchar *
color_reset (gboolean use_color)
{
  /* we may not call _any_ GLib functions here */

  if (!use_color)
    return "";

  return "\033[0m";
}

/**
 * g_log_structured:
 * @log_domain: log domain, usually %G_LOG_DOMAIN
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @...: key-value pairs of structured data to add to the log entry, followed
 *    by the key "MESSAGE", followed by a printf()-style message format,
 *    followed by parameters to insert in the format string
 *
 * Log a message with structured data. The message will be passed through to
 * the log writer set by the application using g_log_set_writer_func(). If the
 * message is fatal (i.e. its log level is %G_LOG_LEVEL_ERROR), the program will
 * be aborted at the end of this function.
 *
 * The structured data is provided as key每value pairs, where keys are UTF-8
 * strings, and values are arbitrary pointers 〞 typically pointing to UTF-8
 * strings, but that is not a requirement. To pass binary (non-nul-terminated)
 * structured data, use g_log_structured_array(). The keys for structured data
 * should follow the [systemd journal
 * fields](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html)
 * specification. It is suggested that custom keys are namespaced according to
 * the code which sets them. For example, custom keys from GLib all have a
 * `GLIB_` prefix.
 *
 * The @log_domain will be converted into a `GLIB_DOMAIN` field. @log_level will
 * be converted into a
 * [`PRIORITY`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#PRIORITY=)
 * field. The format string will have its placeholders substituted for the provided
 * values and be converted into a
 * [`MESSAGE`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#MESSAGE=)
 * field.
 *
 * Other fields you may commonly want to pass into this function:
 *
 *  * [`MESSAGE_ID`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#MESSAGE_ID=)
 *  * [`CODE_FILE`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#CODE_FILE=)
 *  * [`CODE_LINE`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#CODE_LINE=)
 *  * [`CODE_FUNC`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#CODE_FUNC=)
 *  * [`ERRNO`](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#ERRNO=)
 *
 * Note that `CODE_FILE`, `CODE_LINE` and `CODE_FUNC` are automatically set by
 * the logging macros, G_DEBUG_HERE(), g_message(), g_warning(), g_critical(),
 * g_error(), etc, if the symbols `G_LOG_USE_STRUCTURED` is defined before including
 * glib.h.
 *
 * For example:
 * |[<!-- language="C" -->
 * g_log_structured (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
 *                   "MESSAGE_ID", "06d4df59e6c24647bfe69d2c27ef0b4e",
 *                   "MY_APPLICATION_CUSTOM_FIELD", "some debug string",
 *                   "MESSAGE", "This is a debug message about pointer %p and integer %u.",
 *                   some_pointer, some_integer);
 * ]|
 *
 * Note that each `MESSAGE_ID` must be [uniquely and randomly
 * generated](https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html#MESSAGE_ID=).
 * If adding a `MESSAGE_ID`, consider shipping a [message
 * catalog](https://www.freedesktop.org/wiki/Software/systemd/catalog/) with
 * your software.
 *
 * To pass a user data pointer to the log writer function which is specific to
 * this logging call, you must use g_log_structured_array() and pass the pointer
 * as a field with #GLogField.length set to zero, otherwise it will be
 * interpreted as a string.
 *
 * For example:
 * |[<!-- language="C" -->
 * const GLogField fields[] = {
 *   { "MESSAGE", "This is a debug message.", -1 },
 *   { "MESSAGE_ID", "fcfb2e1e65c3494386b74878f1abf893", -1 },
 *   { "MY_APPLICATION_CUSTOM_FIELD", "some debug string", -1 },
 *   { "MY_APPLICATION_STATE", state_object, 0 },
 * };
 * g_log_structured_array (G_LOG_LEVEL_DEBUG, fields, G_N_ELEMENTS (fields));
 * ]|
 *
 * Note also that, even if no structured fields are specified, the key-value
 * part of the argument list must be %NULL-terminated.
 *
 * The default writer function for `stdout` and `stderr` will automatically
 * append a new-line character after the message, so you should not add one
 * manually to the format string.
 *
 * Since: 2.50
 */
void
g_log_structured (const gchar    *log_domain,
                  GLogLevelFlags  log_level,
                  ...)
{
  va_list args;
  gchar buffer[1025], *message_allocated = NULL;
  const char *format;
  const gchar *message;
  gpointer p;
  gsize n_fields, i;
  GLogField stack_fields[16];
  GLogField *fields = stack_fields;
  GLogField *fields_allocated = NULL;
  GArray *array = NULL;

  va_start (args, log_level);

  for (p = va_arg (args, gchar *), i = 3;
       strcmp (p, "MESSAGE") != 0;
       p = va_arg (args, gchar *), i++)
    {
      GLogField field;
      const gchar *key = p;
      gconstpointer value = va_arg (args, gpointer);

      field.key = key;
      field.value = value;
      field.length = -1;

      if (i < 16)
        stack_fields[i] = field;
      else
        {
          /* Don't allow dynamic allocation, since we're likely
           * in an out-of-memory situation. For lack of a better solution,
           * just ignore further key-value pairs.
           */
          if (log_level & G_LOG_FLAG_RECURSION)
            continue;

          if (i == 16)
            {
              array = g_array_sized_new (FALSE, FALSE, sizeof (GLogField), 32);
              g_array_append_vals (array, stack_fields, 16);
            }

          g_array_append_val (array, field);
        }
    }

  n_fields = i;

  if (array)
    fields = fields_allocated = (GLogField *) g_array_free (array, FALSE);

  format = va_arg (args, gchar *);

  if (log_level & G_LOG_FLAG_RECURSION)
    {
      /* we use a stack buffer of fixed size, since we're likely
       * in an out-of-memory situation
       */
      gsize size G_GNUC_UNUSED;

      size = _g_vsnprintf (buffer, sizeof (buffer), format, args);
      message = buffer;
    }
  else
    {
      message = message_allocated = g_strdup_vprintf (format, args);
    }

  /* Add MESSAGE, PRIORITY and GLIB_DOMAIN. */
  fields[0].key = "MESSAGE";
  fields[0].value = message;
  fields[0].length = -1;

  fields[1].key = "PRIORITY";
  fields[1].value = log_level_to_priority (log_level);
  fields[1].length = 1;

  fields[2].key = "GLIB_DOMAIN";
  fields[2].value = log_domain;
  fields[2].length = -1;

  /* Log it. */
  g_log_structured_array (log_level, fields, n_fields);

  g_free (fields_allocated);
  g_free (message_allocated);

  va_end (args);
}

static GLogWriterOutput _g_log_writer_fallback (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gpointer         user_data);

/**
 * g_log_structured_array:
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @fields: (array length=n_fields): key每value pairs of structured data to add
 *    to the log message
 * @n_fields: number of elements in the @fields array
 *
 * Log a message with structured data. The message will be passed through to the
 * log writer set by the application using g_log_set_writer_func(). If the
 * message is fatal (i.e. its log level is %G_LOG_LEVEL_ERROR), the program will
 * be aborted at the end of this function.
 *
 * See g_log_structured() for more documentation.
 *
 * This assumes that @log_level is already present in @fields (typically as the
 * `PRIORITY` field).
 *
 * Since: 2.50
 */
void
g_log_structured_array (GLogLevelFlags   log_level,
                        const GLogField *fields,
                        gsize            n_fields)
{
  GLogWriterFunc writer_func;
  gpointer writer_user_data;
  gboolean recursion;
  guint depth;

  if (n_fields == 0)
    return;

  /* Check for recursion and look up the writer function. */
  depth = GPOINTER_TO_UINT (g_private_get (&g_log_structured_depth));
  recursion = (depth > 0);

  g_mutex_lock (&g_messages_lock);

  writer_func = recursion ? _g_log_writer_fallback : log_writer_func;
  writer_user_data = log_writer_user_data;

  g_mutex_unlock (&g_messages_lock);

  /* Write the log entry. */
  g_private_set (&g_log_structured_depth, GUINT_TO_POINTER (++depth));

  g_assert (writer_func != NULL);
  writer_func (log_level, fields, n_fields, writer_user_data);

  g_private_set (&g_log_structured_depth, GUINT_TO_POINTER (--depth));

  /* Abort if the message was fatal. */
  if (log_level & G_LOG_FATAL_MASK)
    _g_log_abort (!(log_level & G_LOG_FLAG_RECURSION));
}

/**
 * g_log_set_writer_func:
 * @func: log writer function, which must not be %NULL
 * @user_data: (closure func): user data to pass to @func
 * @user_data_free: (destroy func): function to free @user_data once it＊s
 *    finished with, if non-%NULL
 *
 * Set a writer function which will be called to format and write out each log
 * message. Each program should set a writer function, or the default writer
 * (g_log_writer_default()) will be used.
 *
 * Libraries **must not** call this function 〞 only programs are allowed to
 * install a writer function, as there must be a single, central point where
 * log messages are formatted and outputted.
 *
 * There can only be one writer function. It is an error to set more than one.
 *
 * Since: 2.50
 */
void
g_log_set_writer_func (GLogWriterFunc func,
                       gpointer       user_data,
                       GDestroyNotify user_data_free)
{
  g_return_if_fail (func != NULL);
  g_return_if_fail (log_writer_func == g_log_writer_default);

  g_mutex_lock (&g_messages_lock);
  log_writer_func = func;
  log_writer_user_data = user_data;
  log_writer_user_data_free = user_data_free;
  g_mutex_unlock (&g_messages_lock);
}

/**
 * g_log_writer_supports_color:
 * @output_fd: output file descriptor to check
 *
 * Check whether the given @output_fd file descriptor supports ANSI color
 * escape sequences. If so, they can safely be used when formatting log
 * messages.
 *
 * Returns: %TRUE if ANSI color escapes are supported, %FALSE otherwise
 * Since: 2.50
 */
gboolean
g_log_writer_supports_color (gint output_fd)
{
  g_return_val_if_fail (output_fd >= 0, FALSE);

  /* FIXME: This check could easily be expanded in future to be more robust
   * against different types of terminal, which still vary in their color
   * support. cmd.exe on Windows, for example, does not support ANSI colors;
   * but bash on Windows does.
   *
   * On UNIX systems, we probably want to use the functions from terminfo to
   * work out whether colors are supported.
   *
   * Some examples:
   *  - https://github.com/chalk/supports-color/blob/9434c93918301a6b47faa01999482adfbf1b715c/index.js#L61
   *  - http://stackoverflow.com/questions/16755142/how-to-make-win32-console-recognize-ansi-vt100-escape-sequences
   *  - http://blog.mmediasys.com/2010/11/24/we-all-love-colors/
   *  - http://unix.stackexchange.com/questions/198794/where-does-the-term-environment-variable-default-get-set
   */
  return isatty (output_fd);
}

/**
 * g_log_writer_is_journald:
 * @output_fd: output file descriptor to check
 *
 * Check whether the given @output_fd file descriptor is a connection to the
 * systemd journal, or something else (like a log file or `stdout` or
 * `stderr`).
 *
 * Returns: %TRUE if @output_fd points to the journal, %FALSE otherwise
 * Since: 2.50
 */
gboolean
g_log_writer_is_journald (gint output_fd)
{
#ifdef HAVE_LIBSYSTEMD
  /* FIXME: Use the new journal API for detecting whether we＊re writing to the
   * journal. See: https://github.com/systemd/systemd/issues/2473
   */
  static gsize initialized;
  static gboolean fd_is_journal = FALSE;

  g_return_val_if_fail (output_fd >= 0, FALSE);

  if (g_once_init_enter (&initialized))
    {
      struct sockaddr_storage addr;
      socklen_t addr_len = sizeof(addr);
      int err = getpeername (output_fd, (struct sockaddr *) &addr, &addr_len);
      if (err == 0 && addr.ss_family == AF_UNIX)
        fd_is_journal = g_str_has_prefix (((struct sockaddr_un *)&addr)->sun_path,
                                          "/run/systemd/journal/");
      g_once_init_leave (&initialized, TRUE);
    }

  return fd_is_journal;
#else /* if !HAVE_LIBSYSTEMD */
  return FALSE;
#endif
}

static void escape_string (GString *string);

/**
 * g_log_writer_format_fields:
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @fields: (array length=n_fields): key每value pairs of structured data forming
 *    the log message
 * @n_fields: number of elements in the @fields array
 * @use_color: %TRUE to use ANSI color escape sequences when formatting the
 *    message, %FALSE to not
 *
 * Format a structured log message as a string suitable for outputting to the
 * terminal (or elsewhere). This will include the values of all fields it knows
 * how to interpret, which includes `MESSAGE` and `GLIB_DOMAIN` (see the
 * documentation for g_log_structured()). It does not include values from
 * unknown fields.
 *
 * The returned string does **not** have a trailing new-line character. It is
 * encoded in the character set of the current locale, which is not necessarily
 * UTF-8.
 *
 * Returns: (transfer full): string containing the formatted log message, in
 *    the character set of the current locale
 * Since: 2.50
 */
gchar *
g_log_writer_format_fields (GLogLevelFlags   log_level,
                            const GLogField *fields,
                            gsize            n_fields,
                            gboolean         use_color)
{
  gsize i;
  const gchar *message = NULL;
  const gchar *log_domain = NULL;
  gchar level_prefix[STRING_BUFFER_SIZE];
  GString *gstring;

  /* Extract some common fields. */
  for (i = 0; (message == NULL || log_domain == NULL) && i < n_fields; i++)
    {
      const GLogField *field = &fields[i];

      if (g_strcmp0 (field->key, "MESSAGE") == 0)
        message = field->value;
      else if (g_strcmp0 (field->key, "GLIB_DOMAIN") == 0)
        log_domain = field->value;
    }

  /* Format things. */
  mklevel_prefix (level_prefix, log_level, use_color);

  gstring = g_string_new (NULL);
  if (log_level & ALERT_LEVELS)
    g_string_append (gstring, "\n");
  if (!log_domain)
    g_string_append (gstring, "** ");

  if ((g_log_msg_prefix & (log_level & G_LOG_LEVEL_MASK)) ==
      (log_level & G_LOG_LEVEL_MASK))
    {
      const gchar *prg_name = g_get_prgname ();
      gulong pid = getpid ();

      if (prg_name == NULL)
        g_string_append_printf (gstring, "(process:%lu): ", pid);
      else
        g_string_append_printf (gstring, "(%s:%lu): ", prg_name, pid);
    }

  if (log_domain != NULL)
    {
      g_string_append (gstring, log_domain);
      g_string_append_c (gstring, '-');
    }
  g_string_append (gstring, level_prefix);

  g_string_append (gstring, ": ");
  if (message == NULL)
    {
      g_string_append (gstring, "(NULL) message");
    }
  else
    {
      GString *msg;
      const gchar *charset;

      msg = g_string_new (message);
      escape_string (msg);

      if (g_get_charset (&charset))
        {
          /* charset is UTF-8 already */
          g_string_append (gstring, msg->str);
        }
      else
        {
          gchar *lstring = strdup_convert (msg->str, charset);
          g_string_append (gstring, lstring);
          g_free (lstring);
        }

      g_string_free (msg, TRUE);
    }

  return g_string_free (gstring, FALSE);
}

/**
 * g_log_writer_journald:
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @fields: (array length=n_fields): key每value pairs of structured data forming
 *    the log message
 * @n_fields: number of elements in the @fields array
 * @user_data: user data passed to g_log_set_writer_func()
 *
 * Format a structured log message and send it to the systemd journal as a set
 * of key每value pairs. All fields are sent to the journal, but if a field has
 * length zero (indicating program-specific data) then only its key will be
 * sent.
 *
 * This is suitable for use as a #GLogWriterFunc.
 *
 * If GLib has been compiled without systemd support, this function is still
 * defined, but will always return %G_LOG_WRITER_UNHANDLED.
 *
 * Returns: %G_LOG_WRITER_HANDLED on success, %G_LOG_WRITER_UNHANDLED otherwise
 * Since: 2.50
 */
GLogWriterOutput
g_log_writer_journald (GLogLevelFlags   log_level,
                       const GLogField *fields,
                       gsize            n_fields,
                       gpointer         user_data)
{
#ifdef HAVE_LIBSYSTEMD
  gsize i;
  struct iovec *pairs;
  gint retval;

  g_return_val_if_fail (fields != NULL, G_LOG_WRITER_UNHANDLED);
  g_return_val_if_fail (n_fields > 0, G_LOG_WRITER_UNHANDLED);

  /* According to systemd.journal-fields(7), the journal allows fields in any
   * format (including arbitrary binary), but expects text fields to be UTF-8.
   * This is great, because we require input strings to be in UTF-8, so no
   * conversion is necessary and we don＊t need to care about the current
   * locale＊s character set.
   */

  pairs = g_alloca (sizeof (struct iovec) * n_fields);

  for (i = 0; i < n_fields; i++)
    {
      guint8 *buf = NULL;
      gsize key_length;
      gsize value_length;

      /* Build the iovec for this field. */
      key_length = strlen (fields[i].key);
      value_length =
          (fields[i].length < 0) ? strlen (fields[i].value) : fields[i].length;

      buf = g_malloc (key_length + 1 + value_length + 1);
      pairs[i].iov_base = buf;
      pairs[i].iov_len = key_length + 1 + value_length;

      strncpy ((char *) buf, fields[i].key, key_length);
      buf[key_length] = '=';
      memcpy ((char *) buf + key_length + 1, fields[i].value, value_length);
      buf[key_length + 1 + value_length] = '\0';
    }

  retval = sd_journal_sendv (pairs, n_fields);

  for (i = 0; i < n_fields; i++)
    g_free (pairs[i].iov_base);

  return (retval == 0) ? G_LOG_WRITER_HANDLED : G_LOG_WRITER_UNHANDLED;
#else /* if !HAVE_LIBSYSTEMD */
  return G_LOG_WRITER_UNHANDLED;
#endif
}

/**
 * g_log_writer_standard_streams:
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @fields: (array length=n_fields): key每value pairs of structured data forming
 *    the log message
 * @n_fields: number of elements in the @fields array
 * @user_data: user data passed to g_log_set_writer_func()
 *
 * Format a structured log message and print it to either `stdout` or `stderr`,
 * depending on its log level. %G_LOG_LEVEL_INFO and %G_LOG_LEVEL_DEBUG messages
 * are sent to `stdout`; all other log levels are sent to `stderr`. Only fields
 * which are understood by this function are included in the formatted string
 * which is printed.
 *
 * If the output stream supports ANSI color escape sequences, they will be used
 * in the output.
 *
 * A trailing new-line character is added to the log message when it is printed.
 *
 * This is suitable for use as a #GLogWriterFunc.
 *
 * Returns: %G_LOG_WRITER_HANDLED on success, %G_LOG_WRITER_UNHANDLED otherwise
 * Since: 2.50
 */
GLogWriterOutput
g_log_writer_standard_streams (GLogLevelFlags   log_level,
                               const GLogField *fields,
                               gsize            n_fields,
                               gpointer         user_data)
{
  FILE *stream;
  gchar *out = NULL;  /* in the current locale＊s character set */

  g_return_val_if_fail (fields != NULL, G_LOG_WRITER_UNHANDLED);
  g_return_val_if_fail (n_fields > 0, G_LOG_WRITER_UNHANDLED);

  stream = log_level_to_file (log_level);
  out = g_log_writer_format_fields (log_level, fields, n_fields,
                                    g_log_writer_supports_color (fileno (stream)));
  _g_fprintf (stream, "%s\n", out);
  g_free (out);

  return G_LOG_WRITER_HANDLED;
}

/* The old g_log() API is implemented in terms of the new structured log API.
 * However, some of the checks do not line up between the two APIs: the
 * structured API only handles fatalness of messages for log levels; the old API
 * handles it per-domain as well. Consequently, we need to disable fatalness
 * handling in the structured log API when called from the old g_log() API.
 *
 * We can guarantee that g_log_default_handler() will pass GLIB_OLD_LOG_API as
 * the first field to g_log_structured_array(), if that is the case.
 */
static gboolean
log_is_old_api (const GLogField *fields,
                gsize            n_fields)
{
  return (n_fields >= 1 &&
          g_strcmp0 (fields[0].key, "GLIB_OLD_LOG_API") == 0 &&
          g_strcmp0 (fields[0].value, "1") == 0);
}

/**
 * g_log_writer_default:
 * @log_level: log level, either from #GLogLevelFlags, or a user-defined
 *    level
 * @fields: (array length=n_fields): key每value pairs of structured data forming
 *    the log message
 * @n_fields: number of elements in the @fields array
 * @user_data: user data passed to g_log_set_writer_func()
 *
 * Format a structured log message and output it to the default log destination
 * for the platform. On Linux, this is typically the systemd journal, falling
 * back to `stdout` or `stderr` if running from the terminal or if output is
 * being redirected to a file.
 *
 * Support for other platform-specific logging mechanisms may be added in
 * future. Distributors of GLib may modify this function to impose their own
 * (documented) platform-specific log writing policies.
 *
 * This is suitable for use as a #GLogWriterFunc, and is the default writer used
 * if no other is set using g_log_set_writer_func().
 *
 * Returns: %G_LOG_WRITER_HANDLED on success, %G_LOG_WRITER_UNHANDLED otherwise
 * Since: 2.50
 */
GLogWriterOutput
g_log_writer_default (GLogLevelFlags   log_level,
                      const GLogField *fields,
                      gsize            n_fields,
                      gpointer         user_data)
{
  g_return_val_if_fail (fields != NULL, G_LOG_WRITER_UNHANDLED);
  g_return_val_if_fail (n_fields > 0, G_LOG_WRITER_UNHANDLED);

  /* Disable debug message output unless specified in G_MESSAGES_DEBUG. */
  if (!(log_level & DEFAULT_LEVELS) && !(log_level >> G_LOG_LEVEL_USER_SHIFT))
    {
      const gchar *domains, *log_domain = NULL;
      gsize i;

      domains = g_getenv ("G_MESSAGES_DEBUG");

      if ((log_level & INFO_LEVELS) == 0 ||
          domains == NULL)
        return G_LOG_WRITER_HANDLED;

      for (i = 0; i < n_fields; i++)
        {
          if (g_strcmp0 (fields[i].key, "GLIB_DOMAIN") == 0)
            {
              log_domain = fields[i].value;
              break;
            }
        }

      if (strcmp (domains, "all") != 0 &&
          (log_domain == NULL || !strstr (domains, log_domain)))
        return G_LOG_WRITER_HANDLED;
    }

  /* Mark messages as fatal if they have a level set in
   * g_log_set_always_fatal().
   */
  if ((log_level & g_log_always_fatal) && !log_is_old_api (fields, n_fields))
    log_level |= G_LOG_FLAG_FATAL;

  /* Try logging to the systemd journal as first choice. */
  if (g_log_writer_is_journald (fileno (stderr)) &&
      g_log_writer_journald (log_level, fields, n_fields, user_data) ==
      G_LOG_WRITER_HANDLED)
    goto handled;

  /* FIXME: Add support for the Windows log. */

  if (g_log_writer_standard_streams (log_level, fields, n_fields, user_data) ==
      G_LOG_WRITER_HANDLED)
    goto handled;

  return G_LOG_WRITER_UNHANDLED;

handled:
  /* Abort if the message was fatal. */
  if (log_level & G_LOG_FLAG_FATAL)
    {
#ifdef G_OS_WIN32
      if (!g_test_initialized ())
        {
          gchar *locale_msg = NULL;

          locale_msg = g_locale_from_utf8 (fatal_msg_buf, -1, NULL, NULL, NULL);
          MessageBox (NULL, locale_msg, NULL,
                      MB_ICONERROR | MB_SETFOREGROUND);
          g_free (locale_msg);
        }
#endif /* !G_OS_WIN32 */

      _g_log_abort (!(log_level & G_LOG_FLAG_RECURSION));
    }

  return G_LOG_WRITER_HANDLED;
}

static GLogWriterOutput
_g_log_writer_fallback (GLogLevelFlags   log_level,
                        const GLogField *fields,
                        gsize            n_fields,
                        gpointer         user_data)
{
  FILE *stream;
  gsize i;

  /* we cannot call _any_ GLib functions in this fallback handler,
   * which is why we skip UTF-8 conversion, etc.
   * since we either recursed or ran out of memory, we're in a pretty
   * pathologic situation anyways, what we can do is giving the
   * the process ID unconditionally however.
   */

  stream = log_level_to_file (log_level);

  for (i = 0; i < n_fields; i++)
    {
      const GLogField *field = &fields[i];

      /* Only print fields we definitely recognise, otherwise we could end up
       * printing a random non-string pointer provided by the user to be
       * interpreted by their writer function.
       */
      if (strcmp (field->key, "MESSAGE") != 0 &&
          strcmp (field->key, "MESSAGE_ID") != 0 &&
          strcmp (field->key, "PRIORITY") != 0 &&
          strcmp (field->key, "CODE_FILE") != 0 &&
          strcmp (field->key, "CODE_LINE") != 0 &&
          strcmp (field->key, "CODE_FUNC") != 0 &&
          strcmp (field->key, "ERRNO") != 0 &&
          strcmp (field->key, "SYSLOG_FACILITY") != 0 &&
          strcmp (field->key, "SYSLOG_IDENTIFIER") != 0 &&
          strcmp (field->key, "SYSLOG_PID") != 0 &&
          strcmp (field->key, "GLIB_DOMAIN") != 0)
        continue;

      write_string (stream, field->key);
      write_string (stream, "=");
      write_string_sized (stream, field->value, field->length);
    }

#ifndef G_OS_WIN32
  {
    gchar pid_string[FORMAT_UNSIGNED_BUFSIZE];

    format_unsigned (pid_string, getpid (), 10);
    write_string (stream, "_PID=");
    write_string (stream, pid_string);
  }
#endif

  return G_LOG_WRITER_HANDLED;
}

/**
 * g_return_if_fail_warning: (skip)
 * @log_domain: (nullable):
 * @pretty_function:
 * @expression: (nullable):
 */
void
g_return_if_fail_warning (const char *log_domain,
			  const char *pretty_function,
			  const char *expression)
{
  g_log (log_domain,
	 G_LOG_LEVEL_CRITICAL,
	 "%s: assertion '%s' failed",
	 pretty_function,
	 expression);
}

/**
 * g_warn_message: (skip)
 * @domain: (nullable):
 * @file:
 * @line:
 * @func:
 * @warnexpr: (nullable):
 */
void
g_warn_message (const char     *domain,
                const char     *file,
                int             line,
                const char     *func,
                const char     *warnexpr)
{
  char *s, lstr[32];
  g_snprintf (lstr, 32, "%d", line);
  if (warnexpr)
    s = g_strconcat ("(", file, ":", lstr, "):",
                     func, func[0] ? ":" : "",
                     " runtime check failed: (", warnexpr, ")", NULL);
  else
    s = g_strconcat ("(", file, ":", lstr, "):",
                     func, func[0] ? ":" : "",
                     " ", "code should not be reached", NULL);
  g_log (domain, G_LOG_LEVEL_WARNING, "%s", s);
  g_free (s);
}

void
g_assert_warning (const char *log_domain,
		  const char *file,
		  const int   line,
		  const char *pretty_function,
		  const char *expression)
{
  if (expression)
    g_log (log_domain,
	   G_LOG_LEVEL_ERROR,
	   "file %s: line %d (%s): assertion failed: (%s)",
	   file,
	   line,
	   pretty_function,
	   expression);
  else
    g_log (log_domain,
	   G_LOG_LEVEL_ERROR,
	   "file %s: line %d (%s): should not be reached",
	   file,
	   line,
	   pretty_function);
  _g_log_abort (FALSE);
  g_abort ();
}

/**
 * g_test_expect_message:
 * @log_domain: (allow-none): the log domain of the message
 * @log_level: the log level of the message
 * @pattern: a glob-style [pattern][glib-Glob-style-pattern-matching]
 *
 * Indicates that a message with the given @log_domain and @log_level,
 * with text matching @pattern, is expected to be logged. When this
 * message is logged, it will not be printed, and the test case will
 * not abort.
 *
 * Use g_test_assert_expected_messages() to assert that all
 * previously-expected messages have been seen and suppressed.
 *
 * You can call this multiple times in a row, if multiple messages are
 * expected as a result of a single call. (The messages must appear in
 * the same order as the calls to g_test_expect_message().)
 *
 * For example:
 *
 * |[<!-- language="C" --> 
 *   // g_main_context_push_thread_default() should fail if the
 *   // context is already owned by another thread.
 *   g_test_expect_message (G_LOG_DOMAIN,
 *                          G_LOG_LEVEL_CRITICAL,
 *                          "assertion*acquired_context*failed");
 *   g_main_context_push_thread_default (bad_context);
 *   g_test_assert_expected_messages ();
 * ]|
 *
 * Note that you cannot use this to test g_error() messages, since
 * g_error() intentionally never returns even if the program doesn't
 * abort; use g_test_trap_subprocess() in this case.
 *
 * If messages at %G_LOG_LEVEL_DEBUG are emitted, but not explicitly
 * expected via g_test_expect_message() then they will be ignored.
 *
 * Since: 2.34
 */
void
g_test_expect_message (const gchar    *log_domain,
                       GLogLevelFlags  log_level,
                       const gchar    *pattern)
{
  GTestExpectedMessage *expected;

  g_return_if_fail (log_level != 0);
  g_return_if_fail (pattern != NULL);
  g_return_if_fail (~log_level & G_LOG_LEVEL_ERROR);

  expected = g_new (GTestExpectedMessage, 1);
  expected->log_domain = g_strdup (log_domain);
  expected->log_level = log_level;
  expected->pattern = g_strdup (pattern);

  expected_messages = g_slist_append (expected_messages, expected);
}

void
g_test_assert_expected_messages_internal (const char     *domain,
                                          const char     *file,
                                          int             line,
                                          const char     *func)
{
  if (expected_messages)
    {
      GTestExpectedMessage *expected;
      gchar level_prefix[STRING_BUFFER_SIZE];
      gchar *message;

      expected = expected_messages->data;

      mklevel_prefix (level_prefix, expected->log_level, FALSE);
      message = g_strdup_printf ("Did not see expected message %s-%s: %s",
                                 expected->log_domain ? expected->log_domain : "**",
                                 level_prefix, expected->pattern);
      g_assertion_message (G_LOG_DOMAIN, file, line, func, message);
      g_free (message);
    }
}

/**
 * g_test_assert_expected_messages:
 *
 * Asserts that all messages previously indicated via
 * g_test_expect_message() have been seen and suppressed.
 *
 * If messages at %G_LOG_LEVEL_DEBUG are emitted, but not explicitly
 * expected via g_test_expect_message() then they will be ignored.
 *
 * Since: 2.34
 */

void
_g_log_fallback_handler (const gchar   *log_domain,
			 GLogLevelFlags log_level,
			 const gchar   *message,
			 gpointer       unused_data)
{
  gchar level_prefix[STRING_BUFFER_SIZE];
#ifndef G_OS_WIN32
  gchar pid_string[FORMAT_UNSIGNED_BUFSIZE];
#endif
  FILE *stream;

  /* we cannot call _any_ GLib functions in this fallback handler,
   * which is why we skip UTF-8 conversion, etc.
   * since we either recursed or ran out of memory, we're in a pretty
   * pathologic situation anyways, what we can do is giving the
   * the process ID unconditionally however.
   */

  stream = mklevel_prefix (level_prefix, log_level, FALSE);
  if (!message)
    message = "(NULL) message";

#ifndef G_OS_WIN32
  format_unsigned (pid_string, getpid (), 10);
#endif

  if (log_domain)
    write_string (stream, "\n");
  else
    write_string (stream, "\n** ");

#ifndef G_OS_WIN32
  write_string (stream, "(process:");
  write_string (stream, pid_string);
  write_string (stream, "): ");
#endif

  if (log_domain)
    {
      write_string (stream, log_domain);
      write_string (stream, "-");
    }
  write_string (stream, level_prefix);
  write_string (stream, ": ");
  write_string (stream, message);
}

static void
escape_string (GString *string)
{
  const char *p = string->str;
  gunichar wc;

  while (p < string->str + string->len)
    {
      gboolean safe;
	    
      wc = g_utf8_get_char_validated (p, -1);
      if (wc == (gunichar)-1 || wc == (gunichar)-2)  
	{
	  gchar *tmp;
	  guint pos;

	  pos = p - string->str;

	  /* Emit invalid UTF-8 as hex escapes 
           */
	  tmp = g_strdup_printf ("\\x%02x", (guint)(guchar)*p);
	  g_string_erase (string, pos, 1);
	  g_string_insert (string, pos, tmp);

	  p = string->str + (pos + 4); /* Skip over escape sequence */

	  g_free (tmp);
	  continue;
	}
      if (wc == '\r')
	{
	  safe = *(p + 1) == '\n';
	}
      else
	{
	  safe = CHAR_IS_SAFE (wc);
	}
      
      if (!safe)
	{
	  gchar *tmp;
	  guint pos;

	  pos = p - string->str;
	  
	  /* Largest char we escape is 0x0a, so we don't have to worry
	   * about 8-digit \Uxxxxyyyy
	   */
	  tmp = g_strdup_printf ("\\u%04x", wc); 
	  g_string_erase (string, pos, g_utf8_next_char (p) - p);
	  g_string_insert (string, pos, tmp);
	  g_free (tmp);

	  p = string->str + (pos + 6); /* Skip over escape sequence */
	}
      else
	p = g_utf8_next_char (p);
    }
}

/**
 * g_log_default_handler:
 * @log_domain: (nullable): the log domain of the message, or %NULL for the
 * default "" application domain
 * @log_level: the level of the message
 * @message: (nullable): the message
 * @unused_data: (nullable): data passed from g_log() which is unused
 *
 * The default log handler set up by GLib; g_log_set_default_handler()
 * allows to install an alternate default log handler.
 * This is used if no log handler has been set for the particular log
 * domain and log level combination. It outputs the message to stderr
 * or stdout and if the log level is fatal it calls abort(). It automatically
 * prints a new-line character after the message, so one does not need to be
 * manually included in @message.
 *
 * The behavior of this log handler can be influenced by a number of
 * environment variables:
 *
 * - `G_MESSAGES_PREFIXED`: A :-separated list of log levels for which
 *   messages should be prefixed by the program name and PID of the
 *   aplication.
 *
 * - `G_MESSAGES_DEBUG`: A space-separated list of log domains for
 *   which debug and informational messages are printed. By default
 *   these messages are not printed.
 *
 * stderr is used for levels %G_LOG_LEVEL_ERROR, %G_LOG_LEVEL_CRITICAL,
 * %G_LOG_LEVEL_WARNING and %G_LOG_LEVEL_MESSAGE. stdout is used for
 * the rest.
 */
void
g_log_default_handler (const gchar   *log_domain,
		       GLogLevelFlags log_level,
		       const gchar   *message,
		       gpointer	      unused_data)
{
  const gchar *domains;
  GLogField fields[4];

  if ((log_level & DEFAULT_LEVELS) || (log_level >> G_LOG_LEVEL_USER_SHIFT))
    goto emit;

  domains = g_getenv ("G_MESSAGES_DEBUG");
  if (((log_level & INFO_LEVELS) == 0) ||
      domains == NULL ||
      (strcmp (domains, "all") != 0 && (!log_domain || !strstr (domains, log_domain))))
    return;

 emit:
  /* we can be called externally with recursion for whatever reason */
  if (log_level & G_LOG_FLAG_RECURSION)
    {
      _g_log_fallback_handler (log_domain, log_level, message, unused_data);
      return;
    }

  fields[0].key = "GLIB_OLD_LOG_API";
  fields[0].value = "1";
  fields[0].length = -1;

  fields[1].key = "MESSAGE";
  fields[1].value = message;
  fields[1].length = -1;

  fields[2].key = "PRIORITY";
  fields[2].value = log_level_to_priority (log_level);
  fields[2].length = 1;

  fields[3].key = "GLIB_DOMAIN";
  fields[3].value = log_domain;
  fields[3].length = -1;

  /* Print out via the structured log API, but drop any fatal flags since we
   * have already handled them. The fatal handling in the structured logging
   * API is more coarse-grained than in the old g_log() API, so we don't want
   * to use it here.
   */
  g_log_structured_array (log_level & ~G_LOG_FLAG_FATAL, fields, 4);
}

/**
 * g_set_print_handler:
 * @func: the new print handler
 *
 * Sets the print handler.
 *
 * Any messages passed to g_print() will be output via
 * the new handler. The default handler simply outputs
 * the message to stdout. By providing your own handler
 * you can redirect the output, to a GTK+ widget or a
 * log file for example.
 *
 * Returns: the old print handler
 */
GPrintFunc
g_set_print_handler (GPrintFunc func)
{
  GPrintFunc old_print_func;

  g_mutex_lock (&g_messages_lock);
  old_print_func = glib_print_func;
  glib_print_func = func;
  g_mutex_unlock (&g_messages_lock);

  return old_print_func;
}

/**
 * g_print:
 * @format: the message format. See the printf() documentation
 * @...: the parameters to insert into the format string
 *
 * Outputs a formatted message via the print handler.
 * The default print handler simply outputs the message to stdout, without
 * appending a trailing new-line character. Typically, @format should end with
 * its own new-line character.
 *
 * g_print() should not be used from within libraries for debugging
 * messages, since it may be redirected by applications to special
 * purpose message windows or even files. Instead, libraries should
 * use g_log(), or the convenience functions g_message(), g_warning()
 * and g_error().
 */
void
g_print (const gchar *format,
         ...)
{
  va_list args;
  gchar *string;
  GPrintFunc local_glib_print_func;

  g_return_if_fail (format != NULL);

  va_start (args, format);
  string = g_strdup_vprintf (format, args);
  va_end (args);

  g_mutex_lock (&g_messages_lock);
  local_glib_print_func = glib_print_func;
  g_mutex_unlock (&g_messages_lock);

  if (local_glib_print_func)
    local_glib_print_func (string);
  else
    {
      const gchar *charset;

      if (g_get_charset (&charset))
        fputs (string, stdout); /* charset is UTF-8 already */
      else
        {
          gchar *lstring = strdup_convert (string, charset);

          fputs (lstring, stdout);
          g_free (lstring);
        }
      fflush (stdout);
    }
  g_free (string);
}

/**
 * g_set_printerr_handler:
 * @func: the new error message handler
 *
 * Sets the handler for printing error messages.
 *
 * Any messages passed to g_printerr() will be output via
 * the new handler. The default handler simply outputs the
 * message to stderr. By providing your own handler you can
 * redirect the output, to a GTK+ widget or a log file for
 * example.
 *
 * Returns: the old error message handler
 */
GPrintFunc
g_set_printerr_handler (GPrintFunc func)
{
  GPrintFunc old_printerr_func;

  g_mutex_lock (&g_messages_lock);
  old_printerr_func = glib_printerr_func;
  glib_printerr_func = func;
  g_mutex_unlock (&g_messages_lock);

  return old_printerr_func;
}

/**
 * g_printerr:
 * @format: the message format. See the printf() documentation
 * @...: the parameters to insert into the format string
 *
 * Outputs a formatted message via the error message handler.
 * The default handler simply outputs the message to stderr, without appending
 * a trailing new-line character. Typically, @format should end with its own
 * new-line character.
 *
 * g_printerr() should not be used from within libraries.
 * Instead g_log() should be used, or the convenience functions
 * g_message(), g_warning() and g_error().
 */
void
g_printerr (const gchar *format,
            ...)
{
  va_list args;
  gchar *string;
  GPrintFunc local_glib_printerr_func;

  g_return_if_fail (format != NULL);

  va_start (args, format);
  string = g_strdup_vprintf (format, args);
  va_end (args);

  g_mutex_lock (&g_messages_lock);
  local_glib_printerr_func = glib_printerr_func;
  g_mutex_unlock (&g_messages_lock);

  if (local_glib_printerr_func)
    local_glib_printerr_func (string);
  else
    {
      const gchar *charset;

      if (g_get_charset (&charset))
        fputs (string, stderr); /* charset is UTF-8 already */
      else
        {
          gchar *lstring = strdup_convert (string, charset);

          fputs (lstring, stderr);
          g_free (lstring);
        }
      fflush (stderr);
    }
  g_free (string);
}

/**
 * g_printf_string_upper_bound:
 * @format: the format string. See the printf() documentation
 * @args: the parameters to be inserted into the format string
 *
 * Calculates the maximum space needed to store the output
 * of the sprintf() function.
 *
 * Returns: the maximum space needed to store the formatted string
 */
gsize
g_printf_string_upper_bound (const gchar *format,
                             va_list      args)
{
  gchar c;
  return _g_vsnprintf (&c, 1, format, args) + 1;
}
