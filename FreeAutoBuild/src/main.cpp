#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <string>
#include <algorithm>
#include <cctype>

using namespace geode::prelude;

namespace {
    enum class Style { Modern, Tech, Glow };

    Style styleFromIdea(std::string idea) {
        std::transform(idea.begin(), idea.end(), idea.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        if (idea.find("tech") != std::string::npos ||
            idea.find("cyber") != std::string::npos ||
            idea.find("machine") != std::string::npos ||
            idea.find("mechan") != std::string::npos) {
            return Style::Tech;
        }
        if (idea.find("glow") != std::string::npos ||
            idea.find("neon") != std::string::npos ||
            idea.find("space") != std::string::npos ||
            idea.find("energy") != std::string::npos) {
            return Style::Glow;
        }
        return Style::Modern;
    }

    cocos2d::ccColor3B colorFor(Style style) {
        switch (style) {
            case Style::Tech: return {80, 170, 255};
            case Style::Glow: return {180, 90, 255};
            default: return {255, 210, 80};
        }
    }

    void addDecoration(LevelEditorLayer* editor, cocos2d::CCArray* selected, Style style) {
        if (!editor || !selected || selected->count() == 0) {
            FLAlertLayer::create("Free AutoBuild", "Select one or more objects first.", "OK")->show();
            return;
        }

        auto color = colorFor(style);
        unsigned added = 0;
        constexpr cocos2d::CCPoint offsets[] = {
            {-24.f, 24.f}, {24.f, 24.f}, {-24.f, -24.f}, {24.f, -24.f}
        };

        for (unsigned i = 0; i < selected->count(); ++i) {
            auto object = typeinfo_cast<GameObject*>(selected->objectAtIndex(i));
            if (!object) continue;

            for (auto offset : offsets) {
                auto deco = GameObject::createWithKey(1);
                if (!deco) continue;

                deco->setPosition(object->getPosition() + offset);
                deco->setScale(style == Style::Tech ? 0.35f : 0.28f);
                deco->setOpacity(style == Style::Glow ? 155 : 210);
                deco->setObjectColor(color);

                if (style == Style::Glow) {
                    deco->setGlowColor({255, 255, 255});
                    deco->addEmptyGlow();
                }

                editor->addObject(deco);
                ++added;
            }
        }

        auto message = "Added " + std::to_string(added) + " decoration objects.";
        FLAlertLayer::create("Free AutoBuild", message.c_str(), "NICE")->show();
    }
}

class $modify(FreeAutoBuildEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        this->addChild(menu, 100);

        auto modern = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("MODERN", 90, true, 0.65f, 0.8f),
            this, menu_selector(FreeAutoBuildEditorUI::onModern)
        );
        auto tech = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("TECH", 70, true, 0.65f, 0.8f),
            this, menu_selector(FreeAutoBuildEditorUI::onTech)
        );
        auto glow = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("GLOW", 70, true, 0.65f, 0.8f),
            this, menu_selector(FreeAutoBuildEditorUI::onGlow)
        );
        auto idea = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("AI IDEA", 90, true, 0.65f, 0.8f),
            this, menu_selector(FreeAutoBuildEditorUI::onIdea)
        );

        modern->setPosition({60.f, 30.f});
        tech->setPosition({145.f, 30.f});
        glow->setPosition({225.f, 30.f});
        idea->setPosition({330.f, 30.f});

        menu->addChild(modern);
        menu->addChild(tech);
        menu->addChild(glow);
        menu->addChild(idea);

        return true;
    }

    void onModern(CCObject*) {
        addDecoration(LevelEditorLayer::get(), this->m_selectedObjects, Style::Modern);
    }

    void onTech(CCObject*) {
        addDecoration(LevelEditorLayer::get(), this->m_selectedObjects, Style::Tech);
    }

    void onGlow(CCObject*) {
        addDecoration(LevelEditorLayer::get(), this->m_selectedObjects, Style::Glow);
    }

    void onIdea(CCObject*) {
        if (m_ideaInput) {
            m_ideaInput->removeFromParentAndCleanup(true);
            m_ideaInput = nullptr;
            return;
        }

        m_ideaInput = CCTextInputNode::create(
            240.f, 30.f, "e.g. modern tech neon", "chatFont.fnt"
        );
        m_ideaInput->setPosition({330.f, 72.f});
        this->addChild(m_ideaInput, 101);

        auto build = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("BUILD", 70, true, 0.65f, 0.8f),
            this, menu_selector(FreeAutoBuildEditorUI::onIdeaBuild)
        );
        build->setPosition({330.f, 30.f});

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        menu->addChild(build);
        this->addChild(menu, 102);
    }

    void onIdeaBuild(CCObject*) {
        if (!m_ideaInput) return;

        auto idea = m_ideaInput->getString();
        addDecoration(LevelEditorLayer::get(), this->m_selectedObjects, styleFromIdea(idea));

        m_ideaInput->removeFromParentAndCleanup(true);
        m_ideaInput = nullptr;
    }

    CCTextInputNode* m_ideaInput = nullptr;
};
