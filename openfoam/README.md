# openfoam フォルダ構成

```
openfoam/
└── lipinski/                      # Lipinski相関(充填層の二相流モデル)関連の作業一式
    ├── calculation/                # 相関式・境界線の数値計算(C++)
    │   ├── darcyMethod.cpp / darcyMethod        # Darcy則によるアプローチ
    │   ├── lipinski.cpp / lipinski              # Lipinskiモデル本体の計算
    │   └── flowRegimeMap/                        # 流動様式線図(flow regime map)関連
    │       ├── lineA.cpp〜lineD.cpp               # 各遷移線の計算
    │       ├── mishima.cpp / mishima              # Mishima相関
    │       ├── churntoannular.cpp                 # churn→annular遷移
    │       ├── slugtocharn.cpp / slugtocharn       # slug→churn遷移
    │       └── result.csv, transition_boundary*.csv  # 計算結果CSV
    │
    ├── solvers/                    # OpenFOAM用カスタムソルバ
    │   └── myTwoPhaseEulerFoam/    # twoPhaseEulerFoamの改造版
    │       ├── myTwoPhaseEulerFoam.C              # ソルバ本体
    │       ├── *.H (createFields, EEqns, CourantNos 等)  # 各種インクルードファイル
    │       ├── pU/, pUf/                           # 圧力-速度連成の定式化違い(2種)
    │       └── Make/                               # wmakeビルド設定(files, options)
    │
    └── cases/                       # OpenFOAM計算ケース(シミュレーション本体)
        ├── packedBedFlow/            # メインの充填層流動解析ケース
        │   ├── 0/, 0.orig/           # 初期条件
        │   ├── 0.499524 〜 5.0006413215/  # 各時刻の計算結果(タイムステップディレクトリ)
        │   ├── constant/             # 物性・メッシュ・重力等の設定
        │   ├── system/               # blockMeshDict, controlDict, fvSchemes等の計算設定
        │   └── uniform/, *.foam      # ParaView用ケースファイル等
        ├── packedBed/standard/       # 充填層の標準ケース(メッシュ生成寄り?)
        ├── slipPackedBedFlow/packedBedFlow/  # すべり速度(slip)を考慮したケース
        └── updateLipinskiPacked/     # Lipinski相関を更新した版のケース
```

## 補足
- OpenFOAMの典型的なケース構造(`0/`時刻初期条件、`constant/`物性・メッシュ、`system/`計算設定)に従っている。
- `dynamicCode/` はOpenFOAMが実行時に自動生成するコンパイル済みコード(コーディングされた境界条件など)のキャッシュで、ビルド成果物のため上記構成図からは除外している。
