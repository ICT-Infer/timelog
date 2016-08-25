--
-- Copyright (c) 2015, 2016 Erik Nordstrøm <erikn@ict-infer.no>
--
-- Permission to use, copy, modify, and distribute this software for any
-- purpose with or without fee is hereby granted, provided that the above
-- copyright notice and this permission notice appear in all copies.
--
-- THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
-- WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
-- MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
-- ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
-- WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
-- ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
-- OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
--

BEGIN;

-- The passwd_shim table is for integration with host OS user account system.
-- To make migration between hosts simpler, pw_uid should be "semi-private".
-- For this reason, we use pw_name as the primary key.
CREATE TABLE passwd_shim (
  -- Unless you have a good, specific reason to need more than eight characters
  -- for usernames, keep this value as it is since there are other systems
  -- out there to this day where exceeding 8 characters in the username
  -- might either lead to trouble or even not be allowed, e.g. NIS(?)
  -- Besides, eight characters saves typing for your users ;)
  pw_name varchar(8) PRIMARY KEY,
  pw_uid  integer NOT NULL
);

CREATE TABLE categories (
  parent_id integer,

  id      serial PRIMARY KEY,
  name    varchar(255) NOT NULL,
  comment varchar(255),
  slug    char(32) NOT NULL,

  FOREIGN KEY (parent_id) REFERENCES categories  
);

CREATE TABLE entries (
  pw_name varchar(8) NOT NULL,
  catid   integer NOT NULL,

  id       serial PRIMARY KEY,
  t_begin  timestamp with time zone NOT NULL,
  tz_begin varchar(255) NOT NULL,
  t_end    timestamp with time zone,
  tz_end   varchar(255),
  comment  varchar(255),

  FOREIGN KEY (pw_name) REFERENCES passwd_shim,
  FOREIGN KEY (catid)   REFERENCES categories
);

COMMIT;
