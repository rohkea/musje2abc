# musje2abc
Converts tunes in Musje 123 notation to ABC

The program requires flex and bison (or other implementations of
lex and yacc, although they are untested) to compile.

See <https://github.com/jianpu/musje-123-notation> for more info,
and jianpu.webapp (<https://github.com/jianpu/jianpu.webapp>) for
a reference implemenation.

Unsupported syntax:

* Double sharps (`##`) and double flats (`bb`) are not supported yet.
* Dynamics (`\f`, `\ff`, etc; also unsupported in jianpu.webapp)
* All transpositions except `/1=K`: `/[K]`, `[+n]`, `[-n]`,
`(+na)`, `(-na)` (also not supported in jianpu.webapp)
* Tempo (also not supported in jianpu.webapp)
* Articulation (`x , v u > + tr`; also not supported in jianpu.webapp)
* Chords and voices (also not supported by jianpu.webapp)
* All tie types except `~` (also not supported by jianpu.webapp)
* All slur types except solid slurs `( )` (also not supported
by jianpu.webapp)
* Tuplets (also not supported by jianpu.webapp)
* Repeats (also not supported by jianpu.webapp)
* Grace notes (also not supported by jianpu.webapp)

Things that are not implemented yet:

* musje2abc currently accepts absolutely no command-line
arguments, just reads stdin and writes to stdout. Need to make
a normal argument handling.
* musje2abc always uses `L:1/8` in ABC, even when other length could
work better
* musje2abc is only tested on Linux and is likely not to work in Windows,
need to make a Windows makefile
* Quavers are not beamed automatically (jianpu.webapp does this);
I'm not sure how to handle that since Jianpu doesn't make the
distinction between beaming for vocal and instrumental music
* Comments are not carried over to ABC, but are ignored whatsoever

Additional syntax (might be removed in the future):

* Unlike jianpu.webapp, `/1=K` transpositions are supported.
* `1-----` can be used instead of `1----.` (in fact, any combination
of - is possible)
* Spaces between quavers and shorter notes in Musje 123
indicate beaming, as in ABC
