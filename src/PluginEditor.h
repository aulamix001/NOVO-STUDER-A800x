#pragma once
#include "PluginProcessor.h"
class A800Editor final : public juce::AudioProcessorEditor {
public:
 explicit A800Editor(A800Processor&);
 ~A800Editor() override;
 void resized() override;
private:
 struct Canvas;
 std::unique_ptr<Canvas> canvas;
 juce::TooltipWindow tooltips{this,500};
};
