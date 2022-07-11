Hub for GpsCarControl Standalone Apps Windows binaries

Installation:
1) Download and unpack 
    https://raw.githubusercontent.com/tiesky/yadro/master/CDN/GCC/StandaloneApps/StandaloneApp.zip

2) Update file conf line by setting correct value of AppPrefix:
    <add key="AppPrefix" value=""/>

3) Run StandaloneApp.exe
    It will create directory with files and subdirectories
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\

4) App will restart itself and run from 
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\XXX\StandaloneApp.exe

5) Delete downloaded and unpacked files 

6) Create startup link to app 
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\XXX\StandaloneApp.exe

7) Note: application will contain default config file so you need it edit and restart app!
    It can be done using Watcher app minimizet to notification area near clock.
