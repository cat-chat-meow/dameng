"added by check-c"
set nocompatible
set backspace=indent,eol,start
set tabstop=4
set shiftwidth=4
set softtabstop=4
set cindent
set ai
set paste
set smartindent
set smarttab
set ruler
set showcmd
set hlsearch
set incsearch
set expandtab
syntax on
filetype plugin indent on
set viminfo='20,\"50
set history=50
" Only do this part when compiled with support for autocommands
if has("autocmd")
    " In text files, always limit the width of text to 78 characters
    autocmd BufRead *.txt set tw=78
    " When editing a file, always jump to the last cursor position
    autocmd BufReadPost *
        \ if line("'\"") > 0 && line ("'\"") <= line("$") |
        \   exe "normal g'\"" |
        \ endif
endif
set bg=dark
set nu
" imap <c-w> <Esc>

"------------------------------------------------------------------------------
""获取当前路径的上一级的路径
function! GET_UP_PATH(dir)
    let pos=len(a:dir)-1
    while pos>0
        if (a:dir[pos]=="/" )
            return  strpart(a:dir,0,pos)
        endif
        let pos=pos-1
    endwhile
    return  ""
endfunction

"设置相关tags
function! s:SET_TAGS()
    let dir = expand("%:p:h") "获得源文件路径
        "在路径上递归向上查找tags文件 
        while dir!=""
            if findfile("tags",dir) !=""
                "找到了就加入到tags
                 exec "set tags+=" . dir . "/tags"
             endif
            "得到上级路径
            let dir=GET_UP_PATH(dir)
        endwhile
    endfunction

autocmd BufEnter * call s:SET_TAGS()

set cursorline
"set textwidth=80
"autocmd BufWinEnter * call matchadd('ErrorMsg', '\%>'.&l:textwidth.'v.\+', -1)

set encoding=utf-8
set mouse=r
if has("mouse_sgr")
    set ttymouse=sgr
else
    set ttymouse=xterm2
endif
