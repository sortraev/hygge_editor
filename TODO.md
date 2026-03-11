# TODO

Stuff I want to implement.

## Editor

* [ ] editor window scrolling
* [ ] delete before cursor (currently have only deleting *under* cursor)
* [ ] line soft-wrapping
* [ ] escape codes
    - backspace
    - arrow keys
    - ?
* [ ] line numbering

## Files

* [ ] saving files
    - [x] IO functionality
    - [ ] save dialogue when filename not specified
* [ ] opening files
    - [ ] IO functionality
    - [ ] load file on startup if filename given and exists (fail on readonly)
* [x] specifying filename on command line
* [ ] dirty flag
    - [ ] dialogue on attempt to quit while dirty

## Misc

* [ ] more efficient handling of lines wrt. line insertion/deletion
    - as of now, upon inserting a new line, all subsequent lines are shifted
        "down" by one (in the array of `StringBuffer`s, that is)
* [ ] vim-like modes?
* [ ] "message" bar, in addition to status bar (can double as command line
    if vim-like normal mode is implemented)
