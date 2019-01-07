set clipboard=exclude:.*
set hlsearch
set nocompatible



set statusline=%f%=(%{&ff})\ row:%l/%L\ col:%v\ [%p%%]
set laststatus=2
nnoremap <F7>  :ls<CR>:buffer<Space>
nnoremap <F9>  :cs f s<Space>
nnoremap <F10> :cs f g<Space>
nnoremap <C-n> :tabn<CR>
nnoremap <C-p>  :tabp<CR>
syntax on
filetype on

set nocscopeverbose

nmap <C-\>s :cs find s <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>g :cs find g <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>c :cs find c <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>t :cs find t <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>e :cs find e <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>f :cs find f <C-R>=expand("<cword>")<CR><CR>
nmap <C-\>i :cs find i ^<C-R>=expand("<cword>")<CR>$<CR>
nmap <C-\>d :cs find d <C-R>=expand("<cword>")<CR><CR>

if has("cscope")
    set csprg=/usr/bin/cscope
    set csto=1
    set cst
    "set csverb
    set cspc=3
    "add any database in current dir
    if filereadable("cscope.out")
        cs add cscope.out
    "else search cscope.out elsewhere
    else
       let cscope_file=findfile("cscope.out", ".;")
        "echo cscope_file
        if !empty(cscope_file) && filereadable(cscope_file)
            exe "cs add" cscope_file
        endif      
     endif
endif
