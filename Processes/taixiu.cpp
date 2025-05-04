#include "taixiu.h"

void playTaiXiu(const std::vector<std::string>& args) {
    if (args.size() != 1 || (args[0] != "tai" && args[0] != "xiu")) {
        std::cerr << "Usage: taixiu <tai|xiu>" << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 6);

    int d1 = dis(gen), d2 = dis(gen), d3 = dis(gen);
    int sum = d1 + d2 + d3;

    std::cout << "Dice: " << d1 << ", " << d2 << ", " << d3 << " => Sum: " << sum << std::endl;

    bool isTai = sum > 10;
    if ((isTai && args[0] == "tai") || (!isTai && args[0] == "xiu")) {
        std::cout << "Bạn thắng!" << std::endl;
    } else {
        std::cout << "Bạn thua!" << std::endl;
    }
}
