(func find_needle (stack)
    (let ((i 0) (result -1))
        (seq 
            (for (() (< $i (len $stack)) (+= i 1))
                (if (== 
                        (at $stack $i)
                        "needle"
                    )
                    (= $result $i)
                )
            )
            ($result)
        )
    )
)

(print (call :find_needle (array "hay" "junk" "hay" "hay" "moreJunk" "needle" "randomJunk")))