bin/aowow_sql: aowow_sql.cpp dbcfile.cpp dbcfile.h
	g++ aowow_sql.cpp dbcfile.cpp -o bin/aowow_sql
bin/aowow.sql: bin/aowow_sql bin/comments.sql bin/faction.sql bin/instances.sql
	cd bin && ./aowow_sql
	cat bin/comments.sql >> bin/aowow.sql
	cat bin/faction.sql >> bin/aowow.sql
	cat bin/instances.sql >> bin/aowow.sql
#	cat bin/indexes.sql >> bin/aowow.sql
bin/aowow-sql-win32.exe: aowow_sql.cpp dbcfile.cpp dbcfile.h
	i586-mingw32msvc-g++ dbcfile.cpp aowow_sql.cpp -o bin/aowow-sql-win32.exe
bin/aowow-sql-win32.7z: bin/aowow-sql-win32.exe
	rm -Rf bin/aowow-sql-win32.7z
	i586-mingw32msvc-strip -s bin/aowow-sql-win32.exe
	7zr a bin/aowow-sql-win32.7z bin/aowow-sql-win32.exe
	rm -Rf bin/aowow-sql-win32.exe
win32: bin/aowow-sql-win32.7z
import: bin/aowow.sql bin/drop_indexes.sql bin/indexes.sql
	mysql -u root --password=DsWestb -h 192.168.0.183 --database aowow < bin/aowow.sql
#	mysql -u root --password=DsWestb -h 192.168.0.183 --database mangos < bin/drop_indexes.sql
	mysql -u root --password=DsWestb -h 192.168.0.183 --database mangos < bin/indexes.sql
distr: bin/aowow.sql
	7zr a bin/aowow-sql.7z bin/aowow.sql
all: win32 distr
clean:
	rm -f bin/aowow_sql
	rm -f bin/aowow.sql
	rm -f bin/aowow-sql-win32.exe
	rm -f bin/aowow-sql-win32.7z
	rm -f bin/aowow-sql.7z
