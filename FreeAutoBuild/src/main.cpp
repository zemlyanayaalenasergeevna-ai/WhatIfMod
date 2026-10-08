#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <string>
#include <algorithm>

using namespace geode::prelude;

namespace {
    enum class Style {
        Modern,
        Tech,
        Glow
    };

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
            FLAlertLayer::create(
                "Free AutoBuild",
                "Select one or more objects first.",
                "OK"
            )->show();
            return;
        }

        auto color = colorFor(style);
        auto added = 0u;

        for (unsigned i = 0; i < selected->count(); ++i) {
            auto object = typeinfo_cast<GameObject*>(selected->objectAtIndex(i));
            if (!object) continue;

            auto p = object->getPosition();

            // Four small decoration blocks around each selected object.
            constexpr cocos2d::CCPoint offsets[] = {
                {-24.f, 24.f}, {24.f, 24.f},
                {-24.f, -24.f}, {24.f, -24.f}
            };

            for (auto offset : offsets) {
                auto deco = GameObject::createWithKey(1);
                if (!deco) continue;

                deco->setPosition(p + offset);
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

        FLAlertLayer::create(
            "Free AutoBuild",
            fmt::format("Added {} decoration objects.", added).c_str(),
            "NICE"
        )->show();
    }

    class IdeaPopup : public Popup<CCTextInputNode*> {
    protected:
        bool init(CCTextInputNode* input) {
            if (!Popup::init(360.f, 180.f)) return false;

            this->setTitle("AI Decoration Idea");
            m_input = input;

            auto label = CCLabelBMFont::create(
                "Describe the style you want",
                "goldFont.fnt"
            );
            m_mainLayer->addChildAtPosition(label, Anchor::Top, {0.f, -42.f});

            m_input->setPosition({180.f, 82.f});
            m_input->setString("modern tech");
            m_mainLayer->addChild(m_input);

            auto buttonSprite = ButtonSprite::create("BUILD", 80, true, 0.7f, 0.8f);
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(IdeaPopup::onBuild)
            );
            auto menu = CCMenu::create();
            menu->addChild(button);
            menu->setPosition({180.f, 38.f});
            m_mainLayer->addChild(menu);

            return true;
        }

        void onBuild(CCObject*) {
            auto idea = m_input ? m_input->getString() : "";
            auto editor = LevelEditorLayer::get();
            auto ui = EditorUI::get();

            if (editor && ui) {
                addDecoration(editor, ui->m_selectedObjects, styleFromIdea(idea));
            }

            this->onClose(nullptr);
        }

        CCTextInputNode* m_input = nullptr;

    public:
        static IdeaPopup* create() {
            auto input = CCTextInputNode::create(
                300.f, 30.f, "Describe style...", "chatFont.fnt"
            );
            auto ret = new IdeaPopup();
            if (ret->init(input)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
    };
}

class $modify(FreeAutoBuildEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        this->addChild(menu, 100);

        auto modern = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("MODERN", 90, true, 0.65f, 0.8f),
            this,
            menu_selector(FreeAutoBuildEditorUI::onModern)
        );
        auto tech = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("TECH", 70, true, 0.65f, 0.8f),
            this,
            menu_selector(FreeAutoBuildEditorUI::onTech)
        );
        auto glow = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("GLOW", 70, true, 0.65f, 0.8f),
            this,
            menu_selector(FreeAutoBuildEditorUI::onGlow)
        );
        auto idea = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("AI IDEA", 90, true, 0.65f, 0.8f),
            this,
            menu_selector(FreeAutoBuildEditorUI::onIdea)
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
        IdeaPopup::create()->show();
    }
};
