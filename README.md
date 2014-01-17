Game-Editor-Libraries
=====================
Some C-Language scripts for use in the game development software [*Game-Editor*] (http://game-editor.com)

### What is Game-Editor?
Game-Editor is a multi-platform 2D game development tool with exports to: Windows, Linux, Mac, iOS, and Android. Versions currently in development have Open-GL support and integrated Box2D physics as well. The community is a bit inactive, but nice. (catch me there as _DarkParadox_)

### What's there right now?
There's currently TWO versions up, you can find the old, single-file version (ltCanvas Tools, as it was called) in the _old_ folder, and check out the old documentation [here](http://dl.dropboxusercontent.com/u/117694669/Web/CanvasTools/index.html). This version comes with no support other than the documentation, and will not be updated.

The _new_ version is any file outside of the _old_ folder you can see. The files are seperated into categories (image manipulation, text manipulation, etc.) for selection and speed within the GE editor. This version is currently being developed.

### What can it do?
Not too much yet, honestly. The project is just starting up. The gist of it is the following features:
- Load images (BMP 24/32 currently) into a full-featured struct with properties like width/height for drawing.
- Use an image to draw text in a custom image font in the game, still fully modifiable using the image's properties.
- Manipulate the image's data easily (per pixel)

### What's the plan?
Quite a bit more. There's no solid end-goal for this library, merely to add as much as I can think of to it.
- Set up a proper image-based GUI system. e.g. buttons, menus, sliders.
- Create a seperate generator for a debug image / font so you can use the system without an image file.
- Create a text file loading / manipulation system. (Possibly _.ini_ loading as well?)
- Finish a proper documentation using the GIT wiki.
