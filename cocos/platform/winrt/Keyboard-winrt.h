﻿/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#pragma once

#include <agile.h>

NS_CC_BEGIN

ref class KeyBoardWinRT sealed
{
public:
    KeyBoardWinRT();

    void ShowKeyboard(Platform::String^ text);
    void HideKeyboard(Platform::String^ text);

internal:
    KeyBoardWinRT(Windows::UI::Core::CoreDispatcher^ dispatcher, Windows::UI::Xaml::Controls::Panel^ panel);

private:

    void QueueKeyEvent(Cocos2dKeyEvent key);
    void OnKeyPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ args);
    void OnKeyReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ args);
    void OnTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ args);

    Platform::Agile<Windows::UI::Core::CoreDispatcher> m_dispatcher;
    Platform::Agile<Windows::UI::Xaml::Controls::Panel> m_panel;
    Windows::UI::Xaml::Controls::TextBox^ m_textBox;
};

NS_CC_END
