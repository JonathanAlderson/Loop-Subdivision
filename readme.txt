To compile on feng-linux / feng-gps:

module add qt/5.13.0
qmake -project QT+=opengl
qmake
make

To compile on OSX:
Use Homebrew to install qt

qmake -project QT+=opengl
qmake
make


// Jonathan Alderson
All tasks have been completed.
Load a diredgesurface as normal though

./LoopSubdivisionRelease ../LoopSubdivisionRelease/diredgenormals/file.diredgenormal

To subdivide click the subdivide button.
Unfortunately, I couldn't get OpenGL to redraw upon Subdividing.
So you have to move the arc ball controller to see the result.

Files are automatically saved once the division happends.
They are saved into the 'subdivisions' folder.
The names are the same as before with the added vertex count on the end.

I'm happy with the neatness of this code, after my CW for Animation turned
into a bit of a mess, I wanted to make everything nice and neat.
Hopefully I've done a good job at that.

Nice coursework,

thanks,
Jonathan.
