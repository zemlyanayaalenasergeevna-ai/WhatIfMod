#include <Geode/Bindings.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <array>
#include <random>
#include <string>
using namespace geode::prelude;

namespace {
std::string randomWhatIf() {
    static constexpr std::array<const char*, 12> questions = {
        "What if gravity disappeared for exactly 1 second?",
        "What if humans could see Wi-Fi signals?",
        "What if the Moon suddenly doubled in size?",
        "What if every clock on Earth stopped for 10 minutes?",
        "What if your memories could be copied into another person?",
        "What if Earth lost 1% of its oxygen every second?",
        "What if everyone could hear their own future thoughts?",
        "What if the Sun became blue tomorrow morning?",
        "What if every lie became physically visible?",
        "What if time moved twice as fast for everyone except you?",
        "What if computers became conscious for exactly one hour?",
        "What if the ocean suddenly became completely transparent?"
    };
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> pick(0, questions.size() - 1);
    return questions[pick(rng)];
}
}

class $modify(WhatIfMenuLayer, MenuLayer) {
    void onMoreGames(CCObject* sender) {
        auto question = randomWhatIf();
        FLAlertLayer::create("WHAT IF?", question.c_str(), "THINK")->show();
    }
};
