#include "XMLUI/Pages/AppPage.hpp"

static constexpr float CARD_SIZE     = 290.f;  // 封面图区域边长
static constexpr float CARD_MARGIN   = 2.f;   // 卡片右间距
static constexpr float ROW_H_PAD     = 80.f;   // 卡片行左右留白
static constexpr float SCROLL_HEIGHT = 300.f;  // 横向滚动区高度（卡片+缩放动画留白）

// ============================================================
// GameCard
// ============================================================
GameCard::GameCard(const GameEntry& entry)
    : m_entry(entry)
{
    setAxis(brls::Axis::COLUMN);
    setAlignItems(brls::AlignItems::CENTER);
    setWidth(CARD_SIZE);
    setHeight(CARD_SIZE);
    setMarginRight(CARD_MARGIN);
    setHideHighlightBackground(true);
    setHideClickAnimation(true);

    
    m_titleLabel = new brls::Label();
    m_titleLabel->setText(entry.title.empty() ? "—" : entry.title);
    m_titleLabel->setFontSize(30.f);
    m_titleLabel->setSingleLine(true);
    m_titleLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
    m_titleLabel->setTextColor(nvgRGBA(79, 193, 255, 255));
    m_titleLabel->setVisibility(brls::Visibility::INVISIBLE);

    addView(m_titleLabel);

    m_coverImage = new brls::Image();
    m_coverImage->setCornerRadius(13.f);
    m_coverImage->setBackgroundColor(nvgRGBA(252, 255, 248, 255));
    m_coverImage->setHighlightPadding(3.f);
    m_coverImage->setHideHighlightBackground(true);
    m_coverImage->setShadowVisibility(true);
    m_coverImage->setShadowType(brls::ShadowType::GENERIC);
    m_coverImage->setHighlightCornerRadius(12.f);
    m_coverImage->setFocusable(true);
    m_coverImage->setMarginTop(20.f);
    m_coverImage->setScalingType(brls::ImageScalingType::FIT);
    m_coverImage->setInterpolation(brls::ImageInterpolation::LINEAR);
    if (!entry.cover.empty()) {
        // 有封面：直接显示图片
        m_coverImage->setImageFromFile(entry.cover);
        addView(m_coverImage);
    } else {
        // 无封面：显示默认图片
        m_coverImage->setImageFromFile("romfs:/img/ui/gba-icon-256.png");
        addView(m_coverImage);
    }


    beiklive::swallow(this, brls::BUTTON_A);
    beiklive::swallow(this, brls::BUTTON_X);

    // 手柄 A 键启动
    registerAction("启动", brls::BUTTON_A, [this](brls::View*) {
        if (onActivated) onActivated(m_entry);
        return true;
    }, false, false, brls::SOUND_CLICK);

    registerAction("设置", brls::BUTTON_X, [this](brls::View*) {
        brls::Logger::debug("设置");
        return true;
    });

    // 触屏点击启动
    addGestureRecognizer(new brls::TapGestureRecognizer(this));
    registerClickAction([this](brls::View*) {
        if (onActivated) onActivated(m_entry);
        return true;
    });
}

void GameCard::onChildFocusGained(brls::View* directChild, brls::View* focusedView)
{
    // Called when a child of ours gets focused, in that case it's the Image

    Box::onChildFocusGained(directChild, focusedView);
    m_titleLabel->setVisibility(brls::Visibility::VISIBLE);
    m_focused = true;
    if (onFocused) onFocused(m_entry);
    invalidate();   // 触发 draw() 启动缩放动画
}

void GameCard::onChildFocusLost(brls::View* directChild, brls::View* focusedView)
{
    // Called when a child of ours losts focused, in that case it's the Image

    Box::onChildFocusLost(directChild, focusedView);

    m_titleLabel->setVisibility(brls::Visibility::INVISIBLE);
    setHeight(CARD_SIZE);
    m_focused = false;
    invalidate();   // 触发 draw() 启动恢复动画
}




