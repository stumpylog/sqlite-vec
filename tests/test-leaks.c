// Leak and use-after-free checks for vec0 error paths and statements that
// allocate SQL strings. Build with AddressSanitizer (LeakSanitizer is part of
// it on Linux): `make test-leaks CC=clang EXTRA_CFLAGS=-fsanitize=address`.
// Without a sanitizer it only checks that the statements behave. A leak makes
// the process exit non-zero when it ends, so every scenario runs in this one
// process.
#include <stdio.h>
#include <string.h>

#include "sqlite3.h"

extern int sqlite3_vec_init(sqlite3 *, char **, const sqlite3_api_routines *);

static sqlite3 *db;
static int failures = 0;

static void run(const char *sql, int expect_ok) {
  char *err = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
  if ((rc == SQLITE_OK) != expect_ok) {
    fprintf(stderr, "FAIL: rc=%d for `%s` (%s)\n", rc, sql, err ? err : "");
    failures++;
  }
  sqlite3_free(err);
}

static void begin(const char *name) {
  fprintf(stderr, "scenario: %s\n", name);
  sqlite3_open(":memory:", &db);
}

static void end(void) {
  int rc = sqlite3_close(db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "FAIL: sqlite3_close returned %d\n", rc);
    failures++;
  }
}

int main(void) {
  sqlite3_auto_extension((void (*)(void))sqlite3_vec_init);

  // every UPDATE of an auxiliary column built a SQL string and never freed it,
  // and every INSERT did the same
  begin("auxiliary column insert and update");
  run("create virtual table v using vec0(x float[2], +a text)", 1);
  run("insert into v(rowid, x, a) values (1, '[1,2]', 'a')", 1);
  for (int i = 0; i < 50; i++) {
    run("update v set a = 'b' where rowid = 1", 1);
  }
  end();

  // text metadata longer than 12 bytes goes through its own INSERT, UPDATE
  // and DELETE statements; the shadow table names of metadata columns were
  // never freed when the table was
  begin("long text metadata insert, update and delete");
  run("create virtual table v using vec0(x float[2], m text)", 1);
  run("insert into v(rowid, x, m) values (1, '[1,2]', 'a long text value over twelve bytes')", 1);
  run("update v set m = 'another long text value over twelve' where rowid = 1", 1);
  run("update v set m = 'short' where rowid = 1", 1);
  run("delete from v where rowid = 1", 1);
  run("insert into v(rowid, x, m) values (2, '[1,2]', 'a long text value over twelve bytes')", 1);
  run("select rowid, m from v", 1);
  end();

  // a column named like the table is rejected after its name was parsed
  begin("rejected create, partition key");
  run("create virtual table v using vec0(x float[2], v text partition key)", 0);
  end();
  begin("rejected create, auxiliary column");
  run("create virtual table v using vec0(x float[2], +v text)", 0);
  end();
  begin("rejected create, metadata column");
  run("create virtual table v using vec0(x float[2], v integer)", 0);
  end();

  // SQLite keeps a table whose DROP TABLE failed; vec0Destroy freed it anyway
  begin("failed drop, _chunks missing");
  run("create virtual table v using vec0(x float[1], m integer, +a text)", 1);
  run("insert into v(rowid, x, m, a) values (1, '[1]', 2, 'a')", 1);
  run("drop table v_chunks", 1);
  run("drop table v", 0);
  run("select rowid from v", 1);
  end();
  begin("failed drop, _info missing");
  run("create virtual table v using vec0(x float[1], m integer, +a text)", 1);
  run("insert into v(rowid, x, m, a) values (1, '[1]', 2, 'a')", 1);
  run("drop table v_info", 1);
  run("drop table v", 0);
  run("select rowid from v", 1);
  end();

  // the point query cleared the bindings of a statement that failed to prepare
  begin("point query with _rowids missing");
  run("create virtual table v using vec0(x float[1])", 1);
  run("insert into v(rowid, x) values (1, '[1]')", 1);
  run("drop table v_rowids", 1);
  run("select * from v where rowid = 1", 0);
  end();

  if (failures) {
    fprintf(stderr, "%d failure(s)\n", failures);
    return 1;
  }
  fprintf(stderr, "all leak scenarios passed\n");
  return 0;
}
