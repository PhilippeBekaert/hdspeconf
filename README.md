# hdspeconf
User space configuration tool for [RME](http://www.rme-audio.com/) HDSPe MADI / AES / RayDAT / AIO and AIO Pro cards, driven by the [snd-hdspe](https://github.com/PhilippeBekaert/snd-hdspe) driver.

**Building hdspeconf**

- Prerequisites: hdspeconf builds upon ALSA lib and wxwidgets. On ubuntu, 

     apt-get install libasound2-dev wx3.0-headers libwxgtk3.0-dev

The GUI panels were designed with [wxGlade](http://wxglade.sourceforge.net). You do not need wxGlade as long as you do not need or want to update the panel designs. If you do, make sure to use wxGlade version 1.0.0 or later. Older versions of wxGlade (such as the stock version on Ubuntu 20.04) will not work.

- Clone the hdspeconf repository to your local disk. cd to the clone folder. Type

      make depend
      make
      
This will build the hdspeconf executable in your repository clone folder.

**Installation**

- Copy hdspeconf and dialog-warning.png to a folder of your choice, e.g. ~/bin. Add that folder to your $PATH.

**Usage**

- cd to your repository clone folder, or make sure hdspeconf is in a folder in your $PATH, and type

     hdspeconf
     
on the command line, or make a desktop launcher for it and double click that. There are no command line arguments (yet, at this time).

- If you have a supported RME HDSPe card on your system, and the [snd-hdspe](https://github.com/PhilippeBekaert/snd-hdspe) driver is running, a panel comes up with configuration options and settings for your card(s). If either condition is not fulfilled, hdspeconf will
tell you as well.

**Status**

- RME HDSPe AIO Pro cards, TCO, Word Clock module and audio I/O extension boards are fully supported.
- RME HDSPe AIO, MADI, AES and RayDAT configuration panels TODO.
- Tested only on my Ubuntu Studio 20.04 development system so far.

**Acknowledgements**

- Thanks to [RME](http://www.rme-audio.com) for providing the necessary information and code for developing the driver.
- Thanks to [Amptec Belgium](http://www.amptec.be) for hardware support.


**License and (No) Warranty**

See [LICENSE](https://github.com/PhilippeBekaert/hdspeconf/blob/main/LICENSE).

**Author**

[Philippe Bekaert](mailto:linux@panokkel.be), October 2021.
      
      
