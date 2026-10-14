#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main() {
    fs::path caseDir = "cases/packedBed/paramStudy/rho_constant/case4_lipinski_a_nodrag";

    // 最大の時刻ディレクトリを探す
    double maxT = 0;
    fs::path latest;

    for (const auto& entry : fs::directory_iterator(caseDir)) {
        const std::string name = entry.path().filename().string();
        double t;
        try {
            std::size_t pos = 0;
            t = std::stod(name, &pos);
            if (pos != name.size()) continue;   // "5.foam" のような部分一致を除外
        } catch (const std::exception&) {
            continue;                           // "constant", "system"
        }
        if (t > maxT) {
            maxT = t;
            latest = entry.path();
        }
    }

    fs::path file = latest / "alpha.air";
    std::cout << file << std::endl;

    // ファイルを開く
    std::ifstream ifs(file);
    if (!ifs) {
        std::cerr << "cannot open " << file << std::endl;
        return 1;
    }

    std::string line;

    // internalField の行まで読み飛ばす
    while (std::getline(ifs, line)) {
        if (line.find("internalField") != std::string::npos) break;
    }

    // セル数
    std::getline(ifs, line);
    int n = std::stoi(line);

    // "(" の行を読み捨てる
    std::getline(ifs, line);

    // 値を n 個読む
    std::vector<double> alpha;
    alpha.reserve(n);
    for (int i = 0; i < n; i++) {
        std::getline(ifs, line);
        alpha.push_back(std::stod(line));
    }

    std::cout << alpha.size() << " " << alpha[1000] << std::endl;
}