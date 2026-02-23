#include "UI/List_view.hpp"
#include <vector>


#include "Utils/fileUtils.hpp"
#include "Utils/strUtils.hpp"


using namespace brls::literals;  // for _i18n


RecyclerCell::RecyclerCell()
{
    this->inflateFromXMLRes("xml/mgba_xml/cell/Img_text_cell.xml");
    this->getView("img_text_cell_root")->setHighlightAlphaTransparent(true, 0.0f); // 取消选中高亮背景

}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}


void RecyclerCell::onFocusGained()
{
    brls::RecyclerCell::onFocusGained();

    this->accent->setVisibility(brls::Visibility::VISIBLE);
}

void RecyclerCell::onFocusLost()
{
    brls::RecyclerCell::onFocusLost();

    this->accent->setVisibility(brls::Visibility::INVISIBLE);
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
    RecyclerCell* item = (RecyclerCell*)recycler->dequeueReusableCell(cell_name);
    item->label->setText(listItems[indexPath.row].text);

    auto theme = brls::Application::getPlatform()->getThemeVariant();
    switch (theme)
    {
        case brls::ThemeVariant::LIGHT:
            item->image->setImageFromRes(listItems[indexPath.row].imageRes + "_light.png");
            break;
        case brls::ThemeVariant::DARK:
            item->image->setImageFromRes(listItems[indexPath.row].imageRes + "_dark.png");
            break;
    }
    return item;
}

// 处理点击事件
void DataSource::didSelectRowAt(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
    if(cell_name == CELL_NAME_HOME) {
        if(indexPath.row == 0) {
            FileListView* fileListView = new FileListView();
            recycler->present(fileListView);
        }
    }
    else if(cell_name == CELL_NAME_FILE) {
        // 点击了文件列表中的某个文件，暂时直接打印日志，后续可以展示文件详情等界面 
            brls::Logger::info("File selected: " + listItems[indexPath.row].text);
        }
    else  if(cell_name == CELL_NAME_SETTINGS) {
        brls::Logger::info("Item selected: " + listItems[indexPath.row].text);
    }

    
    brls::Logger::info("Item Index(" + std::to_string(indexPath.section) + ":" + std::to_string(indexPath.row) + ") selected.");
}

void DataSource::setCellName(std::string name)
{
    this->cell_name = name;
}





ListView::ListView()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/mgba_xml/view/list_view.xml");

    dataSource = new DataSource();

    brls::Logger::debug("ListView created.");

}

ListView::~ListView()
{
    brls::Logger::debug("ListView destroyed.");
}


// brls::View* ListView::create() 
// {
//     return new ListView();
// }

void ListView::setCellName(std::string name)
{
    cell_name = name;
}

void ListView::applyItems()
{
    // recycler->reloadData();
    recycler->estimatedRowHeight = 70;
    recycler->registerCell(cell_name, []() { return RecyclerCell::create(); });
    dataSource->setCellName(cell_name);

    recycler->setDataSource(dataSource);
}
void ListView::clearItems()
{
    if (dataSource->listItems.empty())
        return;

    dataSource->listItems.clear();
}
void ListView::addItem(std::string title, std::string imageRes)
{
    dataSource->listItems.push_back(ImgTextCell(title, imageRes));
}


HomeMenuListView::HomeMenuListView()
{
    this->setCellName(CELL_NAME_HOME);
    this->clearItems();
    this->addItem("beiklive/select/file"_i18n, "img/ui/folder");
    this->addItem("beiklive/select/recent"_i18n, "img/ui/history");
    this->addItem("beiklive/select/favorites"_i18n, "img/ui/bookmark");
    this->applyItems();

}

HomeMenuListView::~HomeMenuListView()
{
}

brls::View* HomeMenuListView::create()
{
    return new HomeMenuListView();
}



FileListView::FileListView()
{
    this->setCellName(CELL_NAME_FILE);

#if defined(SWITCH)
std::vector<std::string> files = beiklive::file::listDir("/");
#else
std::vector<std::string> files = beiklive::file::listDir("/Users/beiklive/Downloads");
#endif

    this->clearItems();

    for (const std::string& file : files) {
        std::string fileName = beiklive::string::extractFileName(file);
        switch(beiklive::file::getPathType(file)) {
            case beiklive::file::PathType::Directory:
                this->addItem(fileName, "img/ui/folder");
                break;
            case beiklive::file::PathType::File:
                {
                    std::string suffix = beiklive::string::getFileSuffix(fileName);
                    if(beiklive::string::iequals(suffix, "gba"))
                    {
                        this->addItem(fileName, "img/file/gba");
                    }
                    else if(beiklive::string::iequals(suffix, "gb"))
                    {
                        this->addItem(fileName, "img/file/gb");
                    }
                    else if(beiklive::string::iequals(suffix, "zip"))
                    {
                        this->addItem(fileName, "img/file/zip");
                    }
                    else if(beiklive::string::iequals(suffix, "png") || 
                            beiklive::string::iequals(suffix, "jpg") || 
                            beiklive::string::iequals(suffix, "jpeg") || 
                            beiklive::string::iequals(suffix, "gif"))
                    {
                        this->addItem(fileName, "img/file/image");
                    }
                    else
                    {
                        this->addItem(fileName, "img/file/file");
                    }
                }
                break;
            default:
                this->addItem(fileName, "img/file/file");
                break;
        }
    }

    this->applyItems();
    brls::Logger::debug("FileListView created.");
}

FileListView::~FileListView()
{
    brls::Logger::debug("FileListView destroyed.");
}

brls::View* FileListView::create()
{
    return new FileListView();
}