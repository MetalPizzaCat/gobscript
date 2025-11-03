;simple example code that takes all files in a directory, skipping folders, and prints them line-by-line ; 

(let (
        (folder "./")
        (i 0)
    )
    (seq
        ; collect only correct files we are using `is_file` function which takes in a path and returns 1 or 0 ;
        (= dir_contents (filter (listdir $folder) :is_file))

        (for (() (< $i (len $dir_contents)) (+= i 1))
            (seq
                (print (filename (at $dir_contents $i)))
            )
        )
    )
)