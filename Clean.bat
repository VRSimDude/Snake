:: change to project folder
cd Snake

:: remove c++ projects output and intermediate
rmdir _out /q /s
rmdir _tmp /q /s

:: remove build
rmdir Build /q /s

:: remove visual studio files and log files
rmdir .vs /q /s
del *.log

:: change to visual studio project folder and delete user settings
cd Snake
del /q /s /f Snake.vcxproj.user