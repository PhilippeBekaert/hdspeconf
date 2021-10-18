# hdspeconf
User space configuration tool for RME HDSPe MADI / AES / RayDAT / AIO and AIO Pro cards driven by the [snd-hdspe](https://github.com/PhilippeBekaert/snd-hdspe) driver.

**Building hdspeconf**

- Prerequisites: hdspeconf builds upon ALSA lib and wxwidgets. On ubuntu, install libasound2-dev, wx3.0-headers and libwxgtk3.0-dev.

- Clone the hdspeconf repository to your local disk. cd to the clone folder. Type

      make depend
      make
      
This will build the hdspeconf executable in your repository clone folder.

**Installation**

- Copy hdspeconf and dialog-warning.png to a folder of your choice, e.g. /usr/local/bin.

**Usage**

- cd to your repository clone folder, or make sure hdspeconf is in a folder in your $PATH, and type

     hdspeconf
     
on the command line, or make a desktop launcher for it and double click that. There are no command line arguments (yet, at this time).

**Status**

- RME HDSPe AIO Pro cards, TCO, Word Clock module and audio I/O extension boards are fully supported.
- RME HDSPe AIO, MADI, AES and RayDAT configuration panels TODO.

**Author**

[Philippe Bekaert](mailto:linux@panokkel.be), October 2021.
      
      
