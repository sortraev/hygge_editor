# TODO

Stuff I want to implement.

## Editor

* [x] editor window scrolling
    - [ ] nice-to-have: scrolloff
* [ ] delete before cursor (currently have only deleting *under* cursor)
* [ ] line soft-wrapping
* [ ] escape codes
    - backspace
    - arrow keys
    - ?
* [ ] line numbering
* [ ] only redraw dirty lines

## Files

* [ ] saving files
    - [x] IO functionality
    - [x] editor shortcut (for now, ctrl+f)
    - [ ] user response on file saved
    - [ ] save dialogue when filename not already specified
* [x] opening files
    - [x] IO functionality
    - [x] load file on startup if filename given and exists (fail on readonly)
* [x] specifying filename on command line
* [ ] dirty flag
    - [x] prevent quit while dirty (but allow force quit)
    - [ ] dialogue on attempt to quit while dirty

## Misc

* [ ] more efficient handling of lines wrt. line insertion/deletion
    - as of now, upon inserting a new line, all subsequent lines are shifted
        "down" by one (in the array of `StringBuffer`s, that is)
* [ ] vim-like modes?
* [x] "message" bar, in addition to status bar (can double as command line
    if vim-like normal mode is implemented)
