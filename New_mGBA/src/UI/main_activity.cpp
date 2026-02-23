/*
    Copyright 2020-2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "main_activity.hpp"
using namespace brls::literals;  // for _i18n

MainActivity::MainActivity()
{
}

void MainActivity::InitActivity()
{
#if defined(SWITCH)
    this->setBackground("sdmc:/mGBA/backgrounds/pokemon.png");
#else
    this->setBackground("./resources/img/bg2.png");
#endif


}

void MainActivity::setBackground(std::string path)
{
    auto view = this->getContentView();
    view->setBackground(brls::ViewBackground::IMAGE);
    view->backgroundImagePath = path;
}