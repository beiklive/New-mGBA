#include "UI/List_view.hpp"
#include <vector>
using namespace brls::literals;  // for _i18n

std::vector<ImgTextCell> listItems;

RecyclerCell::RecyclerCell()
{
    this->inflateFromXMLRes("xml/mgba_xml/cell/Img_text_cell.xml");
}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}


// DATA SOURCE

int DataSource::numberOfSections(brls::RecyclerFrame* recycler)
{
    return 1;
}

int DataSource::numberOfRows(brls::RecyclerFrame* recycler, int section)
{
    return listItems.size();
}
    
std::string DataSource::titleForHeader(brls::RecyclerFrame* recycler, int section) 
{
    // if (section == 0)
    return "";
    // return "Section #" + std::to_string(section+1);
}

RecyclerCell* DataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    RecyclerCell* item = (RecyclerCell*)recycler->dequeueReusableCell("Cell");
    item->label->setText(listItems[indexPath.row].text);
    item->image->setImageFromRes(listItems[indexPath.row].imageRes);
    return item;
}

void DataSource::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    // 暂时使用ListView来展示被点击的item，后续可以改成更合适的界面
    recycler->present(new ListView());
    brls::Logger::info("Item Index(" + std::to_string(indexPath.section) + ":" + std::to_string(indexPath.row) + ") selected.");
}






ListView::ListView()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/mgba_xml/view/list_view.xml");

    listItems.clear();
    for(int i = 0; i < 20; i++) {
        listItems.push_back(ImgTextCell("Item " + std::to_string(i), "img/ui/folder.png"));
    }

    recycler->estimatedRowHeight = 70;
    recycler->registerCell("Cell", []() { return RecyclerCell::create(); });
    recycler->setDataSource(new DataSource());

    this->getView("list_view_root")->setHighlightAlphaTransparent(true, 0.0f); // 取消选中高亮背景
}

brls::View* ListView::create() 
{
    return new ListView();
}

void ListView::clearItems()
{
    if (listItems.empty())
        return;

    listItems.clear();
}

void ListView::addItem(std::string title, std::string imageRes)
{
    listItems.push_back(ImgTextCell(title, imageRes));
}
