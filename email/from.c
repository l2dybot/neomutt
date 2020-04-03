/**
 * @file
 * Determine who the email is from
 *
 * @authors
 * Copyright (C) 1996-2000,2013 Michael R. Elkins <me@mutt.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page email_from Determine who the email is from
 *
 * Determine who the email is from
 */

#include "config.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mutt/lib.h"
#include "from.h"

/**
 * is_from - Is a string a 'From' header line?
 * @param[in]  s       String to test
 * @param[out] path    Buffer for extracted path
 * @param[in]  pathlen Length of buffer
 * @param[out] tp      Extracted time
 * @retval 1 Yes, it is
 * @retval 0 No, it isn't
 *
 * A valid message separator looks like:
 * `From <return-path> <weekday> <month> <day> <time> <year>`
 */
bool is_from(const char *s, char *path, size_t pathlen, time_t *tp)
{
  const regmatch_t *match = mutt_prex_capture(PREX_MBOX_FROM, s);
  if (!match)
    return false;

  if (path)
  {
    const regmatch_t *msender = &match[PREX_MBOX_FROM_MATCH_ENVSENDER];
    const size_t dsize = MIN(pathlen, mutt_regmatch_len(msender));
    mutt_str_strfcpy(path, s + mutt_regmatch_start(msender), dsize);
  }

  if (tp)
  {
    const regmatch_t *mmonth = &match[PREX_MBOX_FROM_MATCH_MONTH];
    const regmatch_t *mday = &match[PREX_MBOX_FROM_MATCH_DAY];
    const regmatch_t *mtime = &match[PREX_MBOX_FROM_MATCH_TIME];
    const regmatch_t *myear = &match[PREX_MBOX_FROM_MATCH_YEAR];

    struct tm tm = { 0 };
    tm.tm_isdst = -1;
    tm.tm_mon = mutt_date_check_month(s + mutt_regmatch_start(mmonth));
    sscanf(s + mutt_regmatch_start(mday), " %d", &tm.tm_mday);
    sscanf(s + mutt_regmatch_start(mtime), "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    sscanf(s + mutt_regmatch_start(myear), "%d", &tm.tm_year);
    tm.tm_year -= 1900;
    *tp = mutt_date_make_time(&tm, false);
  }

  return true;
}
