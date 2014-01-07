//  _____           _                   _   _                  //
// |_   _|         | |                 | | (_)                 //
//   | |  _ __  ___| |_ _ __ _   _  ___| |_ _  ___  _ __  ___  //
//   | | | '_ \/ __| __| '__| | | |/ __| __| |/ _ \| '_ \/ __| //
//  _| |_| | | \__ \ |_| |  | |_| | (__| |_| | (_) | | | \__ \ //
// |_____|_| |_|___/\__|_|   \__,_|\___|\__|_|\___/|_| |_|___/ //
//                                                             //
// What is this?
//   This is LT Canvas API (Lyut, Lyut Canvas tools, Whatever)
//   by DarkParadox on Game-editor.com forums.
//   This has MANY functions to use on a canvas, some
//   of which being this like drawing text without a font
//   or actor, drawing gradients with transparency and skew.
//   drawing perfect circles or ellipses, and so on.
//
//   There's lots of comments on the code, so look at it if
//   you want to understand!
//
//   This version is ~2874 lines and adds about 30 KB to a .GED
//   This is VERSION 6.0:
//   Changelist:
//          This update is mostly just to make all the code run
//          a bit smoother for the user.
//      Added new STYLE functionality to the drawText system
//          STYLE option is now a DOUBLE, 0 is still normal text,
//          1 is still bold text. ANY number above one will now be
//          the scale to draw the text at (typing "2" will draw the
//          text at double size). As said above, values like "1.5"
//          are supported, but the resulted text will be pretty weird
//          looking.
//      Added "putpixeltoscale()" function
//          This is the function I made to support the above scale drawing.
//          Functions just like putpixel() but with a third parameter to
//          set the scale of the pixel to be drawn. Supports DOUBLE type
//          scale (decimals)
//      Added "drawTextS()"
//          More or less, I've decided to ignore drawTextP. I've left the
//          function for the sake of backwards compatibility, but I've made
//          the new function drawTextS. Four parameters: text, X, Y, Style.
//          I've removed the RGB functions because they can be simulated with
//          \a000 color codes instead.
//      Simplified font variable list
//          In previous versions, every letter of the font alphabet was its
//          own variable. This totally clogged up the variables/menu list,
//          and was hard to navigate when looking at this code file. Instead,
//          I've replaced it with a single struct type (FONTSTRUCT) and its
//          resulting font data "DEFAULTFONT." These two variables should
//          show up in the variables/functions menu, but none of the other
//          letters.
//      Added "\v" option to drawText functions.
//          This new option is used to shift the text over in any direction
//          you want within the rows and colums of the text. Scroll down
//          further into the text options to see exactly how to use it. You
//          can even use it to type text over other text in the same command,
//          and make stuff like underlined text by doing something like:
//            drawText("Text\v-4+0____");
//
// How do I use the text drawing system??
//     drawText("Text here");
//       This will draw the text in the top left of the canvas it's run on.
//       The "\n" symbol is usable for new lines.
//
//     drawTextX("Text here", x_position, y_position, Style);
//       Draw text at a specific XY position on the screen, and set the style
//       of the typing as well.
//
//     drawTextF("Text here", x, y, xPos, yPos, red, green, blue, style);
//       xPos and yPos move the text based on character size. So if you were to use:
//         drawTextF("My text", 0, 0, 1, 1, 255, 255, 255, 0);
//       It would draw white text one character down and one character right of
//       "0, 0" on the canvas.
//       Style is either "1" or "0", with 0 being normal and 1 being bold.
//       Bold is 1 pixel longer per letter than normal letters.
//       You can't switch to bold in the middle of a paragraph as it has different
//       spacing.
//
//     When writing the text, typing one of these (minus quotes) will do special actions:
//       "\n" - Will move the text onto a new line
//       "\a" - an "\a" follow by three numbers will change the following text to the
//         Corresponding color "\a[red][blue][green]", with "\a000" being black, and
//         "\a999" being white.
//       "\t" - followed by a number 0-9 will change the transparency level with "\t0" being
//         not transparent and "\t9" being fully transparent.
//         You can't set transparency any other way.
//         This isn't quite proper transparency. It will overwrite any pixels on the canvas
//         underneath it, but show any actors that are behind it. While this is a nice option,
//         it protectedobably won't be too useful to you.
//       "\v" - Will immediately shift the xpos and ypos of the letter over the specified num
//         of spaces. Works slightly like \a, example:
//           drawText("This is \v-1+3DOWNLEFT!");
//         Will shift the corresponding text down three rows, and one column to the left.
//         GUI stuff will accept this character, but will probably end up crashing your game
//         in really weird ways. Don't do it.

//  _____        __ _                  //
// |  __ \      / _(_)                 //
// | |  | | ___| |_ _ _ __   ___  ___  //
// | |  | |/ _ \  _| | '_ \ / _ \/ __| //
// | |__| |  __/ | | | | | |  __/\__ \ //
// |_____/ \___|_| |_|_| |_|\___||___/ //
//                                     //
#define pi 3.14159265

// For use with "typedef bool".
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0
#define THIS getclone(name)

// This is here because it's very annoying to type this every time I make a GUI script
#define MTA screen_to_actor(&xmouse, &ymouse)
// Alternate version
#define GUI screen_to_actor(&xmouse, &ymouse)
// "LUM" is short for "Luminosity." Just lets you type in one color if you don't feel
// like typing three.
#define LUM(X) X,X,X,X-20,X-20,X-20,X-30,X-30,X-30,255,255,255
#define RGB(X,Y,Z) X,Y,Z,X-20,Y-20,Z-20,X-30,Y-30,Z-30,255,255,255
#define THEME(X) X[0],X[1],X[2],X[3],X[4],X[5],X[6],X[7],X[8],X[9],X[10],X[11]

// Themes for THEME(X) ------------------------------------------------------//
typedef const int gui_theme[12];                                             //
gui_theme theme_dark  = {50,50,50,    40,40,40,    20,20,20,    255,255,255};//
gui_theme theme_grey  = {120,120,120, 80,80,80,    50,50,50,    255,255,255};//
gui_theme theme_light = {250,250,250, 200,200,200, 150,150,150, 0,0,0};      //
gui_theme theme_sepia = {80,58,35,    85,72,48,    85,80,60,    200,200,175};//
gui_theme theme_eos   = {65,85,65,    55,75,55,    45,55,45,    200,200,200};//
gui_theme theme_blank = {0,0,0,       20,20,20,    0,0,0,       255,255,255};//
gui_theme theme_ice   = {85,105,105,  65,100,100,  50,50,35,    175,225,225};//
int CURTHEME[12]      = {65,85,65,    55,75,55,    45,55,45,    200,200,200};//
//--------theme_NAME-----BORDER-------CLICKABLE----BACKGROUND---TEXT COLOR---//
//                   & MOUSE-OVER

// Other typedefs to make coding smoother.
typedef char* string;
typedef unsigned short int bool;

// This makes it easy to know the width
// and height of the letters we're using
// the font I set up is 5 wide and 11 tall.
const int CHARWIDTH = 5;
const int CHARHEIGHT = 11;

// Color themes for button GUI:

// CHARMAX is the maximum amount of characters
// that can be passed to the drawText functions.
// You can increase or decrease this to change
// memory usage (less is better, if you can afford it)
// I was forced to implement this when the FUNC_drawText
// couldn't take pointers (char *) as an argument properly.
// It can only take char arrays. You can bypass the limit
// by calling FUNC_drawText directly. It has the same
// arguments as drawTextF.
const int CHARMAX = 256;

// Contains drawing options for the GUI interface. Some new
// stuff like drawing the GUI using light gradients instead
// of solid colors, nothing customizable yet.
struct INTERFACE_OPTIONS {
    int textbox;
        // 0: With ">" button
        // 1: Without ">" button
    int menu;
        // 0: Fades in/out
        // 1: Instant in/out
} GUIOPT;
GUIOPT.textbox = 0;
GUIOPT.menu = 0;

//  _          _   _                 //
// | |        | | | |                //
// | |     ___| |_| |_ ___ _ __ ___  //
// | |    / _ \ __| __/ _ \ '__/ __| //
// | |___|  __/ |_| ||  __/ |  \__ \ //
// |______\___|\__|\__\___|_|  |___/ //
//                                   //
// These letters used to be arranged in an easy-to-
// understand way, but for the sake of end-user
// simplicity, I had to make it more complex.
// The FONTSTRUCT struct contains a variable for each
// printable letter of the alphabet for GE, and the
// subsequent struct declaration initializes each of
// this values to their 11x5 array.
// "1" means it places a pixel, "0" means it doesn't.

struct FONTSTRUCT {
    int CHAR_line[CHARHEIGHT][CHARWIDTH];
    int CHAR_block[CHARHEIGHT][CHARWIDTH];

