Game-Editor-Libraries
=====================

Some C-Language scripts for use in the game development software *Game-Editor*
( http://wwww.game-editor.com/ )

Right now the actual scripts aren't much, either difficult to read/understand/deprecated by myself, and the new ones are incomplete or non-functional. I'm working on a couple things, mostly related to the "Canvas" actor/element of Game-Editor, which allows you to draw pixels as you please. The current solutions (available on the forums of the website) are limited, and not easy to use. For my scripts, I'm focusing on consolidating pretty much all of the data members into structs, reducing the amount of functions (GE doesn't allow overloading), and optimizing for the new structs.

Some old stuff that I had already completed, include a:
-  Pixel-based text writing (**no image file required**) *(complete)*
-  GUI system for the canvas (**no image files required**) *(complete)*

But these things are written poorly, and lack customizability. Each GUI element is simply contained within a function that does the drawing and checks, and in general the code is poorly optimized (GUI is redrawn every frame instead of only when needed, etc).

I hope to complete this sort of thing:
-  Image loading system (extensible) - I want to load any image into a generic "image" struct for manipulation
-  Image manipulation system - Really simple stuff, just flipping, basic rotation, color changing, stretching, etc.
-  Text-drawing system based on IMAGE structs - 95 characters, loads from image file the same way GE can already do it.
-  GUI system - Same as my previous GUI system, but using IMAGE structs, and more optimized.
