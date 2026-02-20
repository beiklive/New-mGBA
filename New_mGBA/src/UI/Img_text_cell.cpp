#include "UI/Img_text_cell.hpp"


using namespace brls::literals;  // for _i18n

Img_text_cell::Img_text_cell()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/mgba_xml/cell/Img_text_cell.xml");


}

brls::View* Img_text_cell::create() 
{
    return new Img_text_cell();
}

void Img_text_cell::setTitle(std::string title)
{
    this->label->setText(title);
}

void Img_text_cell::setImage(std::string res)
{
    this->image->setImageFromRes(res);
}
