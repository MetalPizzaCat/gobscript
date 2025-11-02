#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <map>
#include <variant>
#include <string.h>
#include <optional>

#include "Pigeon/State.hpp"
#include "Pigeon/Parser.hpp"

#include "GobScriptHelper/StandardFunctions.hpp"

/*
(func f1 (a) (print $a $a))
(func action (callback a)
    (let ((i 0))
        (for (() (< $i 10) (+= i 1))
             (call $callback $a)
        )
    )
)
(func even (val) (if (== 0 (% $val 2)) (1) else (0)))
(call :action :f1 3)
(print (filter (array 1 2 3 4 5 6) :even))

*/

/*
(func is_correct_file (path ext)
    (and )
    (== (get_suffix path ) ext)
)
(let ((root_folder "./1997 - Fallout (The Soundtrack)") (dest_folder "./dest") (dir_contents 0))
    (= dir_contents )
)
(print (listdir ./))
*/

bool f1()
{
    std::cout << "f1" << std::endl;
    return false;
}
int main(int, char **)
{
    std::string program = R"GOB(
(func is_correct_file (path) 
    (and 
        (is_file $path) 
        (== (filename_suffix $path) ".flac")
    )
)
(let (
        (root_folder "./1997 - Fallout (The Soundtrack)/")
        (dest_folder "./dest/")
        (dir_contents 0) 
        (i 0)
    )
    (seq 
        (= dir_contents (filter (listdir $root_folder) :is_correct_file))
        (print $dir_contents)
        (print (listdir $root_folder))
    
        (for (() (< $i (len $dir_contents)) (+= i 1))
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

)GOB";
    std::string::const_iterator it = program.begin();
    try
    {
        using namespace GobScriptHelper;
        State state({nativePrintLineFunction,
                     nativeLenFunction,
                     nativeArrayFilter,
                     nativeMapArray,
                     nativeListDirectory,
                     nativeGetFileNameSuffix,
                     nativeIsDirectory,
                     nativeIsFile,
                     nativeAppend,
                     nativeAt,
                     nativeGetFileName,
                     nativeGetFileNameStem});
        std::vector<std::unique_ptr<Action>> prog = parseTopLevelDeclarations(it, program.end());
        for (auto const &act : prog)
        {
            act->execute(state);
        }
    }
    catch (ParsingError e)
    {
        std::cerr << "Code error at symbol " << (e.getIterator() - program.begin()) << " :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
