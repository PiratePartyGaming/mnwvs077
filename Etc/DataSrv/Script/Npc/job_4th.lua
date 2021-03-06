
require "./DataSrv/Script/sysDef"

function changeJob4(jobCode, skillLearnItem)
{
    nPSP = (target->getLevel() - 120) * 3;
    if(target->getSP(0) > nPSP)
        self->say("你似乎還有剩餘的#b技能點數#k尚未使用完畢！請把剩餘的技能點數使用完畢後再來與我對話。");
    else
    {
        if(inventory->exchange(0, skillLearnItem, 1) != 0)
        {
            self->say("請確保背包的消耗欄還有足夠的空間。");
            return;
        }
        target->setJob(jobCode + 1);
        target->incAP(5);
        target->incSP(3);

        nJobType = math.floor(jobCode / 100);
        if(nJobType == 2)
        {
            if(jobCode == 111)
                self->sayNext("恭喜你成為了劍士世界的#b終極英雄#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 121)
                self->sayNext("恭喜你成為了劍士世界的#b光之聖騎士#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 131)
                self->sayNext("恭喜你成為了劍士世界的#b黑騎士#k。開始發揮魔力的無限潛能吧！");
            self->sayNext("這並非劍士之路的終點，你的劍士之路將取決於你如何鍛鍊的自己。");
        }
        else if(nJobType == 2)
        {
            if(jobCode == 211)
                self->sayNext("恭喜你成為了魔法世界的#b大魔導士（火、毒）#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 221)
                self->sayNext("恭喜你成為了魔法世界的#b大魔導士（冰、雷）#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 231)
                self->sayNext("恭喜你成為了魔法世界的#b主教#k。開始發揮魔力的無限潛能吧！");
            self->sayNext("這並非法師之路的終點，你的法師之路將取決於你如何鍛鍊的自己。");
        }
        else if(nJobType == 3)
        {
            if(jobCode == 311)
                self->sayNext("恭喜你成為了弓箭手世界的#b箭神#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 321)
                self->sayNext("恭喜你成為了弓箭手世界的#b神射手#k。開始發揮魔力的無限潛能吧！");
            self->sayNext("這並非弓箭手之路的終點，你的弓箭手之路將取決於你如何鍛鍊的自己。");
        }
        else if(nJobType == 4)
        {
            if(jobCode == 411)
                self->sayNext("恭喜你成為了盜賊世界的#b夜使��#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 421)
                self->sayNext("恭喜你成為了盜賊世界的#b暗影神偷#k。開始發揮魔力的無限潛能吧！");
            self->sayNext("這並非盜賊之路的終點，你的盜賊之路將取決於你如何鍛鍊的自己。");
        }
        else if(nJobType == 5)
        {
            if(jobCode == 511)
                self->sayNext("恭喜你成為了海盜世界的#b拳霸#k。開始發揮魔力的無限潛能吧！");
            else if(jobCode == 521)
                self->sayNext("恭喜你成為了海盜世界的#b槍神#k。開始發揮魔力的無限潛能吧！");
            self->sayNext("這並非海盜之路的終點，你的海盜之路將取決於你如何鍛鍊的自己。");
        }
    }
}