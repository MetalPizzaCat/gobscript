; very simple implementation of brainfuck interpreter;

(func lock_to_u8 (val) (if (>= $val 255) (- $val 255) elif (< $val 0) (+ $val 255) else ($val)))

(func find_next_open_bracket (code start_pos direction target_bracket start_bracket)
    (if (or (< $start_pos 0) (>= $start_pos (len $code))) (-1) else 
        (let ((i $start_pos) (counter 1))
            (seq 
                (while 
                    (and 
                        (and 
                            (>= $i 0) 
                            (< $i (len $code))
                        ) 
                        (>= $counter 1)
                    )
                    (seq
                        (if (== (at $code $i) $target_bracket)
                            (-= $counter 1) 
                        elif (== (at $code $i) $start_bracket)
                            (+= $counter 1) 
                        )
                        (if (!= $counter 0) (+= $i $direction))
                        
                    )
                )
                (if 
                    (or 
                        (< $i 0) 
                        (>= $i (len $code))
                    ) 
                    (-1) 
                else 
                    $i
                )
            )
        )
    )
)

(let ((code ",.") (memory (array_of_size 30)) (mem_ptr 0) (code_ptr 0) (err 0))
    (seq 
        (while (and (< $code_ptr (len $code)) (== $err 0))
            (seq
                (let ((curr (at $code $code_ptr)))
                    (if (== $curr "+")
                        (set $memory $mem_ptr (call :lock_to_u8 (+ (at $memory $mem_ptr) 1)))
                    elif (== $curr "-")
                        (set $memory $mem_ptr (call :lock_to_u8 (- (at $memory $mem_ptr) 1)))
                    elif (== $curr ">")
                        (seq 
                            (+= $mem_ptr 1)
                            (if (> $mem_ptr (len $memory)) (= $err "Pointer out of bounds"))
                        )
                    elif (== $curr "<")
                        (seq 
                            (-= $mem_ptr 1)
                            (if (< $mem_ptr 0) (= $err "Pointer out of bounds"))
                        )
                    elif (== $curr ",")
                        (set $memory $mem_ptr (ord (at (input) 0)))
                    elif (== $curr ".")
                        (print (chr (at $memory $mem_ptr))) 
                    elif (== $curr "[")
                        (if (== (at $memory $mem_ptr) 0) 
                            (let ((temp (call :find_next_open_bracket $code (+ $code_ptr 1) 1 "]" "[")))
                                (if (== $temp -1) (= $err "Code pointer out of bounds") else (= $code_ptr $temp))
                            )
                        )
                    elif (== $curr "]")
                        (if (!= (at $memory $mem_ptr) 0) 
                            (let ((temp (call :find_next_open_bracket $code (- $code_ptr 1) -1 "[" "]")))
                                (if (== $temp -1) (= $err "Code pointer out of bounds") else (= $code_ptr $temp))
                            ) 
                        )
                    )
                )
                (+= $code_ptr 1)
            )
        )
    (if (!= $err 0) (print "Error: " $err))
    )
)