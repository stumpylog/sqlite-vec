"""Regression tests for vec0 error paths.

Each test makes a vec0 shadow table disappear or fail, then checks that the
extension reports an error instead of crashing or using freed memory. Ported
from photostructure/sqlite-vec (tests/test-error-paths.py).
"""

import re
import sqlite3
import subprocess
import sys

import pytest

EXTENSION = "dist/vec0"


def _raises(message, error=sqlite3.OperationalError):
    return pytest.raises(error, match=re.escape(message))


def test_point_query_reports_unpreparable_rowid_lookup(db):
    # vec0 prepares its rowid-to-chunk lookup on first use, and called
    # sqlite3_clear_bindings() on the NULL statement when that prepare failed.
    db.execute("CREATE VIRTUAL TABLE test USING vec0(v float[1])")
    db.execute("INSERT INTO test(rowid, v) VALUES (1, '[1]')")
    db.execute("DROP TABLE test_rowids")
    with _raises("could not initialize 'rowids get chunk position' statement"):
        db.execute("SELECT * FROM test WHERE rowid = 1").fetchall()


_FAILED_DROP_SCRIPT = """
import sqlite3, sys
ext, shadow, then = sys.argv[1:]
db = sqlite3.connect(":memory:", isolation_level=None)
db.enable_load_extension(True)
db.load_extension(ext)
db.execute("CREATE VIRTUAL TABLE v USING vec0(x float[1], m integer, +a text)")
db.execute("INSERT INTO v(rowid, x, m, a) VALUES (1, '[1]', 2, 'a')")
db.execute(f"DROP TABLE v_{shadow}")
try:
    db.execute("DROP TABLE v")
except sqlite3.OperationalError as e:
    print("drop failed:", e)
if then == "drop-again":
    print(db.execute("SELECT rowid, x, m, a FROM v").fetchall())
    db.execute(f"CREATE TABLE v_{shadow}(x)")
    db.execute("DROP TABLE v")
    print(db.execute("SELECT name FROM sqlite_master WHERE name LIKE 'v%'").fetchall())
db.close()
print("closed")
"""


@pytest.mark.parametrize("then", ["close", "drop-again"])
@pytest.mark.parametrize("shadow", ["chunks", "info"])
def test_failed_drop_keeps_table_usable(shadow, then):
    # vec0Destroy freed the table even when a DROP TABLE of a shadow table
    # failed, but SQLite keeps a virtual table whose xDestroy fails, so the
    # failed DROP TABLE itself, or the next query, DROP, or close, used freed
    # memory. The script runs in a child process because the bug crashed it.
    result = subprocess.run(
        [sys.executable, "-c", _FAILED_DROP_SCRIPT, EXTENSION, shadow, then],
        capture_output=True,
        text=True,
    )
    output = ["drop failed: SQL logic error"]
    if then == "drop-again":
        output += ["[(1, b'\\x00\\x00\\x80?', 2, 'a')]", "[]"]
    output += ["closed"]
    assert (result.returncode, result.stdout.splitlines()) == (0, output), result.stderr
