set cin aw ai is et ts=4 sw=4 sts=4 ttm=50 nu noeb ru cul bs=2 mouse=a noswf
" set bg=dark  (if the terminal background is dark)
sy on | ino <A-[> <Esc>
for k in split('h j k l o') | exe 'ino <A-'.k.'> <C-o>'.k | exe 'nno <A-'.k.'> '.k | endfor

" F5: save, compile with sanitizers, run (paste the input, then Ctrl-D):
nno <F5> :w<CR>:!g++ -Wall -Wfatal-errors -fsanitize=address,undefined -g -Og % -o %< && ./%<<CR>
nno <F6> :w<CR>:!g++ -Wall -Wfatal-errors -fsanitize=address,undefined -g -Og % -o %< && ./%< < in<CR>
" Select region and then type :Hash to hash your selection.
ca Hash w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \| md5sum \| cut -c-6

" auto-close: ( inserts () with the cursor inside; Enter between {} makes a block
ino ( ()<Left>
ino [ []<Left>
ino {<CR> {<CR>}<Esc>O
" alternative for auto close {
" ino { {}<Left>
" ino <expr> <CR> getline('.')[col('.')-2:col('.')-1]=='{}' ? "\<CR>\<Esc>O" : "\<CR>"
