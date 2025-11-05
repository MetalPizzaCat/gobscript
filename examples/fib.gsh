(func fib (n)
    (if (<= $n 1) ($n) else ( + 
            (call :fib (- $n 1))
            (call :fib (- $n 2))
        )
    )
)

(print (call :fib 20))