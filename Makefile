include config.mk

all: musje2abc

musje2abc: y.tab.c lex.yy.c musje2abc.o read123.o writeabc.o common.h
	@${CC} lex.yy.c y.tab.c musje2abc.o read123.o writeabc.o -o musje2abc

y.tab.c: musje.y
	yacc -d musje.y

lex.yy.c: musje.l
	lex musje.l

musje2abc.o: musje2abc.c musje2abc.h common.h
	@${CC} -c -o musje2abc.o musje2abc.c

read123.o: read123.c read123.h common.h
	@${CC} -c -o read123.o read123.c

writeabc.o: writeabc.c writeabc.h common.h
	@${CC} -c -o writeabc.o writeabc.c

clean:
	@echo cleaning
	@rm -f ./musje2abc ./musje2abc.o ./read123.o ./writeabc.o
	@rm -f ./lex.yy.c
	@rm -f ./y.tab.[ch]

install: all
	@echo installing the executable to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -r musje2abc ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/musje2abc
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < musje2abc.1 >${DESTDIR}${MANPREFIX}/man1/musje2abc.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1

uninstall:
	@echo removing the executable from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/musje2abc
	@echo removing the manual page from ${DESTDIR}${MANPREFIX}/man1

.PHONY: all clean install uninstall
