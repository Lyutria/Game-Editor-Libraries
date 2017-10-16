Game-Editor-Libraries
=====================
Some C-Language scripts for use in the game development software [*Game-Editor*] (http://game-editor.com)

### What is Game-Editor?
Game-Editor is a multi-platform 2D game development tool with exports to: Windows, Linux, Mac, iOS, and Android. Versions currently in development have Open-GL support and integrated Box2D physics as well. The community is a bit inactive, but nice. (catch me there as _DarkParadox_)

### What's there right now?
There's currently two versions. The one in the root directory is currently being developed, and under _bin_ you can find an example source .ged and an .exe that is compiled before I commit, so it will usually be up to date. That as well as various images that are used in the program.

### What can it do?
Not too much yet, honestly. The project is just starting up. The gist of it is the following features:
- Load images (BMP 24/32 currently) into a full-featured struct with properties like width/height for drawing.
- Load any image as a ASCII-based font with midstring tinting and ASCII escape codes
- Draw via scale and custom width / height properties.
- Some minor GUI elements
    - Buttons, windows, progress bar / meters
    - Binding to windows with left/right/top/bottom sizing
    - Managing GUI focus to bring windows forefront
- Generate code to load images into a struct without the original
- Generate debug files or a GUI element

### What's the plan?
Quite a bit more. There's no solid end-goal for this library, merely to add as much as I can think of to it.
- Implement more GUI elements (Sliders, textboxes, counters, toggles).
- Add the ability to load more different image types.
- Implement transparency on pixels.
- Create a text file loading / manipulation system. (Possibly _.ini_ loading as well?)
