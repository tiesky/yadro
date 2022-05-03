Hub for GpsCarControl Standalone Apps Windows binaries

Installation:
*) Download and unpack 
    https://raw.githubusercontent.com/tiesky/yadro/master/CDN/GCC/StandaloneApps/StandaloneApp.zip

*) Update file conf line by setting correct value of AppPrefix:
    <add key="AppPrefix" value=""/>

*) Run StandaloneApp.exe
    It will create directory with files and subdirectories
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\

*) App will restart itself and run from 
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\XXX\StandaloneApp.exe

*) Delete downloaded and unpacked files 

*) Create startup link to app 
    %USERPROFILE%\AppData\Local\S-TEC GmbH\GpsCarControl\Applications\XXX\StandaloneApp.exe

*) Note: application will contain default config file so you need it edit and restart app!
    It can be done using Watcher app minimizet to notification area near clock.
