## Firebird Database to SQL insert statement (fdb2sql)

###### Export all table rows from .fdb file to SQL insert statement, optionally export only statement from input file

### Usage
```
fdb2sql -d database -u user -p password -i input -o output
```
-d: Full database path
-h: hostname to connect, default: localhost
-u: user name to connect, default: SYSDBA
-p: password to connect, default: masterkey
-i: SQL file with SELECT statement separated by ; semicolon, when away, export all tables rows
-o: output SQL statment to file, otherwise will be show on console

### Example
```
fdb2sql -d ..\data\EMPLOYEE.FDB -i ..\data\input.sql -o ..\data\output.sql
```

### Troubleshooting
Check if running Firebird version is 2.5.x, otherwise error like: "Database Server Error: unsupported on-disk structure" will appear

### Dependencies
Firebird 2.5.x server runnning

### Limitations
Don't export BLOB and ARRAY fields (Always Export as NULL)
Don't allow input statement with ; semicolon into, Ex.: SELECT (";") as WrongSplit;