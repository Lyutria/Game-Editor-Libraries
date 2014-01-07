Game-Editor-Libraries
=====================

Some C-Language scripts for use in the game development software [*Game-Editor*] (http://game-editor.com)

Right now the actual scripts aren't much, either difficult to read/understand/deprecated by myself, and the new ones are incomplete or non-functional. I'm working on a couple things, mostly related to the "Canvas" actor/element of Game-Editor, which allows you to draw pixels as you please. The current solutions (available on the forums of the website) are limited, and not easy to use. For my scripts, I'm focusing on consolidating pretty much all of the data members into structs, reducing the amount of functions (GE doesn't allow overloading), and optimizing for the new structs.

### What is Game-Editor?
Game-Editor is a multi-platform 2D game development tool with exports to: Windows, Linux, Mac, iOS, and Android. Versions currently in development have Open-GL support and integrated Box2D physics as well. The community is a bit inactive, but nice. (catch me there as _DarkParadox_)

### What's there right now?
In the __old__ folder there's the most recent "stable" version of the libraries, and it should work perfectly. In GE, these sorts of scripts are added in the GLOBAL SCRIPTS menu. Access it by clicking _scripts_ in the main menu, and clicking _global code_. Paste or load the code into the editor, add a name at the bottom, and click _add_. The functions presented in the old version will work in any script you code then.

[__OLD DOCUMENTATION HERE__] (http://dl.dropboxusercontent.com/u/117694669/Web/CanvasTools/index.html)

There's also the NEW scripts, which are in-development. They may or may not work at any time, and they most likely _won't_, at least not very well.

### Plans
In the new scripts, I hold to implement this sort of system:
-  Image loading system (extensible) - I want to load any image into a generic "image" struct for manipulation
-  Image manipulation system - Really simple stuff, just flipping, basic rotation, color changing, stretching, etc.
-  Text-drawing system based on IMAGE structs - 95 characters, loads from image file the same way GE can already do it.
-  GUI system - Same as my previous GUI system, but using IMAGE structs, and more optimized.
