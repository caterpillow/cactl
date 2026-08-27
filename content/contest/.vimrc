set cin aw ai is et ts=4 sw=4 sts=4 tm=50 nu noeb ru cul bs=2 mouse=a noswf
" set bg=dark  (if the terminal background is dark)
sy on | ino <A-[> <Esc>
for k in split('h j k l o') | exe 'ino <A-'.k.'> <Esc>'.k | exe 'nno <A-'.k.'> '.k | endfor
" Enter after ( { [ inserts the closing bracket on its own line:
" for [a,b] in items({'(':')','{':'}','[':']'}) | exe 'ino '.a.'<CR> '.a.'<CR>'.b.'<Esc>O' | endfor
" F5: save, compile with sanitizers, run on file 'in':
" nno <F5> :w<CR>:!g++ -Wall -fsanitize=address,undefined -g -Og % -o %< && ./%< < in<CR>
" Select region and then type :Hash to hash your selection.
ca Hash w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \| md5sum \| cut -c-6
