R4ditCast - Software para radiodifusión.
=============================================

![Raditcast-1](https://raw.githubusercontent.com/jfperini/raditcast/master/screenshots/raditcast-header.jpg "Raditcast-1")

#### RaditCast es un proyecto de software libre para streaming.

Puede ser utilizado para crear una estación de  radio en internet o para uso privado para servidores: Shoutcast e Icecast. 
    
    Soporta los estándares Ogg Vorbis, MP3, y AAC+.

## Instalar

    En ArchLinux:

    [user@archlinux:~] yaourt -S raditcast
    
Se necesitan algunos permisos. Ejecutar el [script](https://github.com/jfperini/radit/blob/master/radit.sh):

    [user@archlinux:~] sh ~/Descargas/radit.sh
    
    [user@archlinux:~] raditcast

## Compilar

    cd src

    qmake-qt4 raditcast.pro

    make

El binario generado se encuntra en la carpeta LinuxDesktop.

![Raditcast-2](https://raw.githubusercontent.com/jfperini/raditcast/master/screenshots/raditcast-footer.jpg "Raditcast-2")

Página oficial: http://www.radit.org/net


