/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
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

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <android/log.h>
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif


USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    log("initializing firebase");

    //JniHelper::getEnv();
    //JniHelper::getActivity();

    const auto fbase = ::firebase::App::Create(firebase::AppOptions(),JniHelper::getEnv(),JniHelper::getActivity());
    firebase::analytics::Initialize(*fbase);

    log("enabling data collection");
    firebase::analytics::SetAnalyticsCollectionEnabled(true);

    log("minimum session duration add");
    firebase::analytics::SetMinimumSessionDuration(1000*10); //10 sec

    log("time out duration");
    firebase::analytics::SetSessionTimeoutDuration(1000*60*30);

    log("enabling data collection");
    firebase::analytics::SetAnalyticsCollectionEnabled(true);

    log("getting future result");
    firebase::analytics::SetAnalyticsCollectionEnabled(true);

    futureResult = firebase::analytics::GetAnalyticsInstanceId();
    if (futureResult.status() == firebase::kFutureStatusComplete) {
        log("Analytics Instance ID %s", futureResult.result()->c_str());
    } else {
        log("ERROR: Failed to fetch Analytics Instance ID %s (%d)",
            futureResult.error_message(), futureResult.error());
    }

    auto midButton = ui::Button::create("HelloWorld.png");
    midButton->addClickEventListener([this](Ref* sender){
        firebase::analytics::LogEvent(firebase::analytics::kEventPostScore, firebase::analytics::kParameterScore, 874);
        firebase::analytics::LogEvent("logging_event_without_int", "value_int",45);
        firebase::analytics::LogEvent("logging_event_without_"
                                      "float", "value_float",45);
        log("logging event done");
    });

    midButton->setPosition(Vec2(Director::getInstance()->getVisibleSize().width * 0.5f,
                                Director::getInstance()->getVisibleSize().height * 0.5f) );
    this->addChild(midButton);
    totalMillisecondsWaiting = -1;
    this->scheduleUpdate();
    return true;
}


void HelloWorld::update(float delta) {

    totalMillisecondsWaiting += delta;
    if (totalMillisecondsWaiting > 1000 && futureResult.status() != firebase::kFutureStatusComplete){
        log("Some error happened");

        if (futureResult.status() == firebase::kFutureStatusPending){
            log("still pending");
        } else if (futureResult.status() == firebase::kFutureStatusInvalid) {
            log("request invalid");
        }
    }
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
