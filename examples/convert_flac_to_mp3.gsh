(func is_correct_file (path)
    (and
        (is_file $path)
        (== (filename_suffix $path) ".flac")
    )
)
(let (
        (root_folder "./flac_folder/") 
        (dest_folder "./dest/")
        (dir_contents 0)
        (i 0)
    )
    (seq
        (= $dir_contents (filter (listdir $root_folder) :is_correct_file))
        (print $dir_contents)
        (print (listdir $root_folder))

        (for (() (< $i (len $dir_contents)) (+= $i 1))
            (seq
                (print (filename (at $dir_contents $i)))
                (exec ffmpeg
                    -i (+ $root_folder (filename (at $dir_contents $i)))
                    -map_metadata 0
                    -id3v2_version 3
                    (+ (+ $dest_folder (filename_stem (at $dir_contents $i)) ) ".mp3") )
            )
        )
    )
)