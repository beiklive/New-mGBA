#pragma once

#include <borealis.hpp>

#include <string>


class ImgTextCell
{
  public:
    std::string text;
    std::string imageRes;

    ImgTextCell(std::string text, std::string imageRes)
        : text(text)
        , imageRes(imageRes)
    {
    }
};



class RecyclerCell
    : public brls::RecyclerCell
{
  public:
    RecyclerCell();

    void onFocusGained() override;
    void onFocusLost() override;



    BRLS_BIND(brls::Rectangle, accent, "brls/sidebar/item_accent");
    BRLS_BIND(brls::Label, label, "title");
    BRLS_BIND(brls::Image, image, "image");

    static RecyclerCell* create();
};


class DataSource
    : public brls::RecyclerDataSource
{
  public:
    int numberOfSections(brls::RecyclerFrame* recycler) override;
    int numberOfRows(brls::RecyclerFrame* recycler, int section) override;
    RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath index) override;
    void didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath) override;
    std::string titleForHeader(brls::RecyclerFrame* recycler, int section) override;
};


class ListView : public brls::Box
{
  public:
    ListView();

    static brls::View* create();

    
    BRLS_BIND(brls::RecyclerFrame, recycler, "recycler");

    void addItem(std::string title, std::string imageRes);
    void clearItems();
    void applyItems();
    
};