    int CHAR_space[CHARHEIGHT][CHARWIDTH];
    int CHAR_a[CHARHEIGHT][CHARWIDTH];
    int CHAR_b[CHARHEIGHT][CHARWIDTH];
    int CHAR_c[CHARHEIGHT][CHARWIDTH];
    int CHAR_d[CHARHEIGHT][CHARWIDTH];
    int CHAR_e[CHARHEIGHT][CHARWIDTH];
    int CHAR_f[CHARHEIGHT][CHARWIDTH];
    int CHAR_g[CHARHEIGHT][CHARWIDTH];
    int CHAR_h[CHARHEIGHT][CHARWIDTH];
    int CHAR_i[CHARHEIGHT][CHARWIDTH];
    int CHAR_j[CHARHEIGHT][CHARWIDTH];
    int CHAR_k[CHARHEIGHT][CHARWIDTH];
    int CHAR_l[CHARHEIGHT][CHARWIDTH];
    int CHAR_m[CHARHEIGHT][CHARWIDTH];
    int CHAR_n[CHARHEIGHT][CHARWIDTH];
    int CHAR_o[CHARHEIGHT][CHARWIDTH];
    int CHAR_p[CHARHEIGHT][CHARWIDTH];
    int CHAR_q[CHARHEIGHT][CHARWIDTH];
    int CHAR_r[CHARHEIGHT][CHARWIDTH];
    int CHAR_s[CHARHEIGHT][CHARWIDTH];
    int CHAR_t[CHARHEIGHT][CHARWIDTH];
    int CHAR_u[CHARHEIGHT][CHARWIDTH];
    int CHAR_v[CHARHEIGHT][CHARWIDTH];
    int CHAR_w[CHARHEIGHT][CHARWIDTH];
    int CHAR_x[CHARHEIGHT][CHARWIDTH];
    int CHAR_y[CHARHEIGHT][CHARWIDTH];
    int CHAR_z[CHARHEIGHT][CHARWIDTH];

    int CHAR_0[CHARHEIGHT][CHARWIDTH];
    int CHAR_1[CHARHEIGHT][CHARWIDTH];
    int CHAR_2[CHARHEIGHT][CHARWIDTH];
    int CHAR_3[CHARHEIGHT][CHARWIDTH];
    int CHAR_4[CHARHEIGHT][CHARWIDTH];
    int CHAR_5[CHARHEIGHT][CHARWIDTH];
    int CHAR_6[CHARHEIGHT][CHARWIDTH];
    int CHAR_7[CHARHEIGHT][CHARWIDTH];
    int CHAR_8[CHARHEIGHT][CHARWIDTH];
    int CHAR_9[CHARHEIGHT][CHARWIDTH];

    int CHAR_leftbracket[CHARHEIGHT][CHARWIDTH];
    int CHAR_rightbracket[CHARHEIGHT][CHARWIDTH];
    int CHAR_exclamation[CHARHEIGHT][CHARWIDTH];
    int CHAR_quote[CHARHEIGHT][CHARWIDTH];
    int CHAR_hash[CHARHEIGHT][CHARWIDTH];
    int CHAR_dollar[CHARHEIGHT][CHARWIDTH];
    int CHAR_percent[CHARHEIGHT][CHARWIDTH];
    int CHAR_ampersand[CHARHEIGHT][CHARWIDTH];
    int CHAR_apostrophe[CHARHEIGHT][CHARWIDTH];
    int CHAR_leftparenthese[CHARHEIGHT][CHARWIDTH];
    int CHAR_rightparenthese[CHARHEIGHT][CHARWIDTH];
    int CHAR_asterisk[CHARHEIGHT][CHARWIDTH];
    int CHAR_plus[CHARHEIGHT][CHARWIDTH];
    int CHAR_at[CHARHEIGHT][CHARWIDTH];
    int CHAR_minus[CHARHEIGHT][CHARWIDTH];
    int CHAR_comma[CHARHEIGHT][CHARWIDTH];
    int CHAR_period[CHARHEIGHT][CHARWIDTH];
    int CHAR_slash[CHARHEIGHT][CHARWIDTH];
    int CHAR_backslash[CHARHEIGHT][CHARWIDTH];
    int CHAR_pipe[CHARHEIGHT][CHARWIDTH];
    int CHAR_caret[CHARHEIGHT][CHARWIDTH];
    int CHAR_underscore[CHARHEIGHT][CHARWIDTH];
    int CHAR_grave[CHARHEIGHT][CHARWIDTH];
    int CHAR_curlyleft[CHARHEIGHT][CHARWIDTH];
    int CHAR_curlyright[CHARHEIGHT][CHARWIDTH];
    int CHAR_tilde[CHARHEIGHT][CHARWIDTH];
    int CHAR_colon[CHARHEIGHT][CHARWIDTH];
    int CHAR_semicolon[CHARHEIGHT][CHARWIDTH];
    int CHAR_lessthan[CHARHEIGHT][CHARWIDTH];
    int CHAR_morethan[CHARHEIGHT][CHARWIDTH];
    int CHAR_equals[CHARHEIGHT][CHARWIDTH];

    int CHAR_A[CHARHEIGHT][CHARWIDTH];
    int CHAR_B[CHARHEIGHT][CHARWIDTH];
    int CHAR_C[CHARHEIGHT][CHARWIDTH];
    int CHAR_D[CHARHEIGHT][CHARWIDTH];
    int CHAR_E[CHARHEIGHT][CHARWIDTH];
    int CHAR_F[CHARHEIGHT][CHARWIDTH];
    int CHAR_G[CHARHEIGHT][CHARWIDTH];
    int CHAR_H[CHARHEIGHT][CHARWIDTH];
    int CHAR_I[CHARHEIGHT][CHARWIDTH];
    int CHAR_J[CHARHEIGHT][CHARWIDTH];
    int CHAR_K[CHARHEIGHT][CHARWIDTH];
    int CHAR_L[CHARHEIGHT][CHARWIDTH];
    int CHAR_M[CHARHEIGHT][CHARWIDTH];
    int CHAR_N[CHARHEIGHT][CHARWIDTH];
    int CHAR_O[CHARHEIGHT][CHARWIDTH];
    int CHAR_P[CHARHEIGHT][CHARWIDTH];
    int CHAR_Q[CHARHEIGHT][CHARWIDTH];
    int CHAR_R[CHARHEIGHT][CHARWIDTH];
    int CHAR_S[CHARHEIGHT][CHARWIDTH];
    int CHAR_T[CHARHEIGHT][CHARWIDTH];
    int CHAR_U[CHARHEIGHT][CHARWIDTH];
    int CHAR_V[CHARHEIGHT][CHARWIDTH];
    int CHAR_W[CHARHEIGHT][CHARWIDTH];
    int CHAR_X[CHARHEIGHT][CHARWIDTH];
    int CHAR_Y[CHARHEIGHT][CHARWIDTH];
    int CHAR_Z[CHARHEIGHT][CHARWIDTH];
};

struct FONTSTRUCT DEFAULTFONT = {
{
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0} },
{
        {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1},
        {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1},
        {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1} },

{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {0,0,0,0,1}, {0,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,0},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1}, {0,1,1,1,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,1,0}, {0,1,0,0,1}, {0,1,0,0,0}, {0,1,0,0,0},
        {1,1,1,1,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0},
        {0,1,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,1},
        {0,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0} },
{
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,1,1,0},
        {1,1,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,1,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,1,0}, {0,0,0,0,0}, {0,0,1,1,0},
        {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,0,1,0}, {1,0,0,1,0}, {0,1,1,0,0} },
{
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,1,0},
        {1,0,1,0,0}, {1,1,0,0,0}, {1,0,1,0,0}, {1,0,0,1,0},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,1,0,1,0},
        {1,0,1,0,1}, {1,0,1,0,1}, {1,0,1,0,1}, {1,0,1,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,1,1,0},
        {1,1,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,0},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,1},
        {0,0,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,1,1,0},
        {1,1,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,1,1,1,0},
        {1,0,0,0,1}, {0,1,1,0,0}, {0,0,0,1,0}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {1,1,1,1,0},
        {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,1},
        {0,0,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,1,1},
        {0,1,1,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,1,0,1},
        {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,0,0,1},
        {0,1,0,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,1,0,1,0},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,1,1}, {0,1,1,0,1},
        {0,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,1},
        {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },

