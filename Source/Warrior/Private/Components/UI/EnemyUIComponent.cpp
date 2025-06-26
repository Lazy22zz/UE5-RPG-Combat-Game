// ALL FREE


#include "Components/UI/EnemyUIComponent.h"
#include "Widgets/WarriorWidgetBase.h"

void UEnemyUIComponent::RegisteredDrawnWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	if (InWidgetToRegister)
	{
		EnemyDrawnWidgets.Add(InWidgetToRegister);
	}

	return;
}

void UEnemyUIComponent::RemoveAnyEnemyWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	for (UWarriorWidgetBase* DrawnWidgets : EnemyDrawnWidgets)
	{
		if (DrawnWidgets)
		{
			DrawnWidgets->RemoveFromParent();
		}
	}

	EnemyDrawnWidgets.Empty();
}
