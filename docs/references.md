The following references can be used to help with the decomp:

# RVL_SDK
- Any game, preferably released around late 2009 with a symbol map, can be used for the SDK. This extends to the HOME button menu library.
    - Note that games with DWARF v2 might not contain all type data from the SDK.

# NintendoWare
- For NintendoWare, third-party games are not recommended as their implementations of NintendoWare are often lightly or heavily edited to match each studio's workflow. This sometimes extends to second-party studios such as HAL Laboratory. The exception to this is nw4hbm.

# EGG
- *Big Brain Academy: Wii Degree* was the primary base for a lot of EGG work, though the library was heavily updated sometime between March 2007 (when the game was built) and July 2007 (when revision 1 of *Wii Sports* was released). As such, while you may use the *Wii Sports* decomp as a reference for EGG, please note that the assertions were stubbed out in the Pack Project beginning with *Wii Fit* and that manual analysis is required in many cases. If you have any questions, you can contact [https://github.com/vabold Vabold], as he is pretty familiar with the library.

# Pack Project
- The *Wii Sports* decomp and *Wii Fit U* can be used as sources for symbols and such. While *Wii Fit U* is especially helpful due to it sharing a lot of the same game logic, note that there are several games not included in *Wii Fit U* that, therefore, require manual symbol name-cracking. They are as follows:
    - Big Top Juggling
    - Lotus Focus
    - Penguin Slide
    - Segway Circuit
    - Skateboard Arena
    - Snowboard Slalom
    - Tightrope Walk
    - Rhythm Parade
        - Basic Run Plus is technically also missing, though the only real difference between it and Basic Run is the addition of the ending quiz.

- In general, also note that the Pack Project library itself was heavily updated for the Wii U, so be sure to examine the code carefully.