void GameCard::draw(NVGcontext* vg,
                    float x, float y, float w, float h,
                    brls::Style style, brls::FrameContext* ctx)
{
    // 平滑插值缩放：聚焦 1.08×，失焦 1.0×
    float target = m_focused ? 1.0f : 0.9f;
    float delta  = target - m_scale;
    if (std::abs(delta) > 0.002f) {
        m_scale += delta * 0.3f;
        invalidate();  // 动画尚未结束，继续请求重绘
    } else {
        m_scale = target;
    }

    const float cx = x + w * 0.5f;
    const float cy = y + h * 0.5f;
    nvgSave(vg);
    nvgTranslate(vg,  cx,  cy);
    nvgScale(vg, m_scale, m_scale);
    nvgTranslate(vg, -cx, -cy);
    brls::Box::draw(vg, x, y, w, h, style, ctx);
    nvgRestore(vg);
}


AppPage::AppPage()
{
    setWidth(View::AUTO);
    setHeight(View::AUTO);
    setAxis(brls::Axis::COLUMN);


    setGrow(1.0f);



    // m_InfoRow = new brls::Box(brls::Axis::COLUMN);
    // m_InfoRow->setAlignItems(brls::AlignItems::FLEX_START);
    // // m_InfoRow->setPaddingTop(20.f);
    // m_InfoRow->setPaddingLeft(ROW_H_PAD);
    // m_InfoRow->setPaddingRight(ROW_H_PAD);

    // m_titleLabel = new brls::Label();
    // m_titleLabel->setFontSize(50.f);
    // m_titleLabel->setSingleLine(true);
    // m_titleLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
    // m_titleLabel->setTextColor(nvgRGBA(79, 193, 255, 255));
    // m_titleLabel->setVisibility(brls::Visibility::VISIBLE);

    // m_InfoRow->addView(m_titleLabel);
    // m_InfoRow->setGrow(0.0f);

    // // m_InfoRow->setPaddingBottom(50.f);
    // addView(m_InfoRow);





    // ── 横向滚动区 ──────────────────────────────────────────
    m_scroll = new brls::HScrollingFrame();
    m_scroll->setWidth(View::AUTO);
    m_scroll->setHeight(SCROLL_HEIGHT);
    m_scroll->setGrow(1.0f);
    m_scroll->setScrollingBehavior(brls::ScrollingBehavior::CENTERED);
    m_scroll->setScrollingIndicatorVisible(false);
    // m_scroll->setBackgroundColor(nvgRGBA(31, 31, 31, 50));

    m_cardRow = new brls::Box(brls::Axis::ROW);
    m_cardRow->setAlignItems(brls::AlignItems::CENTER);
    m_cardRow->setPaddingLeft(ROW_H_PAD);
    m_cardRow->setPaddingRight(ROW_H_PAD);

    m_scroll->setContentView(m_cardRow);
    m_scroll->setGrow(0.0f);

    addView(m_scroll);


    // 填充
    auto* box = new brls::Box(brls::Axis::ROW);
    box->setGrow(1.0f);
    addView(box);

}

void AppPage::addGame(const GameEntry& entry)
{
    auto* card = new GameCard(entry);
    card->onFocused   = [this](const GameEntry& e) { onCardFocused(e);   };
    card->onActivated = [this](const GameEntry& e) { onCardActivated(e); };
    m_cardRow->addView(card);
}

void AppPage::setGames(const std::vector<GameEntry>& games)
{
    m_cardRow->clearViews();
    for (const auto& g : games)
        addGame(g);
}

void AppPage::onCardFocused(const GameEntry& entry)
{
    // m_titleLabel->setText(entry.title.empty() ? "—" : entry.title);
    // m_pathLabel->setText(entry.path);
}

void AppPage::onCardActivated(const GameEntry& entry)
{
    if (onGameSelected)
        onGameSelected(entry);
}
