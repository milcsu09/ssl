syntax clear

" Comments, literals
syntax match  sslComment  /#.*/
syntax match  sslNumber   /\v\d+/
syntax match  sslEscape /\\./ contained
syntax region sslString start=/"/ skip=/\\./ end=/"/ contains=sslEscape

" Keywords, constants
syntax keyword sslKeyword  match
syntax keyword sslBoolean  true false otherwise

" Builtins
syntax keyword sslBuiltin  head tail print printf printl printlf error dis

" Lambda
syntax match sslFunction /\v[a-zA-Z_][a-zA-Z0-9_]*\./

" Delimiters
syntax match sslSymbols  /[|,;]/

highlight link sslComment  Comment
highlight link sslNumber   Number
highlight link sslEscape   SpecialChar
highlight link sslString   String
highlight link sslKeyword  Keyword
highlight link sslBoolean  Boolean
highlight link sslBuiltin  Function
highlight link sslFunction Function
highlight link sslSymbols  Delimiter

