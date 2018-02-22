#include "Item.h"
#include "MessageEvents.h"

Item::Item() {
	SetTag("Item");
};

void Item::Render(bool render) {
	if (render == isRendered) return;

	if (render) {
		MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(this));
		isRendered = true;
	}
	else {
		MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(this));
		isRendered = false;
	}
}