{
        {0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,0,0}, {0,1,1,0,0}, {1,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1},
        {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,1,0}, {0,0,1,1,0}, {0,0,1,1,0}, {0,1,0,1,0},
        {0,1,0,1,0}, {1,0,0,1,0}, {1,1,1,1,1}, {0,0,0,1,0},
        {0,0,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,1,1,0},
        {1,1,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,1,0,0,0}, {0,1,0,0,0},
        {0,1,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },

{
        {0,1,1,1,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0},
        {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,1,0,1,0}, {1,1,1,1,1},
        {0,1,0,1,0}, {0,1,0,1,0}, {1,1,1,1,1}, {0,1,0,1,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,1,1,1,1}, {1,0,1,0,0}, {1,0,1,0,0},
        {0,1,1,0,0}, {0,0,1,1,0}, {0,0,1,0,1}, {0,0,1,0,1},
        {1,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,0,0,0}, {1,0,1,0,1}, {0,1,0,1,0}, {0,0,0,1,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,1,0,1,0}, {0,1,1,0,1},
        {1,0,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,0,0,0}, {1,0,1,0,0}, {1,0,1,0,0}, {0,1,0,0,0},
        {1,0,1,0,1}, {1,0,0,1,0}, {1,0,0,1,0}, {0,1,1,0,1},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,1,0}, {0,0,1,0,0}, {0,1,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {0,1,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0} },
{
        {0,0,1,0,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,0,1},
        {0,0,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1}, {0,0,0,0,1},
        {0,0,0,1,0}, {0,0,0,1,0}, {0,0,1,0,0} },
{
        {0,0,0,0,0}, {0,0,1,0,0}, {1,0,1,0,1}, {1,1,1,1,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {1,1,1,1,1}, {1,0,1,0,1},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,1,1},
        {1,0,1,0,1}, {1,0,1,0,1}, {1,0,1,1,0}, {1,0,0,0,0},
        {0,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,1,0},
        {0,0,1,0,0}, {0,1,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,1,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,1,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,0}, {1,0,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0},
        {0,0,1,0,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,0,1},
        {0,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0} },
{
        {0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {1,1,1,1,1}, {0,0,0,0,0} },
{
        {0,0,1,1,0}, {0,0,0,1,0}, {0,0,0,0,1}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {1,1,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,0,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,0,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,0,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {1,1,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,1,1,0,1}, {1,0,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,1,1,1,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,1,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,1,1,1,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,1,0},
        {0,0,1,0,0}, {0,1,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0},
        {0,1,0,0,0}, {1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0},
        {0,0,0,1,0}, {0,0,0,0,1}, {0,0,0,0,0} },
{
        {1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0}, {0,0,0,1,0},
        {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0},
        {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,1},
        {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,1}, {0,0,0,0,0},
        {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },

{
        {0,0,1,0,0}, {0,1,0,1,0}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,0}, {0,1,0,0,1}, {0,1,0,0,1}, {0,1,0,0,1},
        {0,1,1,1,0}, {0,1,0,0,1}, {0,1,0,0,1}, {0,1,0,0,1},
        {1,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,0}, {0,1,0,0,1}, {0,1,0,0,1}, {0,1,0,0,1},
        {0,1,0,0,1}, {0,1,0,0,1}, {0,1,0,0,1}, {0,1,0,0,1},
        {1,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,1,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,0,1,1,1}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {1,0,0,1,0},
        {0,1,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,1,0}, {1,0,1,0,0},
        {1,1,0,0,0}, {1,0,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,1,0,1,1}, {1,0,1,0,1},
        {1,0,1,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,1,0,0,1}, {1,1,0,0,1}, {1,0,1,0,1},
        {1,0,1,0,1}, {1,0,0,1,1}, {1,0,0,1,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0},
        {1,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,1,0,1},
        {0,1,1,1,0}, {0,0,0,0,1}, {0,0,0,0,0} },
{
        {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,1,1,1,0}, {1,0,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,0}, {1,0,0,0,0},
        {0,1,1,1,0}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1},
        {1,0,1,0,1}, {1,0,1,0,1}, {1,0,1,0,1}, {1,1,0,1,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0},
        {0,0,1,0,0}, {0,1,0,1,0}, {0,1,0,1,0}, {1,0,0,0,1},
        {1,0,0,0,1}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0},
        {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0},
        {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
{
        {1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,0,1,0},
        {0,0,1,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {1,0,0,0,0},
        {1,1,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} }
};

//  _____                  _______        _    //
// |  __ \                |__   __|      | |   //
// | |  | |_ __ __ ___      _| | _____  _| |_  //
// | |  | | '__/ _` \ \ /\ / / |/ _ \ \/ / __| //
// | |__| | | | (_| |\ V  V /| |  __/>  <| |_  //
// |_____/|_|  \__,_| \_/\_/ |_|\___/_/\_\\__| //
//                                             //

void putpixeltoscale(int X, int Y, double Scale) {
    int i, i2;
    for (i=0; i<Scale; i++) {
        moveto((X*Scale + i), (Y*Scale));
        lineto((X*Scale + i), (Y*Scale + (Scale-1)));
    }
}

// This function draws a single character like "L"
// It's good to seperate all the parts into their own functions.
// The "int (*lText)[CHARWIDTH]" let's me pass a 2D array like
// the ones above to this function.
void FUNC_drawChar(int (*lText)[CHARWIDTH], int X, int Y, int xPos, int yPos, int R, int G, int B, float T, double Style) {
    int i, i2, switcher = ceil(Style);
    // Set our putpixel()'s color.
    setpen(R, G, B, T, 1);

    // Loops through each row in order
    for(i=0; i<CHARHEIGHT; i++) {
        for(i2=0; i2<CHARWIDTH; i2++) {
            // If the spot we're on is "1", then
            // put a pixel of the selected color there.
            if(lText[i][i2] == 1) {
                // The math here is a bit complicated, so what
                // it does is this,
                //   (The distance set in X)
                //   plus (the width of a character) times (the set xPosition)
                //   plus (the column we're on in the middle of the letter)
                // Basically the same thing for Y too.
                // With case 1, which is bold, it places it twice, one placed 1 pixel to the right
                // to artificially elongate the characters
                // With any case above 1, we use the putpixeltoscale function to draw the text at
                // a larger scale.
                switch(switcher) {
                    case 0:
                        putpixel((X + ((CHARWIDTH + 1) * xPos)) + i2, (Y + ((CHARHEIGHT + 1) * yPos)) + i);
                        break;
                    case 1:
                        putpixel((X + ((CHARWIDTH + 2) * xPos)) + i2, (Y + ((CHARHEIGHT + 1) * yPos)) + i);
                        putpixel((1+ X + ((CHARWIDTH + 2) * xPos)) + i2, (Y + ((CHARHEIGHT + 1) * yPos)) + i);
                        break;
                    default:
                        putpixeltoscale((X + ((CHARWIDTH + 1) * xPos)) + i2, (Y + ((CHARHEIGHT + 1) * yPos)) + i, Style);
                }
            }
        }
    }
}

// This function receives all the parameters we need to write some text.
void FUNC_drawText(char *dText, int X, int Y, int sxPos, int syPos, int R, int G, int B, double Style) {
    int i, i2, xPos = sxPos, yPos = syPos;
    double T = 0;
    // Loop through "dText" until we reach the end.
    for (i=0; i<strlen(dText); i++) {
        // Set "cText" to the current character
        char cText = dText[i];
        // Start checking what cText is
        switch (cText) {
            case '\n':
                // If it's a New Line symbol, then
                // move our yPos down and set xPos back
                // to the start.
                yPos++;
                xPos = sxPos - 1;
                break;

            case '\a':
                // These handle colour changing in the middle of a
                // string. This one in particular takes the next 3
                // characters of a string and convert it to a RGB value.
                // This sets xPos back by one to offset the always functioning
                // +1 later in the code. We don't want the next char to be
                // offset.
                // The piece of code " - '0' " is a standard way to convert a
                // char value of a number to an int. Don't do it if you don't
                // know if the char is going to be an int or not.
                R = (255 / 9) * (dText[i+1] - '0');
                G = (255 / 9) * (dText[i+2] - '0');
                B = (255 / 9) * (dText[i+3] - '0');
                i += 3;
                xPos--;
                break;

            case '\t':
                // This handles transparency changing
                T = (1.0 / 9.0) * (dText[i+1] - '0');
                i++; xPos--;
                break;

            case '\v':
                xPos--;
                switch(dText[i+1]) {
                    case '-':
                        xPos -= dText[i+2] - '0';
                        break;
                    default:
                    case '+':
                        xPos += dText[i+2] - '0';
                        break;
                }

                switch(dText[i+3]) {
                    case '-':
                        yPos -= dText[i+4] - '0';
                        break;
                    default:
                    case '+':
                        yPos += dText[i+4] - '0';
                        break;
                }
                i += 4;
                break;

            case ' ':
                // Each of these cases is basically the same:
                //   Check the letter
                //   If it's this letter then send
                //   the command to draw a character with FUNC_drawChar

                // Every time we loop through the FOR loop above, the
                // xPos goes up, so we make a perfectly set down text
                FUNC_drawChar(DEFAULTFONT.CHAR_space, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'A':
                FUNC_drawChar(DEFAULTFONT.CHAR_A, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'a':
                FUNC_drawChar(DEFAULTFONT.CHAR_a, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
             case 'B':
                FUNC_drawChar(DEFAULTFONT.CHAR_B, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'b':
                FUNC_drawChar(DEFAULTFONT.CHAR_b, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'C':
                FUNC_drawChar(DEFAULTFONT.CHAR_C, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'c':
                FUNC_drawChar(DEFAULTFONT.CHAR_c, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
             case 'D':
                FUNC_drawChar(DEFAULTFONT.CHAR_D, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'd':
                FUNC_drawChar(DEFAULTFONT.CHAR_d, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'E':
                FUNC_drawChar(DEFAULTFONT.CHAR_E, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'e':
                FUNC_drawChar(DEFAULTFONT.CHAR_e, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'F':
                FUNC_drawChar(DEFAULTFONT.CHAR_F, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'f':
                FUNC_drawChar(DEFAULTFONT.CHAR_f, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'G':
                FUNC_drawChar(DEFAULTFONT.CHAR_G, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'g':
                FUNC_drawChar(DEFAULTFONT.CHAR_g, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'H':
                FUNC_drawChar(DEFAULTFONT.CHAR_H, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'h':
                FUNC_drawChar(DEFAULTFONT.CHAR_h, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'I':
                FUNC_drawChar(DEFAULTFONT.CHAR_I, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'i':
                FUNC_drawChar(DEFAULTFONT.CHAR_i, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'J':
                FUNC_drawChar(DEFAULTFONT.CHAR_J, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'j':
                FUNC_drawChar(DEFAULTFONT.CHAR_j, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'K':
                FUNC_drawChar(DEFAULTFONT.CHAR_K, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'k':
                FUNC_drawChar(DEFAULTFONT.CHAR_k, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'L':
                FUNC_drawChar(DEFAULTFONT.CHAR_L, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'l':
                FUNC_drawChar(DEFAULTFONT.CHAR_l, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'M':
                FUNC_drawChar(DEFAULTFONT.CHAR_M, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'm':
                FUNC_drawChar(DEFAULTFONT.CHAR_m, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'N':
                FUNC_drawChar(DEFAULTFONT.CHAR_N, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'n':
                FUNC_drawChar(DEFAULTFONT.CHAR_n, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'O':
                FUNC_drawChar(DEFAULTFONT.CHAR_O, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'o':
                FUNC_drawChar(DEFAULTFONT.CHAR_o, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'P':
                FUNC_drawChar(DEFAULTFONT.CHAR_P, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'p':
                FUNC_drawChar(DEFAULTFONT.CHAR_p, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'Q':
                FUNC_drawChar(DEFAULTFONT.CHAR_Q, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'q':
                FUNC_drawChar(DEFAULTFONT.CHAR_q, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'R':
                FUNC_drawChar(DEFAULTFONT.CHAR_R, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'r':
                FUNC_drawChar(DEFAULTFONT.CHAR_r, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'S':
                FUNC_drawChar(DEFAULTFONT.CHAR_S, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 's':
                FUNC_drawChar(DEFAULTFONT.CHAR_s, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'T':
                FUNC_drawChar(DEFAULTFONT.CHAR_T, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 't':
                FUNC_drawChar(DEFAULTFONT.CHAR_t, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'U':
                FUNC_drawChar(DEFAULTFONT.CHAR_U, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'u':
                FUNC_drawChar(DEFAULTFONT.CHAR_u, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'V':
                FUNC_drawChar(DEFAULTFONT.CHAR_V, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'v':
                FUNC_drawChar(DEFAULTFONT.CHAR_v, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'W':
                FUNC_drawChar(DEFAULTFONT.CHAR_W, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'w':
                FUNC_drawChar(DEFAULTFONT.CHAR_w, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'X':
                FUNC_drawChar(DEFAULTFONT.CHAR_X, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'x':
                FUNC_drawChar(DEFAULTFONT.CHAR_x, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'Y':
                FUNC_drawChar(DEFAULTFONT.CHAR_Y, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'y':
                FUNC_drawChar(DEFAULTFONT.CHAR_y, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'Z':
                FUNC_drawChar(DEFAULTFONT.CHAR_Z, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case 'z':
                FUNC_drawChar(DEFAULTFONT.CHAR_z, X, Y, xPos, yPos, R, G, B, T, Style);
                break;

            case '0':
                FUNC_drawChar(DEFAULTFONT.CHAR_0, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '1':
                FUNC_drawChar(DEFAULTFONT.CHAR_1, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '2':
                FUNC_drawChar(DEFAULTFONT.CHAR_2, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '3':
                FUNC_drawChar(DEFAULTFONT.CHAR_3, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '4':
                FUNC_drawChar(DEFAULTFONT.CHAR_4, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '5':
                FUNC_drawChar(DEFAULTFONT.CHAR_5, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '6':
                FUNC_drawChar(DEFAULTFONT.CHAR_6, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '7':
                FUNC_drawChar(DEFAULTFONT.CHAR_7, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '8':
                FUNC_drawChar(DEFAULTFONT.CHAR_8, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '9':
                FUNC_drawChar(DEFAULTFONT.CHAR_9, X, Y, xPos, yPos, R, G, B, T, Style);
                break;

            case '[':
                FUNC_drawChar(DEFAULTFONT.CHAR_leftbracket, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case ']':
                FUNC_drawChar(DEFAULTFONT.CHAR_rightbracket, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '!':
                FUNC_drawChar(DEFAULTFONT.CHAR_exclamation, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '"':
                FUNC_drawChar(DEFAULTFONT.CHAR_quote, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '#':
                FUNC_drawChar(DEFAULTFONT.CHAR_hash, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '$':
                FUNC_drawChar(DEFAULTFONT.CHAR_dollar, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '%':
                FUNC_drawChar(DEFAULTFONT.CHAR_percent, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '&':
                FUNC_drawChar(DEFAULTFONT.CHAR_ampersand, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '\'':
                FUNC_drawChar(DEFAULTFONT.CHAR_apostrophe, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '(':
                FUNC_drawChar(DEFAULTFONT.CHAR_leftparenthese, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case ')':
                FUNC_drawChar(DEFAULTFONT.CHAR_rightparenthese, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '*':
                FUNC_drawChar(DEFAULTFONT.CHAR_asterisk, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '+':
                FUNC_drawChar(DEFAULTFONT.CHAR_plus, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '@':
                FUNC_drawChar(DEFAULTFONT.CHAR_at, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '-':
                FUNC_drawChar(DEFAULTFONT.CHAR_minus, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case ',':
                FUNC_drawChar(DEFAULTFONT.CHAR_comma, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '.':
                FUNC_drawChar(DEFAULTFONT.CHAR_period, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '/':
                FUNC_drawChar(DEFAULTFONT.CHAR_slash, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '\\':
                FUNC_drawChar(DEFAULTFONT.CHAR_backslash, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '|':
                FUNC_drawChar(DEFAULTFONT.CHAR_pipe, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '^':
                FUNC_drawChar(DEFAULTFONT.CHAR_caret, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '_':
                FUNC_drawChar(DEFAULTFONT.CHAR_underscore, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '`':
                FUNC_drawChar(DEFAULTFONT.CHAR_grave, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '{':
                FUNC_drawChar(DEFAULTFONT.CHAR_curlyleft, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '}':
                FUNC_drawChar(DEFAULTFONT.CHAR_curlyright, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '~':
                FUNC_drawChar(DEFAULTFONT.CHAR_tilde, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case ':':
                FUNC_drawChar(DEFAULTFONT.CHAR_colon, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case ';':
                FUNC_drawChar(DEFAULTFONT.CHAR_semicolon, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '<':
                FUNC_drawChar(DEFAULTFONT.CHAR_lessthan, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '>':
                FUNC_drawChar(DEFAULTFONT.CHAR_morethan, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            case '=':
                FUNC_drawChar(DEFAULTFONT.CHAR_equals, X, Y, xPos, yPos, R, G, B, T, Style);
                break;
            default:
                FUNC_drawChar(DEFAULTFONT.CHAR_block, X, Y, xPos, yPos, R, G, B, T, Style);
        }
        // Raises the xPos so the next letter is to the right.
        xPos++;
    }
}

// The next three functions are what we use to call FUNC_drawText
// Each one has less parameters than the original except for drawTextF
// Instead, each has default numbers.
//
// Each of these auto converts your text to an array, since
// FUNC_drawText can't handle char* pointers.
void drawText(char *dText) {
    char buf[CHARMAX];
    strncpy(buf, dText, CHARMAX);
    FUNC_drawText(buf, 0, 0, 0, 0, 255, 255, 255, 0);
}

// Deprecated,
// Left in for backwards compatibility
void drawTextP(char *dText, int X, int Y, int R, int G, int B) {
    char buf[CHARMAX];
    strncpy(buf, dText, CHARMAX);
    FUNC_drawText(buf, X, Y, 0, 0, R, G, B, 0);
}

void drawTextS(char *dText, int X, int Y, double Style) {
    char buf[CHARMAX];
    strncpy(buf, dText, CHARMAX);
    FUNC_drawText(buf, X, Y, 0, 0, 255, 255, 255, Style);
}

void drawTextF(char *dText, int X, int Y, int xPos, int yPos, int R, int G, int B, double Style) {
    char buf[CHARMAX];
    strncpy(buf, dText, CHARMAX);
    FUNC_drawText(buf, X, Y, xPos, yPos, R, G, B, Style);
}

//  _____                     _               ______                 //
// |  __ \                   (_)             |  ____|                //
// | |  | |_ __ __ ___      ___ _ __   __ _  | |__ _   _ _ __   ___  //
// | |  | | '__/ _` \ \ /\ / / | '_ \ / _` | |  __| | | | '_ \ / __| //
// | |__| | | | (_| |\ V  V /| | | | | (_| | | |  | |_| | | | | (__  //
// |_____/|_|  \__,_| \_/\_/ |_|_| |_|\__, | |_|   \__,_|_| |_|\___| //
//                                     __/ |                         //
//                                    |___/                          //

int dif(int x1, int x2) {
    return max(x1, x2) - min(x1, x2);
}

// Replacement for setpen that doesn't require trasnp or pen size
void setColor(int R, int G, int B) {
    setpen(R, G, B, 0, 1);
}

// Requires setpen before use.
// This will draw a simple rectange using your pen.   1
void drawRectangle(int x1, int y1, int x2, int y2) {
    int i;
    for (i=0; i<dif(x1,x2); i++) putpixel(x1+i, y1);
    for (i=0; i<dif(x1,x2); i++) putpixel(x1+i, y2);
    for (i=0; i<dif(y1,y2)+1; i++) putpixel(x1, y1+i);
    for (i=0; i<dif(y1,y2)+1; i++) putpixel(x2, y1+i);
}

// Requires setpen before use.
// This will go through all the columns of the rectangle
// drawing it one line at a time.
void drawSolidRectangle(int x1, int y1, int x2, int y2) {
    int i = x1;
    for(i=x1; i<x2; i++) {
        moveto(i, y1);
        lineto(i, y2);
    }
}

// Requires setpen before use.
// Draws the edges of the canvas using the pen.
void drawEdges() {
   int i;
   for (i=0; i<width; i++) putpixel(i, 0);
   for (i=0; i<width; i++) putpixel(i, height-1);
   for (i=0; i<height; i++) putpixel(0, i);
   for (i=0; i<height; i++) putpixel(width-1, i);
}

// Vertical and horizontal gradients, valid "vh": 'v'(vertical), 'h'(horizontal).
// [defaults to vertical gradient]
//
// rgb1 being the color on the top OR left depending on the type of gradient.
//
// Syntax:
//   drawGradient('v', 255, 255, 255, 0, 0, 0);
// Draws a white-to-black gradient vertically.
void drawGradient(char vh, double r1, double g1, double b1, double r2, double g2, double b2) {
    double i;
    switch(vh) {
        default:
        case 'v':
            for(i=0; i<height; i++) {
                setpen(((1-max(i/height,0))*r1)+(max(i/height,0)*r2),
                       ((1-max(i/height,0))*g1)+(max(i/height,0)*g2),
                       ((1-max(i/height,0))*b1)+(max(i/height,0)*b2),
                       0, 1);
                moveto(0,i);
                lineto(width, i);
            }
            break;

        case 'h':
            for(i=0; i<width; i++) {
                setpen(((1-max(i/width,0))*r1)+(max(i/width,0)*r2), ((1-max(i/width,0))*g1)+(max(i/width,0)*g2), ((1-max(i/width,0))*b1)+(max(i/width,0)*b2), 0, 1);
                moveto(i, 0);
                lineto(i, height);
            }
    }
}

// Vertical and horizontal gradients, valid "vh": 'v'(vertical), 'h'(horizontal).
// [defaults to vertical gradient]
// t1 & t2 are for transperancy.
//
// xy1 and xy2 are for the position of the gradient on the vancas.
// You can also use transperancy in this version.
//
// Syntax:
//   drawGradientP('v', 0,0, 50,50, 255,255,255,0, 255,255,255,1);
// Draws a white-to-trasperant gradient vertically in the bounds of 0,0 and 50,50.
void drawGradientP(char vh, int x1, int y1, int x2, int y2, double r1, double g1, double b1, double t1, double r2, double g2, double b2, double t2) {
    double xmax = max(x1, x2);
    double xmin = min(x1, x2);
    double ymax = max(y1, y2);
    double ymin = min(y1, y2);
    double i;
    // if(er == 'e') erase(0,0,0,1);
    switch(vh) {
        default:
        case 'v':
            for(i=0; i<ymax-ymin; i++) {
                setpen(((1-max(i/(ymax-ymin),0))*r1)+(max(i/(ymax-ymin),0)*r2),
                       ((1-max(i/(ymax-ymin),0))*g1)+(max(i/(ymax-ymin),0)*g2),
                       ((1-max(i/(ymax-ymin),0))*b1)+(max(i/(ymax-ymin),0)*b2),
                       ((1-max(i/(ymax-ymin),0))*t1)+(max(i/(ymax-ymin),0)*t2),
                       1);
                moveto(x1,y1+i);
                lineto(x2,y1+i);
            }
            break;

        case 'h':
        for(i=0; i<xmax-xmin; i++) {
            setpen(((1-max(i/(xmax-xmin),0))*r1)+(max(i/(xmax-xmin),0)*r2),
                   ((1-max(i/(xmax-xmin),0))*g1)+(max(i/(xmax-xmin),0)*g2),
                   ((1-max(i/(xmax-xmin),0))*b1)+(max(i/(xmax-xmin),0)*b2),
                   ((1-max(i/(xmax-xmin),0))*t1)+(max(i/(xmax-xmin),0)*t2),
                   1);
            moveto(x1+i, y1);
            lineto(x1+i, y2);
        }
    }
}

// Vertical and horizontal gradients, valid "vh": 'v'(vertical), 'h'(horizontal).
// [defaults to vertical gradient]
// t1 & t2 are for transperancy.
// extra variable at the end to "skew" the gradient
//
// Syntax is the same as above, but with an extra number on the end.
// use it to skew the gradient in interesting ways.
void drawGradientF(char vh, int x1, int y1, int x2, int y2, double r1, double g1, double b1, double t1, double r2, double g2, double b2, double t2, double skew) {
    double xmax = max(x1, x2);
    double xmin = min(x1, x2);
    double ymax = max(y1, y2);
    double ymin = min(y1, y2);
    double i;
    // if(er == 'e') erase(0,0,0,1);
    switch(vh) {
        default:
        case 'v':
            for(i=0; i<ymax-ymin; i++) {
                setpen(((1-max(i/(ymax-ymin),0))*r1)+(max(i/(ymax-ymin),0)*r2),
                       ((1-max(i/(ymax-ymin),0))*g1)+(max(i/(ymax-ymin),0)*g2),
                       ((1-max(i/(ymax-ymin),0))*b1)+(max(i/(ymax-ymin),0)*b2),
                       ((1-max(i/(ymax-ymin),0))*t1)+(max(i/(ymax-ymin),0)*t2),
                       1);
                moveto(x1,y1+i);
                lineto(x2,y1+i+skew);
            }
            break;

        case 'h':
            for(i=0; i<xmax-xmin; i++) {
            setpen(((1-max(i/(xmax-xmin),0))*r1)+(max(i/(xmax-xmin),0)*r2),
                   ((1-max(i/(xmax-xmin),0))*g1)+(max(i/(xmax-xmin),0)*g2),
                   ((1-max(i/(xmax-xmin),0))*b1)+(max(i/(xmax-xmin),0)*b2),
                   ((1-max(i/(xmax-xmin),0))*t1)+(max(i/(xmax-xmin),0)*t2),
                   1);
            moveto(x1+i, y1);
            lineto(x1+i+skew, y2);
        }
    }
}

// Requires setpen before use.
// If you want to draw just a circle, use
// drawEllipses which is below.
void drawSolidCircle(double xpos, double ypos, double r) {
  // Function written by Asmodeus
  int xp, yp;
  for(yp=floor(ypos)-ceil(r);yp<ceil(ypos)+ceil(r);yp++) {
  for(xp=floor(xpos)-ceil(r);xp<ceil(xpos)+ceil(r);xp++) {
  if(sqrt(pow(max(xpos, xp)-min(xpos, xp), 2)+pow(max(ypos, yp)-min(ypos, yp), 2))<=r) putpixel(xp, yp);
  }}
}

//requires setpen before use.
void drawElipse(int xe, int ye, short int xrad,short int yrad) {
    // Function written by Novice
    float i, precision;

    if (abs(xrad+yrad)<30) precision=0.03;
    else if (abs(xrad+yrad)>29  && abs(xrad+yrad)<70)precision=0.01;
    else if (abs(xrad+yrad)>69  && abs(xrad+yrad)<140)precision=0.005;
    else if (abs(xrad+yrad)>139 && abs(xrad+yrad)<250)precision=0.0025;
    else precision=0.0008;
    //Draws the circle using a for loop
    for (i=0;i<2; i+=precision) putpixel(xe+xrad*cos(i*pi), ye+yrad*sin(i*pi));
}

//  _______          _      //
// |__   __|        | |     //
//    | | ___   ___ | |___  //
//    | |/ _ \ / _ \| / __| //
//    | | (_) | (_) | \__ \ //
//    |_|\___/ \___/|_|___/ //
//                          //
int random(int lower, int upper) {
    int v1 = max(upper, lower), v2 = min(upper, lower);
    return (rand(v1-v2)+v2);
}

char*intToStr(int val) {
    // Creates a string using sprintf
    char buf[128];
    sprintf(buf, "%d", val);
    return buf;
}

char*floatToStr(float val){
    // Creates a string using sprintf
    char buf[128];
    sprintf(buf, "%f", val);
    return buf;
}

int lockToGrid(int val, int delimiter)
{
    return round(val/delimiter)*delimiter;
}

int countChar(const char *str, char character) {
    const char *p = str;
    int count = 0;
    do {
        if (*p == character) count++;
    } while (*(p++));

    return count;
}

char toLower(char val) {
    switch (val) {
        case 'A': return 'a';
        case 'B': return 'b';
        case 'C': return 'c';
        case 'D': return 'd';
        case 'E': return 'e';
        case 'F': return 'f';
        case 'G': return 'g';
        case 'H': return 'h';
        case 'I': return 'i';
        case 'J': return 'j';
        case 'K': return 'k';
        case 'L': return 'l';
        case 'M': return 'm';
        case 'N': return 'n';
        case 'O': return 'o';
        case 'P': return 'p';
        case 'Q': return 'q';
        case 'R': return 'r';
        case 'S': return 's';
        case 'T': return 't';
        case 'U': return 'u';
        case 'V': return 'v';
        case 'W': return 'w';
        case 'X': return 'x';
        case 'Y': return 'y';
        case 'Z': return 'z';
    }
    return val;
}

char toUpper(char val) {
    switch (val) {
        case 'a': return 'A';
        case 'b': return 'B';
        case 'c': return 'C';
        case 'd': return 'D';
        case 'e': return 'E';
        case 'f': return 'F';
        case 'g': return 'G';
        case 'h': return 'H';
        case 'i': return 'I';
        case 'j': return 'J';
        case 'k': return 'K';
        case 'l': return 'L';
        case 'm': return 'M';
        case 'n': return 'N';
        case 'o': return 'O';
        case 'p': return 'P';
        case 'q': return 'Q';
        case 'r': return 'R';
        case 's': return 'S';
        case 't': return 'T';
        case 'u': return 'U';
        case 'v': return 'V';
        case 'w': return 'W';
        case 'x': return 'X';
        case 'y': return 'Y';
        case 'z': return 'Z';
    }
    return val;
}

char * strInsert (char* txt, int pos, char* in) {
    char firsthalf[255], secondhalf[255];
    int i;
    strncpy(firsthalf, txt, pos);

    // And now loop through the rest and add it to
    // secondhalf
    for (i=0; i<(strlen(txt)-pos); i++) {
        secondhalf[i] = txt[i+pos];
    }

    // Append the character to firsthalf
    strcat(firsthalf, in);

    // Reappend secondhalf
    strcat(firsthalf, secondhalf);

    // And make our text the new variable.
    strcpy(txt, firsthalf);
    return txt;
}

int strend(char*source, char*check) {
   char*temp1;
   strncpy(temp1, source, strlen(source)-strlen(check));
   strcat(temp1, check);
   if(strcmp(source, temp1)==0) return 1;
   else return 0;
}

int strbeg(char*source, char*check) {
    char*temp1;
    strncpy(temp1, source, strlen(check));
    if(!strcmp(temp1, check)) return 1;
    else return 0;
}

int strlens(char*T) {
    int i, i2;
    char buf[256];
    strcpy(buf, T);
    i2 = strlen(T);
    for (i=0; i<strlen(T); i++) {
        // Set "cText" to the current character
        char cText = buf[i];
        // Start checking what cText is
        switch (cText) {
            case '\a':
                i2-=4;
                break;
            case '\t':
                i2-=2;
                break;
            case '\n':
            case '\0':
                break;
            case '\v':
                i2-=5;
                break;
        }
    }
    return i2;
}




//   _____ _    _ _____  //
//  / ____| |  | |_   _| //
// | |  __| |  | | | |   //
// | | |_ | |  | | | |   //
// | |__| | |__| |_| |_  //
//  \_____|\____/|_____| //
//                       //

int hotspot(int X1, int Y1, int X2, int Y2) {
    if (xmouse >= X1 && xmouse <= X2 && ymouse >= Y1 && ymouse <= Y2) {
        return 1;
    }
    return 0;
}

int mouseOver() {
    if(hotspot(0,0,width,height)) return 1;
    return 0;
}

void label(char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int xpos = (width/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);
    int ypos = (height/2) - (CHARHEIGHT/2);

    // 20,20,20 is minumum RGB, 255,255,255 is max.
    erase(R3,G3,B3,0);
    setpen(R,G,B,0,1);
    drawEdges();
    drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);
}

void button(char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int i = mouseOver();
    int xpos = (width/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);
    int ypos = (height/2) - (CHARHEIGHT/2);

    switch (i) {
        case 0:
            erase(R3,G3,B3,0);
            break;
        case 1:
            erase(R2,G2,B2,0);
    }
    setpen(R,G,B,0,1);
    drawEdges();
    drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);
}

void buttonToggle(int var, char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int xpos, ypos = (height/2) - (CHARHEIGHT/2);
    xpos = 15 + ((width-12)/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);

    // Drawtext run early so counter buttons show over prompt
    erase(R3,G3,B3,0);
    drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);

    setpen(R,G,B,0,1);
    drawEdges();

    if(var == 0) {
        setpen(R3,G3,B3,0,1);
        drawSolidRectangle(5,height/2-5, 15, height/2+5);
        setpen(R2,G2,B2,0,1);
        drawRectangle(5,height/2-5, 15, height/2+5);
        setpen(R,G,B,0,1);
        if(hotspot(0,0,15,height)) drawSolidRectangle(5,height/2-5, 16, height/2+5);
    }
    else {
        setpen(R2,G2,B2,0,1);
        drawRectangle(5,height/2-5, 15, height/2+5);
        setpen(R,G,B,0,1);
        drawSolidRectangle(5,height/2-5, 16, height/2+5);
        setpen(R2,G2,B2,0,1);
        if(hotspot(0,0,15,height)) drawSolidRectangle(6,height/2-4, 15, height/2+4);
        setpen(R,G,B,0,1);
        drawSolidRectangle(8,height/2-2, 13, height/2+2);
    }
}

void buttonToggleDown(long int *var) {
    if(hotspot(0,0,15,height)) {
        if (*var==1) *var=0;
        else *var=1;
    }
}

void buttonCounter(int var, char*delim1, char*delim2, char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int xpos, ypos = (height/2) - (CHARHEIGHT/2);
    char buf[64];
    sprintf(buf, "%s%d", T, var);
    xpos = (width/2) - (((strlens(buf) * (CHARWIDTH+1))-1)/2);

    // Drawtext run early so counter buttons show over prompt
    erase(R3,G3,B3,0);
    drawTextF(buf, xpos,ypos, 0,0, R4,G4,B4, 0);

    setpen(R2,G2,B2,0,1);
    drawSolidRectangle(width-(CHARWIDTH*3),0,width,height);
    drawSolidRectangle(0,0,CHARWIDTH*3,height);
    setpen(R,G,B,0,1);
    if(hotspot(0,0,CHARWIDTH*3,height)) drawSolidRectangle(0,0,CHARWIDTH*3,height);
    if(hotspot(width-(CHARWIDTH*3),0,width,height)) drawSolidRectangle(width-(CHARWIDTH*3),0,width,height);

    drawEdges();
    if (strlens(delim1) != 1 || strlens(delim2) !=1) {
        drawTextF("-", (1+CHARWIDTH),ypos, 0,0, R4,G4,B4, 0);
        drawTextF("+", (width-(CHARWIDTH*2)),ypos, 0,0, R4,G4,B4, 0);
    } else {
        drawTextF(delim1, (1+CHARWIDTH),ypos, 0,0, R4,G4,B4, 0);
        drawTextF(delim2, (width-(CHARWIDTH*2)),ypos, 0,0, R4,G4,B4, 0);
    }
}

void buttonCounterDown(long int* var, int inc, int lLimit, int hLimit) {
    if(hotspot(0,0,CHARWIDTH*3,height)) {
        if(*var > lLimit) *var = *var - inc;
        else *var = hLimit;
    }
    if(hotspot(width-(CHARWIDTH*3),0,width,height)) {
        if(*var < hLimit) *var = *var + inc;
        else *var = lLimit;
    }
}

//void comboBox(char *list, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
//    int i, i2, ypos, xpos, lines=0;
//    char buf[256];
//    erase(R3,G3,B3,0);
//    setpen(R,G,B,0,1);
//    drawEdges();
//    // Get the amount of '\n' characters in LIST
//    lines = countChar(list, '\n');
//    for (i=0; i<strlen(list); i++) {
//        if (list[i] == '\n') sprintf(buf, "%s%c", buf, list[i]);
//        else {
//            i2++;
//            xpos = (width/2) - (((strlen(buf) * (CHARWIDTH+1))-1)/2);
//            ypos = ((i2*(height/lines)) - ((height/lines)/2)) - (CHARHEIGHT/2);
//            drawTextF(buf, xpos,ypos, 0,0, 255,255,255, 0);
//            strcpy(buf, "");
//        }
//    }
//}

void drawGrid(int step, int R, int G, int B) {
    int X, Y;
    setpen(R,G,B,0,1);
    for(Y=0;Y<height+1;Y+=step)
    {
        moveto(0, Y);
        lineto(width, Y);
    }
    for(X=0;X<width+1;X+=step)
    {
        moveto(X, 0);
        lineto(X, height);
    }
}

void textBox(int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int i, i2=0, i3=0;
    int ax = animindex;
    int xpos, ypos = (height/2) - (CHARHEIGHT/2);
    xpos = 2;

    erase(R3,G3,B3,0);

    if(transp == 1) animpos=0;

    if (animindex*(CHARWIDTH+1) > (width - ((CHARWIDTH+1)*5))) {
        xpos = ((width - ((CHARWIDTH+1)*5))- (animindex*(CHARWIDTH+1)));
    }

    // Drawtext run early so counter buttons show over prompt
    drawTextF(text, xpos,ypos, 0,0, R4,G4,B4, 0);
    if (GUIOPT.textbox == 0) {
        setpen(R2,G2,B2,0,1);
        drawSolidRectangle(width-(CHARWIDTH*3),0,width,height);
        setpen(R,G,B,0,1);
        if(hotspot((width - ((CHARWIDTH+1)*5)),0,width,height)) drawSolidRectangle(width-(CHARWIDTH*3),0,width,height);
        drawTextF(">", (width-(CHARWIDTH*2)),ypos, 0,0, R4,G4,B4, 0);
    }
    setpen(R,G,B,0,1);
    drawEdges();
    if(strlen(text)*(CHARWIDTH+1) > (width - ((CHARWIDTH+1)*5)) && strlen(text) > animindex) {
        if(GUIOPT.textbox == 0) {
            drawGradientP('h', (width-(CHARWIDTH*3))-5,1, (width-(CHARWIDTH*3)),height-1, R3,G3,B3,0,R2,G2,B2,0);
        }
        else {
            drawGradientP('h', width-6,1,width-1,height-1, R3,G3,B3,0,R2,G2,B2,0);
        }
    }
    if(animindex*(CHARWIDTH+1) > (width - ((CHARWIDTH+1)*5))) {
        drawGradientP('h', 1,1, 6,height-1, R,G,B,0,R3,G3,B3,0);
    }
    if (animpos == 1) { animindex = strlen(text); animpos = 2; }
    if (animpos == 2) {

        // Set AX to the current horizontal position,
        // then check if it's less than the current line's
        // length. If it isn't, set it to the end of the line.
        ax = animindex;

        // Caret drawing
        if (view.animpos <= real_fps / 2) {
            FUNC_drawChar(DEFAULTFONT.CHAR_line, xpos,ypos, animindex, 0, R4, G4, B4,0,0);
        }
        if (view.animpos <= 0) {
            view.animpos = real_fps;
        }
    }
}

void textBoxKeyDown(int limit, char* modes) {
    char* key = GetKeyState();
    Actor * actor = getclone(name);
    int letters=0, numbers=0, symbols=0, whitespace=0, i;
    for (i=0; i<strlen(modes); i++) {
        switch(modes[i]) {
            case 'a':
            case 'A':
                letters=1;
                numbers=1;
                symbols=1;
                whitespace=1;
                break;
            case 'l':
            case 'L':
                letters=1;
                break;
            case 'n':
            case 'N':
                numbers=1;
                break;
            case 's':
            case 'S':
                symbols=1;
                break;
            case 'w':
            case 'W':
                whitespace=1;
        }
    }

    if (actor->animpos == 2) {
        if(key[KEY_RETURN] || key[KEY_PAD_ENTER]) actor->animpos = 0;
        else if (key[KEY_LEFT]) {
            if (actor->animindex > 0) animindex -= 1;
            view.animpos = real_fps / 2;
            view.textNumber = real_fps / 8;
        }
        else if (key[KEY_RIGHT]) {
            if (actor->animindex < strlen(actor->text)) actor->animindex += 1;
            view.animpos = real_fps / 2;
            view.textNumber = real_fps / 8;
        } else if (key[KEY_SPACE] && whitespace==1) {
            if (strlen(actor->text) < limit || limit == 0) {
                strInsert(actor->text, actor->animindex, " ");
                actor->animindex+=1;
            }
        } else if (key[KEY_BACKSPACE]) {
            if (strlen(actor->text) > 0) {
                if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
                    strcpy(actor->text, "");
                    actor->animindex = 0;
                } else {
                    char firsthalf[255], secondhalf[255];
                    int i, pos = actor->animindex;
                    strncpy(firsthalf, actor->text, pos-1);
                    for (i=0; i<(strlen(actor->text)-pos); i++) {
                        secondhalf[i] = actor->text[i+pos];
                    }
                    strcat(firsthalf, secondhalf);
                    strcpy(actor->text, firsthalf);
                    actor->animindex-=1;
                }
            }
        } else if (key[KEY_HOME]) {
            actor->animindex = 0;
        } else if (key[KEY_END]) {
            actor->animindex = strlen(actor->text);
        } else if (strlen(actor->text) < limit || limit == 0) {
            if(letters==1) {
                if(key[KEY_a] ||
                    key[KEY_b] ||
                    key[KEY_c] ||
                    key[KEY_d] ||
                    key[KEY_e] ||
                    key[KEY_f] ||
                    key[KEY_g] ||
                    key[KEY_h] ||
                    key[KEY_i] ||
                    key[KEY_j] ||
                    key[KEY_k] ||
                    key[KEY_l] ||
                    key[KEY_m] ||
                    key[KEY_n] ||
                    key[KEY_o] ||
                    key[KEY_p] ||
                    key[KEY_q] ||
                    key[KEY_r] ||
                    key[KEY_s] ||
                    key[KEY_t] ||
                    key[KEY_u] ||
                    key[KEY_v] ||
                    key[KEY_w] ||
                    key[KEY_x] ||
                    key[KEY_y] ||
                    key[KEY_z] ) {

                    int keyCode = getLastKey();
                    char* keyText = getKeyText(keyCode);
                    if(!key[KEY_LSHIFT]&&!key[KEY_RSHIFT]) keyText[0] = toLower(keyText[0]);
                    strInsert(actor->text, actor->animindex, keyText);
                    actor->animindex+=1;
                }
            }
            if (numbers==1) {
                if(!key[KEY_LSHIFT] && !key[KEY_RSHIFT]) {
                    if(key[KEY_0] || key[KEY_PAD_0]) {
                        strInsert(actor->text, actor->animindex, "0");
                        actor->animindex+=1;
                    } else if(key[KEY_1] || key[KEY_PAD_1]) {
                        strInsert(actor->text, actor->animindex, "1");
                        actor->animindex+=1;
                    } else if(key[KEY_2] || key[KEY_PAD_2]) {
                        strInsert(actor->text, actor->animindex, "2");
                        actor->animindex+=1;
                    } else if(key[KEY_3] || key[KEY_PAD_3]) {
                        strInsert(actor->text, actor->animindex, "3");
                        actor->animindex+=1;
                    } else if(key[KEY_4] || key[KEY_PAD_4]) {
                        strInsert(actor->text, actor->animindex, "4");
                        actor->animindex+=1;
                    } else if(key[KEY_5] || key[KEY_PAD_5]) {
                        strInsert(actor->text, actor->animindex, "5");
                        actor->animindex+=1;
                    } else if(key[KEY_6] || key[KEY_PAD_6]) {
                        strInsert(actor->text, actor->animindex, "6");
                        actor->animindex+=1;
                    } else if(key[KEY_7] || key[KEY_PAD_7]) {
                        strInsert(actor->text, actor->animindex, "7");
                        actor->animindex+=1;
                    } else if(key[KEY_8] || key[KEY_PAD_8]) {
                        strInsert(actor->text, actor->animindex, "8");
                        actor->animindex+=1;
                    } else if(key[KEY_9] || key[KEY_PAD_9]) {
                        strInsert(actor->text, actor->animindex, "9");
                        actor->animindex+=1;
                    }
                }
            }
            if (symbols==1) {
                if(key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
                    if(key[KEY_0]) {
                        strInsert(actor->text, actor->animindex, ")");
                        actor->animindex+=1;
                    } else if(key[KEY_1]) {
                        strInsert(actor->text, actor->animindex, "!");
                        actor->animindex+=1;
                    } else if(key[KEY_2]) {
                        strInsert(actor->text, actor->animindex, "@");
                        actor->animindex+=1;
                    } else if(key[KEY_3]) {
                        strInsert(actor->text, actor->animindex, "#");
                        actor->animindex+=1;
                    } else if(key[KEY_4]) {
                        strInsert(actor->text, actor->animindex, "$");
                        actor->animindex+=1;
                    } else if(key[KEY_5]) {
                        strInsert(actor->text, actor->animindex, "%");
                        actor->animindex+=1;
                    } else if(key[KEY_6]) {
                        strInsert(actor->text, actor->animindex, "^");
                        actor->animindex+=1;
                    } else if(key[KEY_7]) {
                        strInsert(actor->text, actor->animindex, "&");
                        actor->animindex+=1;
                    } else if(key[KEY_8]) {
                        strInsert(actor->text, actor->animindex, "*");
                        actor->animindex+=1;
                    } else if(key[KEY_9]) {
                        strInsert(actor->text, actor->animindex, "(");
                        actor->animindex+=1;
                    }

                    else if (key[KEY_PERIOD]) {
                        strInsert(actor->text, actor->animindex, ">");
                        actor->animindex+=1;
                    } else if (key[KEY_COMMA]) {
                        strInsert(actor->text, actor->animindex, "<");
                        actor->animindex+=1;
                    } else if (key[KEY_QUOTE]) {
                        strInsert(actor->text, actor->animindex, ":");
                        actor->animindex+=1;
                    } else if (key[KEY_SEMICOLON]) {
                        strInsert(actor->text, actor->animindex, ":");
                        actor->animindex+=1;
                    } else if (key[KEY_LEFTBRACKET]) {
                        strInsert(actor->text, actor->animindex, "{");
                        actor->animindex+=1;
                    } else if (key[KEY_RIGHTBRACKET]) {
                        strInsert(actor->text, actor->animindex, "}");
                        actor->animindex+=1;
                    } else if (key[KEY_BACKSLASH]) {
                        strInsert(actor->text, actor->animindex, "|");
                        actor->animindex+=1;
                    } else if (key[KEY_EQUALS]) {
                        strInsert(actor->text, actor->animindex, "+");
                        actor->animindex+=1;
                    } else if (key[KEY_MINUS]) {
                        strInsert(actor->text, actor->animindex, "_");
                        actor->animindex+=1;
                    } else if (key[KEY_SLASH]) {
                        strInsert(actor->text, actor->animindex, "?");
                        actor->animindex+=1;
                    } else if (key[KEY_BACKQUOTE]) {
                        strInsert(actor->text, actor->animindex, "~");
                        actor->animindex+=1;
                    }
                } else {
                    if (key[KEY_PERIOD]) {
                        strInsert(actor->text, actor->animindex, ".");
                        actor->animindex+=1;
                    } else if (key[KEY_COMMA]) {
                        strInsert(actor->text, actor->animindex, ",");
                        actor->animindex+=1;
                    } else if (key[KEY_QUOTE]) {
                        strInsert(actor->text, actor->animindex, "'");
                        actor->animindex+=1;
                    } else if (key[KEY_SEMICOLON]) {
                        strInsert(actor->text, actor->animindex, ";");
                        actor->animindex+=1;
                    } else if (key[KEY_LEFTBRACKET]) {
                        strInsert(actor->text, actor->animindex, "[");
                        actor->animindex+=1;
                    } else if (key[KEY_RIGHTBRACKET]) {
                        strInsert(actor->text, actor->animindex, "]");
                        actor->animindex+=1;
                    } else if (key[KEY_BACKSLASH]) {
                        strInsert(actor->text, actor->animindex, "\\");
                        actor->animindex+=1;
                    } else if (key[KEY_EQUALS]) {
                        strInsert(actor->text, actor->animindex, "=");
                        actor->animindex+=1;
                    } else if (key[KEY_MINUS]) {
                        strInsert(actor->text, actor->animindex, "-");
                        actor->animindex+=1;
                    } else if (key[KEY_SLASH]) {
                        strInsert(actor->text, actor->animindex, "/");
                        actor->animindex+=1;
                    } else if (key[KEY_BACKQUOTE]) {
                        strInsert(actor->text, actor->animindex, "`");
                        actor->animindex+=1;
                    }

                    else if (key[KEY_PAD_DIVIDE]) {
                        strInsert(actor->text, actor->animindex, "/");
                        actor->animindex+=1;
                    } else if (key[KEY_PAD_MULTIPLY]) {
                        strInsert(actor->text, actor->animindex, "*");
                        actor->animindex+=1;
                    } else if (key[KEY_PAD_MINUS]) {
                        strInsert(actor->text, actor->animindex, "-");
                        actor->animindex+=1;
                    } else if (key[KEY_PAD_PLUS]) {
                        strInsert(actor->text, actor->animindex, "+");
                        actor->animindex+=1;
                    } else if (key[KEY_PAD_EQUALS]) {
                        strInsert(actor->text, actor->animindex, "=");
                        actor->animindex+=1;
                    } else if (key[KEY_PAD_PERIOD]) {
                        strInsert(actor->text, actor->animindex, ".");
                        actor->animindex+=1;
                    }
                }
            }
        }
    }
}

void textBoxMouseDown() {
    if (GUIOPT.textbox == 0) {
        if(hotspot(width-(CHARWIDTH*3),0,width,height)) animpos = 0;
        else {
            animpos = 2;
            if(animindex*(CHARWIDTH+1) > (width - ((CHARWIDTH+1)*5))) {
                animindex = ((((animindex*(CHARWIDTH+1))-(width - ((CHARWIDTH+1)*5))) + (xmouse)) / (CHARWIDTH+1))+1;
            } else animindex = (xmouse+2) / (CHARWIDTH+1);
            if (animindex > strlen(text)) animindex = strlen(text);
        }
    }
    else {
        animpos = 2;
        if(animindex*(CHARWIDTH+1) > (width - ((CHARWIDTH+1)*5))) {
            animindex = ((((animindex*(CHARWIDTH+1))-(width - ((CHARWIDTH+1)*5))) + (xmouse)) / (CHARWIDTH+1))+1;
        } else animindex = (xmouse+2) / (CHARWIDTH+1);
        if (animindex > strlen(text)) animindex = strlen(text);
    }
}

void textBoxCreate(char *sampleText) {
    view.textNumber = 0;
    animindex = 0;
    animpos = 0;
    strcpy(text, sampleText);
}

void viewCode() {
    if (animpos > 0) animpos--;
}

void progressBar(double var, double max, char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int xpos = (width/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);
    int ypos = (height/2) - (CHARHEIGHT/2);

    // 20,20,20 is minumum RGB, 255,255,255 is max.
    erase(R3,G3,B3,0);
    setpen(R2,G2,B2,0,1);
    drawSolidRectangle(1,1,round((var/max) * (width-2)),height-1);
    setpen(R,G,B,0,1);
    drawEdges();
    drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);
}

void window(char* T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    int xpos = ((width - CHARWIDTH*3)/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);
    int ypos = (CHARHEIGHT) - (CHARHEIGHT/2);
    if (transp == 0.01) {
        // Window Shade mode
        erase(0,0,0,1);
    }
    else {
        erase(R3,G3,B3,0);
        setpen(R,G,B,0,1);
        drawEdges();
    }
    setpen(R2,G2,B2,0,1);
    drawSolidRectangle(0,0, width, CHARHEIGHT * 2);
    if(hotspot((width-3)-CHARWIDTH*3, 3, width-3, CHARHEIGHT*2-3)) {
        setpen(R,G,B,0,1);
        drawSolidRectangle((width-3)-CHARWIDTH*3, 3, width-3, CHARHEIGHT*2-3);
    }
    else if(hotspot(0,0, width, CHARHEIGHT*2)) {
        setpen(R,G,B,0,1);
        drawSolidRectangle(0,0, width, CHARHEIGHT * 2);
    }

    setpen(R,G,B,0,1);
    drawRectangle((width-3)-CHARWIDTH*3, 3, width-3, CHARHEIGHT*2-3);

    drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);

    setpen(R,G,B,0,1);
    drawRectangle(0,0,width-1,CHARHEIGHT*2);
    if(transp==0.01) {
        drawTextF("+", (width-3)-CHARWIDTH*2,ypos, 0,0, R4,G4,B4, 0);
    } else {
        drawTextF("-", (width-3)-CHARWIDTH*2,ypos, 0,0, R4,G4,B4, 0);
    }

    if(animpos==1) {
        x+= xmouse-animindex;
        y+= ymouse-textNumber;
    }
}

void windowDown() {
    if(hotspot((width-3)-CHARWIDTH*3, 3, width-3, CHARHEIGHT*2-3)) {
        if (transp == 0.01) transp = 0;
        else transp = 0.01;
    }
    else if(hotspot(0,0, width, CHARHEIGHT*2)) {
        animpos = 1;
        animindex = xmouse;
        textNumber = ymouse;
    }
}

void windowUp() {
    animpos = 0;
}


void activateWindow(Actor* actor) {
    actor->transp=0;
}

void deactivateWindow(Actor* actor) {
    actor->transp=1;
}

void boundToWindow(Actor* actor) {
    ChangeParent("Event Actor", actor->name);
    if(actor->transp == 1 || actor->transp == 0.01) {
        transp = 1;
        animpos = 0;
    }
    else transp = 0;
}

void menu(int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    erase(R3,G3,B3,0);
    setpen(R,G,B,0,1);
    drawEdges();
    if (animpos==1 || animpos == 2) {
        transp-= GUIOPT.menu == 0? 0.25 : 1;
        if(mouseOver()==1 && animpos != 2) animpos=2;
        if(!mouseOver()==1 && animpos== 2) animpos=0;
    } else transp+= GUIOPT.menu == 0? 0.25 : 1;
}

void activateMenu(Actor* actor) {
    actor->animpos = 1;
}

void menuBoundTo(Actor* actor) {
    ChangeParent("Event Actor", actor->name);
    if(actor->transp==1) animpos=0;
}

void boundToMenu(Actor* actor) {
    ChangeParent("Event Actor", actor->name);
    transp = actor -> transp;
}

void tooltip(int delay, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    // 20,20,20 is minumum RGB, 255,255,255 is max.
    Actor*actor = getclone(name);
    if (animindex>0) width = (strlens(actor->text)*(CHARWIDTH+1)) - (animindex*(CHARWIDTH+1));

    erase(0,0,0,1);
    setpen(R3,G3,B3,0,1);
    drawSolidRectangle(0,0,width,height);
    setpen(R,G,B,0,1);
    drawEdges();


    if (animpos > delay && animindex > 0) {
        drawTextF(text, 3-(CHARWIDTH+1)*(animindex+1),3, 0,0, R4,G4,B4, 0);
        transp-=0.2;
    } else {
        transp=1;
    }

    if(xmouse+(width) > view.width) {
        if(xmouse+(width/2) > view.width) xscreen = xmouse-width;
        else xscreen = xmouse - (xmouse+width - view.width);
    } else xscreen = xmouse+4;
    yscreen = ymouse-height-4;
}

void hasTooltip(Actor* actor, char* T) {
    if(mouseOver() && transp != 1) {
        char buf[256];
        actor->animpos++;
        actor->animindex=strlen(name);
        sprintf(buf, "%s %s", name, T);
        strcpy(actor->text, buf);
    } else {
        char buf[256];
        strncpy(buf, actor->text, strlen(name));
        if (strcmp(buf, name)==0) {
            strcpy(actor->text, "");
            actor->animindex=0;
            actor->animpos=0;
        }
    }
}

void setSlider(Actor*actor, int bt, int ht, int hnum) {
    actor->textNumber = ((actor->width-CHARWIDTH*2-1)/(ht-bt))*(hnum-bt);
}

void slider(long int *var, int bt, int ht, int rnd, char*T, int R, int G, int B, int R2, int G2, int B2, int R3, int G3, int B3, int R4, int G4, int B4) {
    char buf[256];
    int ypos = (height/2) - (CHARHEIGHT/2);
    int xpos = (width/2) - (((strlens(T) * (CHARWIDTH+1))-1)/2);
    double i, i2;
    if (animindex==1) {
        if (xmouse-CHARWIDTH < 0) textNumber = 0;
        else if (xmouse > width-CHARWIDTH) textNumber=width-CHARWIDTH*2-1;
        else textNumber = xmouse-CHARWIDTH;
    }
    i = textNumber;
    i2 = *var;
    *var = lockToGrid((i/(width-CHARWIDTH*2-1))*(ht-bt) + bt, rnd);

    erase(0,0,0,1);
    setpen(R3,G3,B3,0,1);
    drawSolidRectangle(0,height/2-2, width-1,height/2+2);
    setpen(R2,G2,B2,0,1);
    drawRectangle(0,height/2-2, width-1,height/2+2);

    // TEXT DRAWING
    sprintf(buf, "%d", bt);
    drawTextF(buf, 1,ypos, 0,0, R4,G4,B4, 0);
    sprintf(buf, "%d", ht);
    drawTextF(buf, width-(strlen(buf)*(CHARWIDTH+1)),ypos, 0,0, R4,G4,B4, 0);
    if(animindex!=1) {
        drawTextF(T, xpos,ypos, 0,0, R4,G4,B4, 0);
    }
    if(hotspot(textNumber,0,textNumber+CHARWIDTH*2,height) || animindex==1) setpen(R2,G2,B2,0,1);
    else setpen(R3,G3,B3,0,1);
    drawSolidRectangle(textNumber,0, textNumber+CHARWIDTH*2,height-1);
    setpen(R,G,B,0,1);
    drawRectangle(textNumber,0, textNumber+CHARWIDTH*2,height-1);

    if (animindex==1) {
        sprintf(buf, "%d", *var);
        xpos = (width/2) - (((strlens(buf) * (CHARWIDTH+1))-1)/2);
        drawTextF(buf, xpos,ypos, 0,0, R4,G4,B4, 0);
    }
}

void sliderDown() {
    if(hotspot(textNumber,0,textNumber+CHARWIDTH*2,height)) {
        animindex = 1;
    }
}

void sliderUp() {
    animindex = 0;
}

void setCURTHEME(gui_theme setter) {
    int i;
    for(i=0; i<12; i++) CURTHEME[i] = setter[i];
}

