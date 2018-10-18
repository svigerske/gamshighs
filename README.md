# gamshighs

This is a very basic code to call [HiGHS](https://github.com/ERGO-Code/HiGHS) from GAMS.

To build:
- Create a symlink "gams" pointing to a GAMS system directory.
- Create a symlink "highs" pointing to a HiGHS installation directory.
- Call make.

We assume Linux, maybe MacOS X will work too.

Run GAMS on a linear model with option keep=1 (and any GAMS solver).
Then call the gamshighs executable with the path to the GAMS control file (e.g., 225a/gamscntr.dat).
#Optionally, pass the name of a GAMS/HiGHS options file as additional argument.


To install in GAMS:
- In the GAMS system, create an executable file gmshisus.run with content
```
/path/to/here/gamshighs "$4"
```
- Add an entry for HiGHS to gmscmpun.txt in the GAMS system directory:
```
HIGHS 111 5 EC 1 0 1 LP RMIP
gmshisus.run
/path/to/here/gamshighs
```
