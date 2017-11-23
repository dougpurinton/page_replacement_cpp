cd /d %~dp0
g++.exe -Wall -c -g page_replace_polymorphism.cpp -o page_replace_polymorphism.o
g++.exe -static -static-libgcc -static-libstdc++ -o "page_replace_polymorphism_static.exe" page_replace_polymorphism.o
del page_replace_polymorphism.o
PAUSE