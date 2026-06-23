cmake_minimum_required(VERSION 3.10)
project(miniFOAM CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# インクルードパスの設定（src フォルダの中を見に行く）
include_directories(src)

# -------------------------------------------------------------
# 1. 既存の熱伝導アプリ (miniLaplacianFoam)
# -------------------------------------------------------------
add_executable(miniLaplacianFoam 
    applications/miniLaplacianFoam.cpp  # 💡 実際の配置に合わせて階層を修正
    src/fvMesh/fvMesh.cpp
    src/fields/volScalarField.cpp
)

# -------------------------------------------------------------
# 2. 新しいパイプ流メッシュアプリ (miniPipeFlowFoam)
# -------------------------------------------------------------
add_executable(miniPipeFlowFoam 
    applications/miniPipeFlowFoam.cpp   # 💡 実際の配置に合わせて階層を修正
    src/fvMesh/fvCylindricalMesh.cpp
)