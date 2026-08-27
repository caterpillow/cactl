set cin aw ai is et ts=4 sw=4 sts=4 ttm=50 nu noeb ru cul bs=2 mouse=a noswf
" set bg=dark  (if the terminal background is dark)
sy on | ino <A-[> <Esc>
for k in split('h j k l o') | exe 'ino <A-'.k.'> <Esc>'.k | exe 'nno <A-'.k.'> '.k | endfor
" VS Code style auto-close: ( inserts (), typing ) over a ) steps past it,
" Enter between {} makes a block
ino ( ()<Left>
ino [ []<Left>
ino { {}<Left>
ino <expr> ) getline('.')[col('.')-1]==')' ? "\<Right>" : ')'
ino <expr> ] getline('.')[col('.')-1]==']' ? "\<Right>" : ']'
ino <expr> } getline('.')[col('.')-1]=='}' ? "\<Right>" : '}'
ino <expr> <CR> getline('.')[col('.')-2:col('.')-1]=='{}' ? "\<CR>\<Esc>O" : "\<CR>"
" F5: save, compile with sanitizers, run (paste the input, then Ctrl-D):
nno <F5> :w<CR>:!g++ -Wall -fsanitize=address,undefined -g -Og % -o %< && ./%<<CR>
nno <F6> :w<CR>:!g++ -Wall -fsanitize=address,undefined -g -Og % -o %< && ./%< < in<CR>
" Select region and then type :Hash to hash your selection.
ca Hash w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \| md5sum \| cut -c-6
