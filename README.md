# Rabid Rabbit
A game collecting sheep while trying not to be killed

## Installation

To compile and install the program, cd to the rabid_rabbit
directory created when you unzipped the archive and type:

    make
    sudo make install

You can then run the program by typing

    rrabbit

The "make install" creates the link /usr/bin/rrabbit which points
to /usr/games/rrabbit/rrabbit. This assumes that /usr/bin is in your
PATH variable.

The program does require wxWindows to be installed. You will
need the Run Time libraries to run it, and the Development
libraries to compile it. These can be found at

www.wxwindows.org

For Linux Mint 17, the following will be suitable:

    sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
    sudo apt-add-repository 'deb http://repos.codelite.org/wx3.0.2/ubuntu/ trusty universe'
    sudo apt-get update
    sudo apt-get install libwxbase3.0-0-unofficial \
                 libwxbase3.0-dev \
                 libwxgtk3.0-0-unofficial \
                 libwxgtk3.0-dev \
                 wx3.0-headers \
                 wx-common \
                 libwxbase3.0-dbg \
                 libwxgtk3.0-dbg


There is a help file (RRabbit.htm) which explains how to play

Enjoy the mayhem!
