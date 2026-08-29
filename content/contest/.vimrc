" :Hash the whole file, :1,.Hash up to the cursor (that matches a printed
" // abc123 prefix marker), or select a region first. C++ only: cpp does
" not know what a comment is in any other language.
com! -range=% Hash <line1>,<line2>w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \| md5sum \| cut -c-6
" <hash>

set cin aw ai is et ts=4 sw=4 sts=4 ttm=50 nu noeb ru cul bs=2 mouse=a noswf
sy on | ino <A-[> <Esc>
for k in split('h j k l o') | exe 'ino <A-'.k.'> <C-o>'.k | exe 'nno <A-'.k.'> '.k | endfor
" <hash>

" F5: save, compile with sanitizers, run (paste the input, then Ctrl-D):
nno <F5> :w<CR>:!g++ -Wall -Wfatal-errors -fsanitize=address,undefined -g -Og % -o %< && ./%<<CR>
nno <F6> :w<CR>:!g++ -Wall -Wfatal-errors -fsanitize=address,undefined -g -Og % -o %< && ./%< < in<CR>
" <hash>

" OPTIONAL:
" set bg=dark  (if the terminal background is dark)
" auto-close: ( inserts () with the cursor inside; Enter between {} makes a block
" ino ( ()<Left>
" ino [ []<Left>
" ino {<CR> {<CR>}<Esc>O
" alternative for auto close {
" ino { {}<Left>
" ino <expr> <CR> getline('.')[col('.')-2:col('.')-1]=='{}' ? "\<CR>\<Esc>O" : "\<CR>"
