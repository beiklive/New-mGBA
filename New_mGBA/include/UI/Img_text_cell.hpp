#pragma once

#include <borealis.hpp>

class Img_text_cell : public brls::Box
{
  public:
    Img_text_cell();

    BRLS_BIND(brls::Label, label, "title");
    BRLS_BIND(brls::Image, image, "image");


    void setTitle(std::string title);
    void setImage(std::string res);

    static brls::View* create();
};
