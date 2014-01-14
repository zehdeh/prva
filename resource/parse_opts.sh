gengetopt --input=resource/description.ggo
mv cmdline.c src/cmdline.cpp
mv cmdline.h include/
make clear
./prva --help
