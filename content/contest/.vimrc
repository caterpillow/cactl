set cin aw ai is et ts=4 sw=4 sts=4 tm=50 nu noeb ru cul
" set bg=dark  (if the terminal background is dark)
sy on | ino <A-[> <Esc>
for k in split('h j k l o') | exe 'ino <A-'.k.'> <Esc>'.k | exe 'nno <A-'.k.'> '.k | endfor
" Select region and then type :Hash to hash your selection.
ca Hash w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \| md5sum \| cut -c-6
