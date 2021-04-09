# Lyfoes game solver

This was programmed on a phone IDE, so the code was optimized for minimum swiping when writing rather than, well, being good :)


Usage example:

    ./main <board>

where `<board>` is a description of the board in the following format:
a list of `<vial>`s separated by either `','` or `'|'` (using `'|'` adds a newline when drawing the board)

Each `<vial>` is just 4 *case sensitive* letters representing colors, or spaces for empty locations in the vials.

Each letter must appear exactly 4 times.

**NOTE** that you must include the empty vials as well! Meaning you'd usually end the `<board>` with something like `"    ,    "` (see example)

Although you can use any letter you want to represent any color on the board, the following letters are printed with the following colors for your convenience:


| Letter  | color          | 
|:--------|:---------------|
| `y`     | `y`ellow       |
| `p`     | light `p`urple |    
| `P`     | dark `P`urple  |   
| `Y`     | gra`Y`         |
| `b`     | light `b`lue   |  
| `B`     | dark `B`lue    | 
| `g`     | light `g`reen  |   
| `G`     | dark `G`reen   |  
| `o`     | `o`range       |
| `w`     | `w`hite        |
| `r`     | `r`ed          |
| `c`     | `c`yan         |
| `k`     | pin`k`         |


Example:

    ./main "wppg,rkgk,BcGr,bGBB,Gbwp,YbPo,brYc,ocwy|gYgP,wPpc,yoBY,okky,GyPr,    ,    "
