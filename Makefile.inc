CC       = clang++
CCFLAGS  = -std=c++11 -Wall -pedantic -funsigned-char -msse4.2
LDFLAGS  = 
INCLUDE  = -I"/home/alex/Libraries/boost_1_57_0"

EXE      = main
AUXFLAGS =
OPTFLAGS =

# Comment out one of the OPTFLAGS -- either for Debug or for Release.
 
### Debug
# OPTFLAGS += -g	

### Release (If using -Ofast, note that it is not standard-compliant)
OPTFLAGS += -O3 -m32 -DNDEBUG -DNO_ERROR_MSG

# INPUT   = /home/alex/Documents/Textual/English\ dict/english-dict_filtered.txt
QUERIES = ./testing/english_typos.txt
