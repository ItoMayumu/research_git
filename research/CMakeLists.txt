# 1. CMakeの最小バージョンとプロジェクト名
cmake_minimum_required(VERSION 3.10)
project(miniFOAM)

# 2. C++のバージョン指定（C++11以上を指定しておくと安全です）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 3. ライブラリ（3種の神器＋fvm）のソースファイルをまとめる
set(MINIFOAM_SRC
    src/fvMesh/fvMesh.cpp
    src/fields/volScalarField.cpp
    src/fvMatrix/fvMatrix.cpp
    src/finiteVolume/fvm.cpp
)

# 4. 上記のソースから「libminifoam」というライブラリ（部品の塊）を作る
add_library(minifoam_lib ${MINIFOAM_SRC})

# 5. メインプログラム（実行ファイル）を作る
add_executable(miniLaplacianFoam applications/miniLaplacianFoam.cpp)

# 6. メインプログラムに、さっき作ったライブラリ（部品）を合体（リンク）させる
target_link_libraries(miniLaplacianFoam minifoam_lib)