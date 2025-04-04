# Diff Details

Date : 2025-03-27 19:19:23

Directory /home/xiaojiang/DEV/SparkPlayer

Total : 36 files,  675 codes, 79 comments, 118 blanks, all 872 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [CMakeLists.txt](/CMakeLists.txt) | CMake | 2 | 0 | 0 | 2 |
| [SparkMediaControler.cpp](/SparkMediaControler.cpp) | C++ | 163 | 6 | 35 | 204 |
| [SparkMediaControler.h](/SparkMediaControler.h) | C++ | 21 | 53 | 9 | 83 |
| [build/.cmake/api/v1/reply/cmakeFiles-v1-456cf497b6b009e2b6b2.json](/build/.cmake/api/v1/reply/cmakeFiles-v1-456cf497b6b009e2b6b2.json) | JSON | 681 | 0 | 1 | 682 |
| [build/.cmake/api/v1/reply/cmakeFiles-v1-e854d4ec5fa28f7bab0b.json](/build/.cmake/api/v1/reply/cmakeFiles-v1-e854d4ec5fa28f7bab0b.json) | JSON | -675 | 0 | -1 | -676 |
| [build/.cmake/api/v1/reply/codemodel-v2-ac85d17328409b4b548e.json](/build/.cmake/api/v1/reply/codemodel-v2-ac85d17328409b4b548e.json) | JSON | -79 | 0 | -1 | -80 |
| [build/.cmake/api/v1/reply/codemodel-v2-cb22e730040005e71c5d.json](/build/.cmake/api/v1/reply/codemodel-v2-cb22e730040005e71c5d.json) | JSON | 79 | 0 | 1 | 80 |
| [build/.cmake/api/v1/reply/directory-.-Debug-4f374bb98d12a43bbcf5.json](/build/.cmake/api/v1/reply/directory-.-Debug-4f374bb98d12a43bbcf5.json) | JSON | -45 | 0 | -1 | -46 |
| [build/.cmake/api/v1/reply/directory-.-Debug-89f038b4553a8e9bc6a7.json](/build/.cmake/api/v1/reply/directory-.-Debug-89f038b4553a8e9bc6a7.json) | JSON | 45 | 0 | 1 | 46 |
| [build/.cmake/api/v1/reply/index-2025-03-19T07-52-22-0266.json](/build/.cmake/api/v1/reply/index-2025-03-19T07-52-22-0266.json) | JSON | -132 | 0 | -1 | -133 |
| [build/.cmake/api/v1/reply/index-2025-03-27T09-11-06-0494.json](/build/.cmake/api/v1/reply/index-2025-03-27T09-11-06-0494.json) | JSON | 132 | 0 | 1 | 133 |
| [build/.cmake/api/v1/reply/target-SparkPlayer-Debug-2eca8a6e3080b9a0c932.json](/build/.cmake/api/v1/reply/target-SparkPlayer-Debug-2eca8a6e3080b9a0c932.json) | JSON | 918 | 0 | 1 | 919 |
| [build/.cmake/api/v1/reply/target-SparkPlayer-Debug-e841cddc53cf559f2909.json](/build/.cmake/api/v1/reply/target-SparkPlayer-Debug-e841cddc53cf559f2909.json) | JSON | -890 | 0 | -1 | -891 |
| [build/CMakeFiles/SparkPlayer.dir/DependInfo.cmake](/build/CMakeFiles/SparkPlayer.dir/DependInfo.cmake) | CMake | 2 | 0 | 0 | 2 |
| [build/CMakeFiles/SparkPlayer.dir/cmake\_clean.cmake](/build/CMakeFiles/SparkPlayer.dir/cmake_clean.cmake) | CMake | 4 | 0 | 0 | 4 |
| [build/CMakeFiles/SparkPlayer\_autogen.dir/AutogenInfo.json](/build/CMakeFiles/SparkPlayer_autogen.dir/AutogenInfo.json) | JSON | 14 | 0 | 0 | 14 |
| [build/CMakeFiles/VerifyGlobs.cmake](/build/CMakeFiles/VerifyGlobs.cmake) | CMake | 6 | 0 | 0 | 6 |
| [build/SparkPlayer\_autogen/mocs\_compilation.cpp](/build/SparkPlayer_autogen/mocs_compilation.cpp) | C++ | 1 | 0 | 0 | 1 |
| [build/compile\_commands.json](/build/compile_commands.json) | JSON | 12 | 0 | 0 | 12 |
| [sparkplayer.cpp](/sparkplayer.cpp) | C++ | 34 | 0 | 6 | 40 |
| [sparkplayer.h](/sparkplayer.h) | C++ | 8 | 0 | 2 | 10 |
| [utils/Codec.cpp](/utils/Codec.cpp) | C++ | 142 | 9 | 14 | 165 |
| [utils/Codec.h](/utils/Codec.h) | C++ | 25 | 8 | 8 | 41 |
| [utils/ImageTools.cpp](/utils/ImageTools.cpp) | C++ | 3 | 0 | -1 | 2 |
| [widgets/MainPage.cpp](/widgets/MainPage.cpp) | C++ | 9 | 1 | 0 | 10 |
| [widgets/MainPage.h](/widgets/MainPage.h) | C++ | 1 | 0 | 0 | 1 |
| [widgets/MediaBox.cpp](/widgets/MediaBox.cpp) | C++ | 14 | 0 | 2 | 16 |
| [widgets/MediaBox.h](/widgets/MediaBox.h) | C++ | 1 | 0 | 2 | 3 |
| [widgets/TimeLine.cpp](/widgets/TimeLine.cpp) | C++ | 43 | 0 | 7 | 50 |
| [widgets/TimeLine.h](/widgets/TimeLine.h) | C++ | 3 | 0 | 0 | 3 |
| [widgets/TitleBar.cpp](/widgets/TitleBar.cpp) | C++ | 10 | 0 | 1 | 11 |
| [widgets/TitleBar.h](/widgets/TitleBar.h) | C++ | -15 | -2 | -3 | -20 |
| [widgets/VideoBox.cpp](/widgets/VideoBox.cpp) | C++ | 62 | 4 | 10 | 76 |
| [widgets/VideoBox.h](/widgets/VideoBox.h) | C++ | 24 | 0 | 11 | 35 |
| [widgets/VolumeBox.cpp](/widgets/VolumeBox.cpp) | C++ | 33 | 0 | 6 | 39 |
| [widgets/VolumeBox.h](/widgets/VolumeBox.h) | C++ | 19 | 0 | 9 | 28 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details